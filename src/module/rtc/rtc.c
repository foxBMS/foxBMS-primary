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
 * @file    rtc.c
 * @author  foxBMS Team
 * @date    20.10.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  RTC
 *
 * @brief   Driver for the real time clock.
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
#include "rtc.h"

/*================== Macros and Definitions ===============================*/


/*================== Constant and Variable Definitions ====================*/
RTC_HandleTypeDef hrtc;

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/
void RTC_Init(void) {

    /* Configure RTC clocks */
    HAL_RCC_OscConfig(&rtc_cfg.oscInitStruct);

    HAL_RCCEx_PeriphCLKConfig(&rtc_cfg.clkInitStruct);

    /* Enable RTC clock */
    __HAL_RCC_RTC_ENABLE();

    /* Initialize RTC */
    hrtc.Instance = RTC;
    hrtc.Init = rtc_cfg.initconfig;
    HAL_RTC_Init(&hrtc);

    /* Disable WakeUp-Timer */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    // set time and date if not already initialized or V_BAT failed
    if (!(hrtc.Instance->ISR & (uint32_t)RTC_ISR_INITS)) {

        RTC_DATAVALID_VARIABLE = 0;        // invalidate rtc backup data
        HAL_RTC_SetTime(&hrtc, &rtc_cfg.defaultTime, rtc_cfg.timeformat);

        HAL_RTC_SetDate(&hrtc, &rtc_cfg.defaultDate, rtc_cfg.timeformat);

        RTC_DATAVALID_VARIABLE = 1;            // validate rtc backup data
        RTC_BKPSRAM_DATAVALID_VARIABLE  = 0;   // invalidate bkpsram data
        RTC_BKPDIAG_DATAVALID_VARIABLE  = 0;   // invalidate bkpsram diag data
        RTC_NVMRAM_DATAVALID_VARIABLE   = 0;   // invalidate non-volatile data backups
    }

}

void RTC_setTime(RTC_Time_s * time) {
    HAL_RTC_SetTime(&hrtc, time, FORMAT_BIN);
}

void RTC_setDate(RTC_Date_s * date) {
    HAL_RTC_SetDate(&hrtc, date, FORMAT_BIN);
}

void RTC_getTime(RTC_Time_s * time) {
    HAL_RTC_GetTime(&hrtc, time, FORMAT_BIN);
}

void RTC_getDate(RTC_Date_s * date) {
    HAL_RTC_GetDate(&hrtc, date, FORMAT_BIN);
}
