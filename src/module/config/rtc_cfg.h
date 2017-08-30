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
 * @file    rtc_cfg.h
 * @author  foxBMS Team
 * @date    02.10.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  RTC
 *
 * @brief   Headers for the configuration for the real time clock
 *
 */

#ifndef RTC_CONF_H_
#define RTC_CONF_H_

/*================== Includes =============================================*/
#include "mcu_cfg.h"

/*================== Macros and Definitions ===============================*/
/* backup size of the registers 80 Byte */
#define RTC_DATAVALID_VARIABLE              (hrtc.Instance->BKP0R)  // rtc backup register
#define RTC_BKPSRAM_DATAVALID_VARIABLE      (hrtc.Instance->BKP1R)  // bkpsram data
#define RTC_BKPDIAG_DATAVALID_VARIABLE      (hrtc.Instance->BKP2R)  // bpksram diag data
#define RTC_NVMRAM_DATAVALID_VARIABLE       (hrtc.Instance->BKP3R)  // non-volatile data backups
#define RTC_DOWNLOAD_REQUEST_FLAG           (hrtc.Instance->BKP4R)  // flag for flashing new software


/**
 * struct for the initialization and configuration of RTC
 */
typedef struct {
    RCC_OscInitTypeDef       oscInitStruct;  /*!< clock configuration of RTCs clock source */
    RCC_PeriphCLKInitTypeDef clkInitStruct;  /*!< clock source of RTC:  RTCClk can be derived from HSE (ext. crystal), LSE (ext. crystal), LSI (internal RC oscillator) */
    RTC_InitTypeDef          initconfig;     /*!< RTC configuration: hour format, prescaler,... */
    uint8_t                  timeformat;     /*!< date and time format configuration (binary or BCD) */
    RTC_TimeTypeDef          defaultTime;    /*!< default time configuration */
    RTC_DateTypeDef          defaultDate;    /*!< default date configuration */
} RTC_CFG_s;


/*================== Constant and Variable Definitions ====================*/
extern RTC_CFG_s rtc_cfg;

/*================== Function Prototypes ===================================*/

/*================== Function Implementations ==============================*/

#endif /* RTC_CONF_H_ */
