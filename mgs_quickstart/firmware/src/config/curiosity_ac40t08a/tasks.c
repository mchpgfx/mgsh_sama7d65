/*******************************************************************************
 System Tasks File

  File Name:
    tasks.c

  Summary:
    This file contains source code necessary to maintain system's polled tasks.

  Description:
    This file contains source code necessary to maintain system's polled tasks.
    It implements the "SYS_Tasks" function that calls the individual "Tasks"
    functions for all polled MPLAB Harmony modules in the system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    polled in the system.  These handles are passed into the individual module
    "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"
#include "sys_tasks.h"

//CUSTOM CODE - DO NOT REMOVE OR MODIFY ANYTHING BETWEEN CUSTOM CODE MARKERS!!!
#include <stdio.h>
#include "task.h"

//#define SHOW_RTOS_IDLE_TASK 1

enum
{
    APP_TASK_ID,
    LEGATO_TASK_ID,
    MXT_TOUCH_TASK_ID,
    XLCDC_TASK_ID,
    SYS_INPUT_TASK_ID,
    DISP_TASK_ID,
#ifdef SHOW_RTOS_IDLE_TASK
    IDLE_TASK_ID,
#endif
    MAX_TASK_ID
};

typedef struct
{
    TaskHandle_t handle;
    char * name;
    char * rtosName;
    uint32_t lastCount;
    uint32_t count;
} APP_TASK_STRUCT_t;

APP_TASK_STRUCT_t tasks[MAX_TASK_ID] =
{
    [MXT_TOUCH_TASK_ID] =
    {
        .name = "Touch Task",
        .rtosName = "DRV_MAXTOUCH_Ta",  /* truncated to configMAX_TASK_NAME_LEN (16) */
    },
    [XLCDC_TASK_ID] =
    {
        .name = "Display Task",
        .rtosName = "XLCDC_Tasks",
    },
    [LEGATO_TASK_ID] =
    {
        .name = "GFX Task",
        .rtosName = "LEGATO_Tasks",
    },
    [SYS_INPUT_TASK_ID] =
    {
        .name = "Input Task",
        .rtosName = "SYS_INPUT_Tasks",
    },
    [DISP_TASK_ID] =
    {
        .name = "DISP Task",
        .rtosName = "DISP_Tasks",
    },
    [APP_TASK_ID] = {
        .name = "User IDLE Task",
        .rtosName = "APP_DSI_Tasks",
    },
#ifdef SHOW_RTOS_IDLE_TASK
    [IDLE_TASK_ID] = {
        .name = "RTOS IDLE Task",
    },
#endif
};

extern uint32_t leGetScratchBufferSizeKB(void);

static void Task_Init(void)
{
    unsigned int i;
    for (i = 0; i < MAX_TASK_ID; i++)
    {
        if (tasks[i].handle == NULL && tasks[i].rtosName != NULL)
        {
            tasks[i].handle = xTaskGetHandle(tasks[i].rtosName);
            if (tasks[i].handle == NULL)
            {
                printf("WARNING: Task_Init: xTaskGetHandle(\"%s\") returned NULL"
                       " - check configMAX_TASK_NAME_LEN (%d)\n\r",
                       tasks[i].rtosName, configMAX_TASK_NAME_LEN);
            }
        }
    }
#ifdef SHOW_RTOS_IDLE_TASK
    tasks[IDLE_TASK_ID].handle = xTaskGetIdleTaskHandle();
#endif
}

