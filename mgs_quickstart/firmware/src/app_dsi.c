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
