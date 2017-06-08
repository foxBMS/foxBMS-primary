/**
 *
 * @copyright &copy; 2010 - 2016, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
 *
 * BSD 3-Clause License
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * We kindly request you to use one or more of the following phrases to refer to foxBMS in your hardware, software, documentation or advertising materials:
 *
 * &Prime;This product uses parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product includes parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product is derived from foxBMS&reg;&Prime;
 *
 */

/**
 * @file    os.c
 * @author  foxBMS Team
 * @date    27.08.2015 (date of creation)
 * @ingroup OS
 * @prefix  OS
 *
 * @brief   Implementation of the tasks used by the system
 *
 */

/*================== Includes =============================================*/
/* recommended include order of header files:
 * 
 * 1.    include general.h
 * 2.    include module's own header
 * 3...  other headers
 *
 */
#include "general.h"
#include "os.h"

#include "enginetask.h"
#include "appltask.h"
#include "database.h"
#include "syscontrol.h"
#include "os.h"
#include "eepr.h"
#include "nvic.h"
#include "cansignal.h"
#include "can.h"
#include "diag.h"
#include "sdram.h"
/*================== Macros and Definitions ===============================*/


/*================== Constant and Variable Definitions ====================*/
volatile OS_BOOT_STATE_e os_boot;
volatile OS_BOOT_STATE_e os_safety_state;
volatile OS_TIMER_s os_timer;

/**
 * Scheduler "zero" time for task phase control
 */
uint32_t os_schedulerstarttime;

/*================== Function Prototypes ==================================*/
static void OS_TimerTrigger(void);

/*================== Function Implementations =============================*/

void OS_TaskInit() {

    // Configuration of RTOS Queues
    os_boot = OS_ENG_CREATE_QUEUES;
    ENG_CreateQueues();

    // Configuration of RTOS Mutexes
    os_boot = OS_ENG_CREATE_MUTEX;
    ENG_CreateMutex();

    // Configuration of RTOS Events
    os_boot = OS_ENG_CREATE_EVENT;
    ENG_CreateEvent();
    
    // Configuration of RTOS Tasks
    os_boot = OS_ENG_CREATE_TASKS;
    ENG_CreateTask();

    // Configuration of RTOS Mutexes
    os_boot = OS_APPL_CREATE_MUTEX;
    APPL_CreateMutex();

    // Configuration of RTOS Events
    os_boot = OS_APPL_CREATE_EVENT;
    APPL_CreateEvent();
    
    // Configuration of RTOS Tasks
    os_boot = OS_APPL_CREATE_TASKS;
    APPL_CreateTask();
}

void vApplicationIdleHook(void) {

    OS_IdleTask();
}


void OS_TSK_Engine(void) {

    OS_PostOSInit();

    ENG_Init();
    os_boot = OS_SYSTEM_RUNNING;

    for(;;) {

        DATA_Task();    /* Call database manager */
        DIAG_SysMon();  /* Call Overall System Monitoring */
    }
}


void OS_TSK_Cyclic_1ms(void) {

    while (os_boot != OS_SYSTEM_RUNNING) {
        ;
    }

    osDelayUntil(&os_schedulerstarttime, eng_tskdef_cyclic_1ms.Phase);

    while(1) {

        uint32_t currentTime = osKernelSysTick();

        OS_TimerTrigger();        // Increment system timer os_timer
        BKPSRAM_OperatingHoursTrigger(); // Increment operating hours timer
        ENG_TSK_Cyclic_1ms();

        osDelayUntil(&currentTime, eng_tskdef_cyclic_1ms.CycleTime);
    }

}


void OS_TSK_Cyclic_10ms(void) {

    while (os_boot != OS_SYSTEM_RUNNING) {
        ;
    }

    osDelayUntil(&os_schedulerstarttime, eng_tskdef_cyclic_10ms.Phase);

    while(1) {

        uint32_t currentTime = osKernelSysTick();

        ENG_TSK_Cyclic_10ms();

        osDelayUntil(&currentTime, eng_tskdef_cyclic_10ms.CycleTime);
    }

}

