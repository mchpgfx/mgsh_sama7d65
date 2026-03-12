/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_dsi.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_dsi.h"
#include <stdio.h>
#include <string.h>
#ifdef RTOS_ENABLED
#include "task.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

unsigned int idle_secs = 0;
unsigned int demo_mode_count_secs = 0;
unsigned int demo_mode_event_idx = 0;
bool demo_mode_on = true;
bool demo_mode_enabled = false;
volatile unsigned int tick_count = 0;
unsigned int tick_count_last = 0;
volatile unsigned int sec_count = 0;
int last_sec_count = 0;
int clock_sec = 0;
int clock_min = 0;
int clock_hr = 12;
unsigned int last_frame_count = 0;
unsigned int fps;
unsigned int cpu_free;
bool stats_enabled = 0;
static SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;
uint32_t event_parm = 0;
char fpsStrBuff[FPS_STR_SIZE];
leChar fpsStrCharBuff[FPS_STR_SIZE] = {0};

leFixedString fpsStr;

/* Benchmark state */
bool bench_active = false;

typedef enum
{
    BENCH_IDLE = 0,
    BENCH_STARTING,
    BENCH_GOTO_SCREEN,
    BENCH_WAIT_SCREEN,
    BENCH_APPLY_CONFIG,
    BENCH_SETTLE,
    BENCH_DWELL,
    BENCH_NEXT_CONFIG,
    BENCH_FINISHING,
    BENCH_DONE
} BENCH_STATE_T;

static BENCH_STATE_T bench_state = BENCH_IDLE;
static uint32_t bench_config_idx = 0;
static uint32_t bench_settle_start = 0;
static uint32_t bench_dwell_start = 0;
static uint32_t bench_sample_idx = 0;
static uint32_t bench_last_sample_sec = 0;

static char bench_cmd_buf[BENCH_CMD_BUF_SIZE];
static uint32_t bench_cmd_len = 0;

/* 28 benchmark configurations */
static const BENCH_CONFIG_T bench_configs[BENCH_TOTAL_CONFIGS] =
{
    /* Screen 1 — FPS Counter: 5 sizes */
    { 1, "size1", 1, 0 },
    { 1, "size2", 2, 0 },
    { 1, "size3", 3, 0 },
    { 1, "size4", 4, 0 },
    { 1, "size5", 5, 0 },

    /* Screen 2 — Motion Rects: 9 configs (param1=count, param2=size) */
    { 2, "cnt1_sz40",   1, 40 },
    { 2, "cnt1_sz100",  1, 100 },
    { 2, "cnt1_sz200",  1, 200 },
    { 2, "cnt1_full",   1, 0xffffffff },
    { 2, "cnt3_sz40",   3, 40 },
    { 2, "cnt3_sz100",  3, 100 },
    { 2, "cnt5_sz40",   5, 40 },
    { 2, "cnt5_sz100",  5, 100 },
    { 2, "cnt10_sz40", 10, 40 },

    /* Screen 3 — Images: 14 configs (param1=type enum, param2=size enum) */
    /* PNG8888: 40x40, 100x100 */
    { 3, "png8888_40",    0, 0 },
    { 3, "png8888_100",   0, 1 },
    /* JPG24: 40x40, 100x100, 200x200, 480x270 */
    { 3, "jpg24_40",      1, 0 },
    { 3, "jpg24_100",     1, 1 },
    { 3, "jpg24_200",     1, 2 },
    { 3, "jpg24_480",     1, 3 },
    /* RAW565: 40x40, 100x100, 200x200, 480x270 */
    { 3, "raw565_40",     2, 0 },
    { 3, "raw565_100",    2, 1 },
    { 3, "raw565_200",    2, 2 },
    { 3, "raw565_480",    2, 3 },
    /* RAWRLE565: 40x40, 100x100, 200x200, 480x270 */
    { 3, "rawrle565_40",  3, 0 },
    { 3, "rawrle565_100", 3, 1 },
    { 3, "rawrle565_200", 3, 2 },
    { 3, "rawrle565_480", 3, 3 },
};

