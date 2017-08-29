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
 * @file    timing.c
 * @author  foxBMS Team
 * @date    03.02.2016 (date of creation)
 * @ingroup DRIVERS
 * @prefix  TMG
 *
 * @brief   Functions for the MCU to manage time and interrupts
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
#include "timing.h"

#include "os.h"
/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/
static OS_TIMER_s TMG_osKernelSysTick__to__OS_Timer(uint32_t osKernelTick_to_osTimer);
static uint32_t TMG_OS_Timer__to__osKernelSysTick(OS_TIMER_s osTimer_to_osKernelTick);
static uint32_t TMG_calculateTimeDifferenceSimple(uint32_t measurementTimeOlder, uint32_t measurementTimeNewer);

/*================== Function Implementations =============================*/

/*================== Public functions =====================================*/

uint32_t TMG_TimingViolationSimple(uint32_t measurementTimeOlder, uint32_t measurementTimeNewer, uint32_t allowedDelay) {
    STD_RETURN_TYPE_e  timingViolation = E_NOT_OK;
    uint32_t timeDifference = 0xFFFFFFFF;
    timeDifference = TMG_calculateTimeDifferenceSimple(measurementTimeNewer, measurementTimeOlder);
    if (timeDifference <= allowedDelay) {
        timingViolation = E_OK;
    }
    return timingViolation;
}


OS_TIMER_s TMG_calculateTimeDifference(OS_TIMER_s measurementTimeOlder, OS_TIMER_s measurementTimeNewer) {
    uint32_t timeOlder = 0xFFFFFFFF;
    uint32_t timeNewer = 0xFFFFFFFF;
    uint32_t timeDifferenceSimple = 0xFFFFFFFF;
    OS_TIMER_s timeDifference = {0, 0, 0, 0, 0, 0, 0};

    timeOlder = TMG_OS_Timer__to__osKernelSysTick(measurementTimeOlder);
    timeNewer = TMG_OS_Timer__to__osKernelSysTick(measurementTimeNewer);
    timeDifferenceSimple = TMG_calculateTimeDifferenceSimple(timeOlder, timeNewer);
    timeDifference = TMG_osKernelSysTick__to__OS_Timer(timeDifferenceSimple);
    return timeDifference;
}


STD_RETURN_TYPE_e TMG_TimingViolation(OS_TIMER_s measurementTime, OS_TIMER_s currentTime, OS_TIMER_s allowedDelay) {
    STD_RETURN_TYPE_e  timingViolation = E_NOT_OK;
    uint32_t sysTick_measurementTime = 0xFFFFFFFF;
    uint32_t sysTick_currentTime = 0xFFFFFFFF;
    uint32_t sysTick_delay = 0xFFFFFFFF;

    sysTick_measurementTime = TMG_OS_Timer__to__osKernelSysTick(measurementTime);
    sysTick_currentTime = TMG_OS_Timer__to__osKernelSysTick(currentTime);
    sysTick_delay = TMG_OS_Timer__to__osKernelSysTick(allowedDelay);
    timingViolation = TMG_TimingViolationSimple(sysTick_measurementTime, sysTick_currentTime, sysTick_delay);
    return timingViolation;
}
/*================== Static functions =====================================*/
/**
 * @brief   converts osKernelTick into OS_Timer format
 *
 * @param   osKernelTick_to_osTimer
 *
 * @return  osKernel in OS_TIMER format
 */
static OS_TIMER_s TMG_osKernelSysTick__to__OS_Timer(uint32_t osKernelTick_to_osTimer) {
    OS_TIMER_s osTimer = {0, 0, 0, 0, 0, 0, 0};
    osTimer.Timer_d     = (osKernelTick_to_osTimer)/86400000;
    osTimer.Timer_h     = (osKernelTick_to_osTimer%86400000)/3600000;
    osTimer.Timer_min   = (osKernelTick_to_osTimer%3600000)/60000;
    osTimer.Timer_sec   = (osKernelTick_to_osTimer%60000)/1000;
    osTimer.Timer_100ms = (osKernelTick_to_osTimer%1000)/100;
    osTimer.Timer_10ms  = (osKernelTick_to_osTimer%100)/10;
    osTimer.Timer_1ms   = (osKernelTick_to_osTimer%10)/1;
    return osTimer;
}

/**
 * @brief   converts OS_TIMER into osKernelTick format
 *
 * @param   osTimer_to_osKernelTick
 *
 * @return  osTimer in osKernelTick format
 */
static uint32_t TMG_OS_Timer__to__osKernelSysTick(OS_TIMER_s osTimer_to_osKernelTick) {
    uint32_t kernelTick = 0;
    kernelTick += osTimer_to_osKernelTick.Timer_1ms;
    kernelTick += (osTimer_to_osKernelTick.Timer_10ms * 10);
    kernelTick += (osTimer_to_osKernelTick.Timer_100ms * 100);
    kernelTick += (osTimer_to_osKernelTick.Timer_sec * 1000);
    kernelTick += (osTimer_to_osKernelTick.Timer_min * 60000);
    kernelTick += (osTimer_to_osKernelTick.Timer_h *3600000);
    kernelTick += (osTimer_to_osKernelTick.Timer_d * 86400000);
    return kernelTick;
}


/**
 * @brief   calculates the time difference between two measurement timestamps
 *
 * @param   measurementTimeOlder:   older measurement time
 * @param   measurementTimeNewer:   newer measurement time
 *
 * @return  time difference between the two measurement timestamps
 */
static uint32_t TMG_calculateTimeDifferenceSimple(uint32_t measurementTimeOlder, uint32_t measurementTimeNewer) {
    uint32_t timeDifference = 0xFFFFFFFF;
    timeDifference = measurementTimeNewer - measurementTimeOlder;
    return timeDifference;
}
