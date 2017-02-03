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
 * @file    timing.h
 * @author  foxBMS Team
 * @date    03.02.2016 (date of creation)
 * @ingroup DRIVERS
 * @prefix  TMG
 *
 * @brief   Headers for the functions for the MCU to manage time and interrupts.
 *
 */

#ifndef UTILS_TIMING_H_
#define UTILS_TIMING_H_


/*================== Includes =============================================*/
#include "os.h"
/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/

/**
 * @brief   returns the time difference between to time stamps as OS_TIMER_s struct
 *
 * @param   measurementTimeOlder (type: OS_TIMER_s)
 * @param   measurementTimeNewer (type: OS_TIMER_s)
 *
 * @return  timeDifference (type: OS_TIMER_s)
 */
extern OS_TIMER_s TMG_calculateTimeDifference(OS_TIMER_s measurementTimeOlder, OS_TIMER_s measurementTimeNewer);

/**
 * @brief   returns E_OK if there is no timing violation, return E_NOT_OK if there is timing violation
 *
 * @param   measurementTime (type: OS_TIMER_s)
 * @param   currentTime (type: OS_TIMER_s)
 * @param   allowedDelay (type: CONT_SWITCH_e)
 *
 * @return  timingViolation (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e TMG_TimingViolation(OS_TIMER_s measurementTime, OS_TIMER_s currentTime, OS_TIMER_s allowedDelay);

/**
 * @brief returns E_OK if there is no timing violation, return E_NOT_OK if there is timing violation
 * use osKernelSysTick() for the time stamps.
 * @param   measurementTimeOlder (type: uint32_t)
 * @param   measurementTimeNewer (type: uint32_t)
 * @param   allowedDelay (type: uint32_t)
 *
 * @return timingViolation (type: STD_RETURN_TYPE_e)
 */
extern uint32_t TMG_TimingViolationSimple(uint32_t measurementTimeOlder, uint32_t measurementTimeNewer, uint32_t allowedDelay);
/*================== Function Implementations =============================*/

#endif   // UTILS_TIMING_H_
