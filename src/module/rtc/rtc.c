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
 * @file    rtc.c
 * @author  foxBMS Team
 * @date    20.10.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  RTC
 *
 * @brief Driver for the real time clock.
 *
 */



/*================== Includes =============================================*/
#include "rtc.h"

/*================== Macros and Definitions ===============================*/


/*================== Constant and Variable Definitions ====================*/
RTC_HandleTypeDef hrtc;

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/
void RTC_Init(void) {

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    /* Configure RTC clocks */
    RCC_OscInitTypeDef OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    OscInitStruct.LSEState = RCC_LSE_ON;

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

    HAL_RCC_OscConfig(&OscInitStruct);

    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Enable RTC clock */
    __HAL_RCC_RTC_ENABLE();

    /* Initialize RTC */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_ALARMA;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init(&hrtc);

    /* Disable WakeUp-Timer */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    // set time and date if not already initialized or V_BAT failed
    if (!(hrtc.Instance->ISR & (uint32_t)RTC_ISR_INITS)) {

        RTC_DATAVALID_VARIABLE = 0;        // invalidate rtc backup data
        sTime.Hours = 18;
        sTime.Minutes = 20;
        sTime.Seconds = 0;
        sTime.SubSeconds = 0;
        sTime.TimeFormat = RTC_HOURFORMAT12_PM;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;
        HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BIN);

        sDate.WeekDay = RTC_WEEKDAY_MONDAY;
        sDate.Month = RTC_MONTH_NOVEMBER;
        sDate.Date = 23;
        sDate.Year = 15;
        HAL_RTC_SetDate(&hrtc, &sDate, FORMAT_BIN);

        RTC_DATAVALID_VARIABLE = 1;        // validate rtc backup data

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