unsigned int Task_Usage(void)
{
    static uint32_t ulLastTotalTime = 0;
    static int initialized = 0;
    unsigned int i = 0;
    uint32_t ulTotalTime;
    uint32_t pctRunTime;
    TaskStatus_t xTaskDetails;
    uint32_t app_usage = 0;

    if (!initialized)
    {
        Task_Init();
        initialized = 1;
    }

    //gather the new total time
    for (i = 0; i < MAX_TASK_ID; i++)
    {
        // Use the handle to obtain further information about the task.
        vTaskGetInfo( tasks[i].handle,
                      &xTaskDetails,
                      pdTRUE, // Include the high water mark in xTaskDetails.
                      eInvalid ); // Include the task state in xTaskDetails.

        tasks[i].count = xTaskDetails.ulRunTimeCounter;
    }

    printf("\n\r........................................... \n\r");
    printf("Task Usage \t%%   @ %u fps, %ukB sBuff\n\r",
                fps,
                (unsigned int) leGetScratchBufferSizeKB());
    printf("........................................... \n\r");

    ulTotalTime = portGET_RUN_TIME_COUNTER_VALUE(); /* get total time passed in system */

    for (i = 0; i < MAX_TASK_ID; i++)
    {
        pctRunTime = (((tasks[i].count - tasks[i].lastCount) * 100)/(ulTotalTime - ulLastTotalTime));
        pctRunTime = (pctRunTime > 100) ? 100 : pctRunTime;

        if (pctRunTime > 0)
        {
            printf("%.20s \t%u",
                    tasks[i].name,
                    (unsigned int) pctRunTime);
        }
        else
        {
            printf("%.20s \t<1",
                    tasks[i].name);
        }
        tasks[i].lastCount = tasks[i].count;

        if (i == APP_TASK_ID)
        {
            app_usage = pctRunTime;
            printf(" (free) \n\r");
        }
        else
        {
            printf("\n\r");
        }
    }

    ulLastTotalTime = ulTotalTime;

    return app_usage;

}
//END OF CUSTOM CODE

// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************
/* CUSTOM CHANGE - vTaskDelay set to 0 for benchmark max throughput.
   MCC will regenerate with non-zero delays. Change all back to 0,
   including lAPP_DSI_Tasks below. */
void _LEGATO_Tasks(  void *pvParameters  )
{
    while(1)
    {
        Legato_Tasks();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void _SYS_INPUT_Tasks(  void *pvParameters  )
{
    while(1)
    {
        SYS_INP_Tasks();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void _XLCDC_Tasks(  void *pvParameters  )
{
    while(1)
    {
        DRV_XLCDC_Update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void _DISP_Tasks(  void *pvParameters  )
{
    while(1)
    {
        DISP_Update();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void _DRV_MAXTOUCH_Tasks(  void *pvParameters  )
{
    while(1)
    {
        DRV_MAXTOUCH_Tasks(sysObj.drvMAXTOUCH);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


/* Handle for the APP_DSI_Tasks. */
TaskHandle_t xAPP_DSI_Tasks;



static void lAPP_DSI_Tasks(  void *pvParameters  )
{   
    while(true)
    {
        APP_DSI_Tasks();
        vTaskDelay(0U / portTICK_PERIOD_MS);
    }
}




// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    

    /* Maintain Device Drivers */
    
    xTaskCreate( _XLCDC_Tasks,
        "XLCDC_Tasks",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );


    xTaskCreate( _DRV_MAXTOUCH_Tasks,
        "DRV_MAXTOUCH_Tasks",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );



    /* Maintain Middleware & Other Libraries */
    
    xTaskCreate( _LEGATO_Tasks,
        "LEGATO_Tasks",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );


    xTaskCreate( _SYS_INPUT_Tasks,
        "SYS_INPUT_Tasks",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );


    xTaskCreate( _DISP_Tasks,
        "DISP_Tasks",
        1024,
        (void*)NULL,
        1,
        (TaskHandle_t*)NULL
    );



    /* Maintain the application's state machine. */
    
    /* Create OS Thread for APP_DSI_Tasks. */
    (void) xTaskCreate(
           (TaskFunction_t) lAPP_DSI_Tasks,
           "APP_DSI_Tasks",
           1024,
           NULL,
           1U ,
           &xAPP_DSI_Tasks);



    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

/*******************************************************************************
 End of File
 */

