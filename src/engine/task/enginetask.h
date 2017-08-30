/**
 *
 * @copyright &copy; 2010 - 2017, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
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
 * @file    enginetask.h
 * @author  foxBMS Team
 * @date    27.08.2015 (date of creation)
 * @ingroup ENGINE
 * @prefix  ENG
 *
 * @brief   Header for external functions of the  engine task
 *
 */

#ifndef ENGINETASK_H_
#define ENGINETASK_H_

/*================== Includes =============================================*/
#include "enginetask_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/

/**
 * @brief   Initializes modules that were not initialized before scheduler start.
 *
 * This function is called after the scheduler started but before any cyclic task runs.
 * Here modules get initialized that are not used during the startup process.
 *
 * @return  void
 */
extern void ENG_Init(void);


/**
 * @brief   Cyclic 1ms task for the LTC measurement
 *
 * @return  void
 */
extern void ENG_TSK_Cyclic_1ms(void);

/**
 * @brief   Task for system- and bmscontrol and SOX algorithms
 *
 * @return  void
 */
extern void ENG_TSK_Cyclic_10ms(void);

/**
 * @brief   Task for ADC control, balancing control and isolation
 *          measurement
 *
 * @return  void
 */
extern void ENG_TSK_Cyclic_100ms(void);

/**
 * @brief   Engine Task for handling of events
 *
 * @return  void
 */
extern void ENG_TSK_EventHandler(void);

/**
 * @brief   Engine Task for diagnosis
 *
 * @return  void
 */
extern void ENG_TSK_Diagnosis(void);
/*================== Function Implementations =============================*/

#endif /* ENGINETASK_H_ */