void OS_TSK_Cyclic_100ms(void) {

    while (os_boot != OS_SYSTEM_RUNNING) {
        ;
    }
    osDelayUntil(&os_schedulerstarttime, eng_tskdef_cyclic_100ms.Phase);

    while(1) {

        uint32_t currentTime = osKernelSysTick();

        ENG_TSK_Cyclic_100ms();

        osDelayUntil(&currentTime, eng_tskdef_cyclic_100ms.CycleTime);
    }

}


void OS_TSK_EventHandler(void) {

    while (os_boot != OS_SYSTEM_RUNNING) {
        ;
    }
    osDelayUntil(&os_schedulerstarttime, eng_tskdef_eventhandler.Phase);

    while(1) {

        uint32_t currentTime = osKernelSysTick();

        ENG_TSK_EventHandler();

        osDelayUntil(&currentTime, eng_tskdef_eventhandler.CycleTime);
    }
}

void OS_TSK_Diagnosis(void) {

    while (os_boot != OS_SYSTEM_RUNNING) {
        ;
    }
    osDelayUntil(&os_schedulerstarttime, eng_tskdef_diagnosis.Phase);

    while(1) {

        uint32_t currentTime = osKernelSysTick();

        ENG_TSK_Diagnosis();

        osDelayUntil(&currentTime, eng_tskdef_diagnosis.CycleTime);
    }

}



