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
 * @file    enginetask_cfg.c
 * @author  foxBMS Team
 * @date    26.08.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  ENG
 *
 * @brief   Engine task configuration
 * FIXME file description
 */

/*================== Includes =============================================*/
#include "enginetask_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/
BMS_Task_Definition_s eng_tskdef_cyclic_1ms   = {0, 1, osPriorityHigh, 1024 / 4};
BMS_Task_Definition_s eng_tskdef_cyclic_10ms  = {5, 10, osPriorityAboveNormal, 1024 / 4};
BMS_Task_Definition_s eng_tskdef_cyclic_100ms = {53, 100, osPriorityNormal, 1024 / 4};

static BMS_Task_Definition_s eng_tskdef_engine = {0, 1, osPriorityRealtime, 1024 / 4};

/**
 * Definition of task handle of the engine task
 */
static xTaskHandle eng_handle_engine;

/**
 * Definition of task handle 1 millisecond task
 */
static xTaskHandle eng_handle_tsk_1ms;

/**
 * Definition of task handle 10 milliseconds task
 */
static xTaskHandle eng_handle_tsk_10ms;

/**
 * Definition of task handle 100 milliseconds task
 */
static xTaskHandle eng_handle_tsk_100ms;
/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

void ENG_CreateMutex(void) {

}

void ENG_CreateEvent(void) {

}

void ENG_CreateTask(void) {
    // Database Task
    osThreadDef(TSK_Engine, (os_pthread )OS_TSK_Engine,
            eng_tskdef_engine.Priority, 0, eng_tskdef_engine.Stacksize);
    eng_handle_engine = osThreadCreate(osThread(TSK_Engine), NULL);

    // Cyclic Task 1ms
    osThreadDef(TSK_Cyclic_1ms, (os_pthread )OS_TSK_Cyclic_1ms,
            eng_tskdef_cyclic_1ms.Priority, 0, eng_tskdef_cyclic_1ms.Stacksize);
    eng_handle_tsk_1ms = osThreadCreate(osThread(TSK_Cyclic_1ms), NULL);

    // Cyclic Task 10ms
    osThreadDef(TSK_Cyclic_10ms, (os_pthread )OS_TSK_Cyclic_10ms,
            eng_tskdef_cyclic_10ms.Priority, 0, eng_tskdef_cyclic_10ms.Stacksize);
    eng_handle_tsk_10ms = osThreadCreate(osThread(TSK_Cyclic_10ms), NULL);

    // Cyclic Task 100ms
    osThreadDef(TSK_Cyclic_100ms, (os_pthread )OS_TSK_Cyclic_100ms,
            eng_tskdef_cyclic_100ms.Priority, 0, eng_tskdef_cyclic_100ms.Stacksize);
    eng_handle_tsk_100ms = osThreadCreate(osThread(TSK_Cyclic_100ms), NULL);
}