static void Bench_SendLine(const char* line)
{
    FLEXCOM6_USART_Write((void*)line, strlen(line));
    FLEXCOM6_USART_Write((void*)"\r\n", 2);
}

static void Bench_Reset(void)
{
    bench_state = BENCH_IDLE;
    bench_active = false;
    bench_config_idx = 0;
    bench_settle_start = 0;
    bench_dwell_start = 0;
    bench_sample_idx = 0;
    bench_last_sample_sec = 0;
    bench_cmd_len = 0;
}

static void Bench_ProcessCommand(const char* cmd)
{
    if (strcmp(cmd, "BENCH_PING") == 0)
    {
        Bench_SendLine("[BENCH,PONG]");
    }
    else if (strcmp(cmd, "BENCH_START") == 0)
    {
        /* Always accept: reset any in-progress run and restart */
        if (bench_state != BENCH_IDLE)
        {
            Bench_SendLine("[BENCH,ABORTED]");
        }
        Bench_Reset();
        bench_state = BENCH_STARTING;
    }
    else if (strcmp(cmd, "BENCH_STOP") == 0)
    {
        if (bench_state != BENCH_IDLE)
        {
            Bench_Reset();
            Bench_SendLine("[BENCH,ABORTED]");
        }
    }
}

static void Bench_PollUART(void)
{
    /* Clear any overrun/framing errors so the receiver keeps working.
       Errors occur when bytes arrive while the CPU is busy with blocking
       UART writes (printf, Task_Usage output, etc.). */
    FLEXCOM6_USART_ErrorGet();

    while (FLEXCOM6_USART_ReceiverIsReady())
    {
        uint8_t ch = FLEXCOM6_USART_ReadByte();

        if (ch == '\n' || ch == '\r')
        {
            if (bench_cmd_len > 0)
            {
                bench_cmd_buf[bench_cmd_len] = '\0';
                Bench_ProcessCommand(bench_cmd_buf);
                bench_cmd_len = 0;
            }
        }
        else
        {
            if (bench_cmd_len < BENCH_CMD_BUF_SIZE - 1)
            {
                bench_cmd_buf[bench_cmd_len++] = (char)ch;
            }
        }
    }
}

static void Bench_StateMachine(void)
{
    char out_buf[128];
    const BENCH_CONFIG_T* cfg;
    static uint32_t bench_current_screen = 0;

    switch (bench_state)
    {
        case BENCH_IDLE:
            break;

        case BENCH_STARTING:
            bench_active = true;
            bench_config_idx = 0;
            sprintf(out_buf, "[BENCH,START,%u]", (unsigned int)BENCH_TOTAL_CONFIGS);
            Bench_SendLine(out_buf);
            bench_state = BENCH_GOTO_SCREEN;
            bench_current_screen = 0;
            break;

        case BENCH_GOTO_SCREEN:
            cfg = &bench_configs[bench_config_idx];
            if (legato_getCurrentScreen() != cfg->screen)
            {
                legato_showScreen(cfg->screen);
                bench_state = BENCH_WAIT_SCREEN;
            }
            else
            {
                bench_state = BENCH_APPLY_CONFIG;
            }
            bench_current_screen = cfg->screen;
            break;

        case BENCH_WAIT_SCREEN:
            if (!legato_isChangingScreens() &&
                legato_getCurrentScreen() == bench_current_screen)
            {
                bench_state = BENCH_APPLY_CONFIG;
            }
            break;

        case BENCH_APPLY_CONFIG:
            cfg = &bench_configs[bench_config_idx];
            switch (cfg->screen)
            {
                case 1:
                    Screen1_BenchSetCounterSize(cfg->param1);
                    break;
                case 2:
                    Screen2_BenchSetConfig(cfg->param1, cfg->param2);
                    break;
                case 3:
                    Screen3_BenchSetConfig(cfg->param1, cfg->param2);
                    break;
                default:
                    break;
            }
            sprintf(out_buf, "[BENCH,CFG,%u,%s,%u]",
                    (unsigned int)cfg->screen, cfg->label,
                    (unsigned int)BENCH_DWELL_SECS);
            Bench_SendLine(out_buf);
            bench_settle_start = sec_count;
            bench_state = BENCH_SETTLE;
            break;

        case BENCH_SETTLE:
            if ((sec_count - bench_settle_start) >= BENCH_SETTLE_SECS)
            {
                bench_dwell_start = sec_count;
                bench_sample_idx = 0;
                bench_last_sample_sec = sec_count;
                bench_state = BENCH_DWELL;
            }
            break;

        case BENCH_DWELL:
            cfg = &bench_configs[bench_config_idx];
            if (sec_count != bench_last_sample_sec)
            {
                sprintf(out_buf, "[BENCH,DATA,%u,%s,%u,%u,%u]",
                        (unsigned int)cfg->screen,
                        cfg->label,
                        (unsigned int)bench_sample_idx,
                        (unsigned int)fps,
                        (unsigned int)(100 - cpu_free));
                Bench_SendLine(out_buf);
                bench_sample_idx++;
                bench_last_sample_sec = sec_count;
            }
            if ((sec_count - bench_dwell_start) >= BENCH_DWELL_SECS)
            {
                bench_state = BENCH_NEXT_CONFIG;
            }
            break;

        case BENCH_NEXT_CONFIG:
            bench_config_idx++;
            if (bench_config_idx >= BENCH_TOTAL_CONFIGS)
            {
                bench_state = BENCH_FINISHING;
            }
            else
            {
                bench_state = BENCH_GOTO_SCREEN;
            }
            break;

        case BENCH_FINISHING:
            bench_active = false;
            Bench_SendLine("[BENCH,DONE]");
            bench_state = BENCH_IDLE;
            break;

        case BENCH_DONE:
        default:
            bench_state = BENCH_IDLE;
            break;
    }
}