void OS_PostOSInit(void) {

    uint16_t timeout_cnt = 0;
    uint8_t err_type = 0;
    STD_RETURN_TYPE_e ret_val = E_NOT_OK;
    EEPR_RETURNTYPE_e eepr_ret_val = EEPR_ERROR;

    os_boot = OS_RUNNING;

    NVIC_PostOsInit();
#ifdef HAL_SDRAM_MODULE_ENABLED
    SDRAM_Init();
#endif
    CAN_Init();

    os_boot = OS_EEPR_INIT;

    // initialize eeprom driver
    timeout_cnt = 0;
    while (EEPR_GetState() != EEPR_IDLE) {

        if (timeout_cnt > 50) {   // timeout: 50ms (normal needed time: 5ms)

           // eeprom not usable, no eeprom data available
            err_type|=0x01;     // possible reasons: SPI busy, eeprom hardware in busy state or defect
            break;
        }

        timeout_cnt++;
        EEPR_Trigger();
        osDelay(1);             // wait 1ms for next call of eeprom trigger
    }

    if(err_type == 0) {

        // read eeprom data
        eepr_ret_val = EEPR_SetStateRequest(EEPR_READMEMORY);    // Read EEPROM
        if(eepr_ret_val != EEPR_OK)
            err_type|=0x02;

        timeout_cnt = 0;
        while (EEPR_GetState() != EEPR_IDLE) {

            if (timeout_cnt > EEPR_GetReadTime()) {   // timeout (depending on data length)

                err_type|=0x04;
                DIAG_Handler(DIAG_CH_POSTOSINIT_FAILURE,DIAG_EVENT_NOK,0, NULL); // eeprom data not readable
                break;
            }

            timeout_cnt++;
            EEPR_Trigger();
            osDelay(1);      //wait 1ms for next call of eeprom trigger
        }
    } else {

        DIAG_Handler(DIAG_CH_POSTOSINIT_FAILURE, DIAG_EVENT_NOK, 1, NULL);   // no initialization of eeprom hardware possible
    }

    if(err_type == 0) {
        // eeprom has been read succesfully

        if((RTC_NVMRAM_DATAVALID_VARIABLE == 0) || (EEPR_BkpSramCheckChksum() != 0) ) {
            // NVRAM data corrupt but eeprom data is ok, so do data recovery and overtake data from eeprom
            EEPR_BkpSramDataRecovery();
            RTC_NVMRAM_DATAVALID_VARIABLE = 1;      // validate NVNRAM data
        } else {

           //both areas are valid so take NVRAM data and update eeprom values
            EEPR_UpdateEepromData();      // do update eeprom data
            eepr_ret_val = EEPR_SetStateRequest(EEPR_WRITEMEMORY);    // Write EEPROM
            if(eepr_ret_val != EEPR_OK)
                err_type|=0x08;

            timeout_cnt = 0;
            while (EEPR_GetState() != EEPR_IDLE) {

                if (timeout_cnt > EEPR_GetWriteTime()) {   // timeout (depending on data length)

                    err_type|=0x10;
                    DIAG_Handler(DIAG_CH_POSTOSINIT_FAILURE,DIAG_EVENT_NOK, 2, NULL); // eeprom data not writable
                    break;
                }

                timeout_cnt++;
                EEPR_Trigger();
                osDelay(1);      //wait 1ms for next call of eeprom trigger
            }
        }
    } else {
        // no eeprom data available

        if((RTC_NVMRAM_DATAVALID_VARIABLE == 0) || (EEPR_BkpSramCheckChksum() != 0) ) {

            // and NVM data corrupt, so take default values
            EEPR_BkpSramDefaultDataRecovery();
            RTC_NVMRAM_DATAVALID_VARIABLE = 1;      // validate NVNRAM data
        }

        if(err_type == 0x04) {

            //eeprom read error (checksum error)
            EEPR_UpdateEepromData();      // do update eeprom data
            eepr_ret_val = EEPR_SetStateRequest(EEPR_WRITEMEMORY);    // Write EEPROM
            if(eepr_ret_val != EEPR_OK)
                err_type|=0x08;

            timeout_cnt = 0;
            while (EEPR_GetState() != EEPR_IDLE) {

                if (timeout_cnt > EEPR_GetWriteTime()) {   // timeout (depending on data length)

                    err_type|=0x10;
                    DIAG_Handler(DIAG_CH_POSTOSINIT_FAILURE,DIAG_EVENT_NOK,3, NULL); // eeprom data not writable
                    break;
                }

                timeout_cnt++;
                EEPR_Trigger();
                osDelay(1);      //wait 1ms for next call of eeprom trigger
            }
        }
    }

    os_boot = OS_SYSCTRL_INIT;
    // Init SysControl:
    ret_val = SYSCTRL_SetStateRequest(SYSCTRL_STATE_REQ_STANDBY);
    if(ret_val)
        err_type|=0x40;

    timeout_cnt = 0;

    while ((SYSCTRL_GetState() != SYSCTRL_STATE_IDLE) && (SYSCTRL_GetState() != SYSCTRL_STATE_STANDBY)) {

        if (timeout_cnt > 20) {   // timeout

            err_type|=0x80;
            break;
        }

        timeout_cnt++;
        SYSCTRL_Trigger(SYS_MODE_CYCLIC_EVENT);
        osDelay(10);
    }

    if(ret_val) {

         DIAG_Handler(DIAG_CH_POSTOSINIT_FAILURE, DIAG_EVENT_NOK, 3, NULL);
    }
}

void OS_IdleTask(void) {
    ;
}



/**
 * @brief   increments the system timer os_timer
 *
 * The os_timer is a runtime-counter, counting the time since the last reset.
 *
 * @return  void
 */
static void OS_TimerTrigger(void) {

    if(++os_timer.Timer_1ms > 9 ) {
        // 10ms
        os_timer.Timer_1ms=0;

          if(++os_timer.Timer_10ms > 9) {
              // 100ms
              os_timer.Timer_10ms=0;

                if(++os_timer.Timer_100ms > 9) {
                    // 1s
                    os_timer.Timer_100ms=0;

                      if(++os_timer.Timer_sec > 59) {
                          // 1min
                          os_timer.Timer_sec=0;

                           if(++os_timer.Timer_min > 59) {
                               // 1h
                               os_timer.Timer_min=0;

                                 if(++os_timer.Timer_h > 23) {
                                     // 1d
                                     os_timer.Timer_h=0;
                                     ++os_timer.Timer_d;
                                 }
                           }
                      }
                }
          }
    }
}


