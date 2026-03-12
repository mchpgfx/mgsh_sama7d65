/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_dsi.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_DSI_Initialize" and "APP_DSI_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_DSI_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_DSI_H
#define _APP_DSI_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "definitions.h"

#ifdef RTOS_ENABLED
#define CLOCK_TICK_TIMER_PERIOD_MS 10
#else
#define CLOCK_TICK_TIMER_PERIOD_MS 30
#endif

#define NUM_COUNT_SEC_TICK (1000/CLOCK_TICK_TIMER_PERIOD_MS)
#define NUM_COUNT_TAP_TICK (200/CLOCK_TICK_TIMER_PERIOD_MS) 

#define FPS_STR_SIZE 32

#define BENCH_CMD_BUF_SIZE 32
#define BENCH_DWELL_SECS   5
#define BENCH_SETTLE_SECS  1
#define BENCH_TOTAL_CONFIGS 28
    
// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

extern int clock_sec;
extern int clock_min;
extern int clock_hr;

extern unsigned int idle_secs;

extern volatile unsigned int tick_count;
extern unsigned int tick_count_last;
extern volatile unsigned int sec_count;
extern int last_sec_count;
extern unsigned int fps;
extern unsigned int cpu_free;
extern char fpsStrBuff[];
extern bool stats_enabled;
extern leChar fpsStrCharBuff[];
extern leFixedString fpsStr;

extern bool bench_active;

typedef struct
{
    uint32_t screen;       /* 1, 2, or 3 */
    const char* label;     /* human-readable config name */
    uint32_t param1;       /* screen1: size, screen2: count, screen3: type */
    uint32_t param2;       /* screen2: size, screen3: imgSize enum value */
} BENCH_CONFIG_T;

/* Benchmark setter functions (defined in screen files) */
void Screen1_BenchSetCounterSize(uint32_t size);
void Screen2_BenchSetConfig(uint32_t count, uint32_t size);
void Screen3_BenchSetConfig(uint32_t type, uint32_t size);

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_DSI_STATE_INIT=0,
    APP_DSI_STATE_SERVICE_TASKS,
    /* TODO: Define states used by the application state machine. */

} APP_DSI_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_DSI_STATES state;

    /* TODO: Define any additional data used by the application. */

} APP_DSI_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_DSI_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_DSI_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_DSI_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_DSI_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_DSI_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_DSI_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_DSI_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_DSI_H */

/*******************************************************************************
 End of File
 */