#ifdef RTOS_ENABLED
extern unsigned int Task_Usage(void);
#endif

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_DSI_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DSI_DATA app_dsiData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

#ifdef RTOS_ENABLED
void RTOS_AppConfigureTimerForRuntimeStats()
{
    //do nothing
    tick_count = 0;
}

uint32_t RTOS_AppGetRuntimeCounterValue(void)
{
    return tick_count;
}
#endif

static void Timer_Callback ( uintptr_t context)
{
    tick_count++;
    
    if (tick_count % NUM_COUNT_SEC_TICK == 0)
    {
        
        if (leRenderer_GetDrawCount() > last_frame_count)
        {
            fps = leRenderer_GetDrawCount() - last_frame_count;
            last_frame_count = leRenderer_GetDrawCount();
        }
        else
        {
            fps = 0;
        }
                
        sec_count++;
        idle_secs++;
        
        clock_sec++;
        if (clock_sec == 60)
        {
            clock_sec = 0;
            clock_min++;
            if (clock_min == 60)
            {
                clock_min = 0;
                clock_hr++;
                if (clock_hr == 24)
                {
                    clock_hr = 0;
                }
            }
        }        
    }    
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_DSI_Initialize ( void )

  Remarks:
    See prototype in app_dsi.h.
 */

void APP_DSI_Initialize ( void )
{ }


/******************************************************************************
  Function:
    void APP_DSI_Tasks ( void )

  Remarks:
    See prototype in app_dsi.h.
 */

void APP_DSI_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( app_dsiData.state )
    {
        /* Application's initial state. */
        case APP_DSI_STATE_INIT:
        {
            bool appInitialized = true;
            stats_enabled = true;
            
            timer = SYS_TIME_CallbackRegisterMS(Timer_Callback, 1, CLOCK_TICK_TIMER_PERIOD_MS, SYS_TIME_PERIODIC);   

            if (appInitialized)
            {

                app_dsiData.state = APP_DSI_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_DSI_STATE_SERVICE_TASKS:
        {
#ifdef RTOS_ENABLED
            static unsigned int sec_count_last;

            if (stats_enabled == true &&
                sec_count != sec_count_last)
            {
                cpu_free = Task_Usage();
                sec_count_last = sec_count;
            }
#endif
            Bench_PollUART();
            Bench_StateMachine();

            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
