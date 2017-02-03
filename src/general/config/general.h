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
 * @file    general.h
 * @author  foxBMS Team
 * @date    25.02.2015 (date of creation)
 * @ingroup GENERAL_CONF
 *
 * @brief Settings for the system (e.g., enable modules, set software version)
 *
 *
 */

#ifndef GENERAL_H_
#define GENERAL_H_


/*================== Includes =============================================*/
#include <stdint-gcc.h>
#include "std_types.h"
#include "global.h"
#include "database.h"

/*================== Macros and Definitions ===============================*/
#define BMS_BOARD_V_1_0                   0

#define EVAL_BOARD                        0
#define FOXBMS_BOARD                      1
/*fox
 * enables checking of flash checksum at startup.
 * @var enable flash checksum
 * @level advanced
 * @type select(2)
 * @default 0
 * @group GENERAL
 */
#define BUILD_MODULE_ENABLE_FLASHCHECKSUM           1
//#define BUILD_MODULE_ENABLE_FLASHCHECKSUM           0

/*fox
 * enables Mini-USB: USART3 peripheral (serial interface)
 * @var enable Mini-USB (USART3 peripheral)
 * @level advanced
 * @type select(2)
 * @default 0
 * @group GENERAL
 */
#define BUILD_MODULE_ENABLE_UART          1
//#define BUILD_MODULE_ENABLE_UART          0

/*fox
 * enables RS485: USART2 peripheral (serial interface)
 * @var enable RS485 (USART2 peripheral)
 * @level advanced
 * @type select(2)
 * @default 0
 * @group GENERAL
 */
#define BUILD_MODULE_ENABLE_RS485           1
//#define BUILD_MODULE_ENABLE_RS485          0

/*fox
 * enables COM module.
 * @var enable COM module
 * @level advanced
 * @type select(2)
 * @default 0
 * @group GENERAL
 */
#define BUILD_MODULE_ENABLE_COM           1
//#define BUILD_MODULE_ENABLE_COM           0

/*fox
 * enables printf debugging with serial interface
 * @var enable printf debugging
 * @level advanced
 * @type select(2)
 * @default 0
 * @group GENERAL
 */
#define BUILD_MODULE_DEBUGPRINTF          1
//#define BUILD_MODULE_DEBUGPRINTF          0

/*fox
 * enables RTC peripheral (Real Time Clock)
 * @var enable RTC peripheral
 * @level advanced
 * @type select(2)
 * @default 0
 * @group GENERAL
 */
#define BUILD_MODULE_ENABLE_RTC           1
//#define BUILD_MODULE_ENABLE_RTC           0


/*fox
 * enables MCU Watchdog
 * @var enable MCU Watchdog
 * @level devel
 * @type select(2)
 * @default 0
 * @group GENERAL
 */
#define BUILD_MODULE_ENABLE_WATCHDOG        1
//#define BUILD_MODULE_ENABLE_WATCHDOG      0


#define STR(TESTMACRO) #TESTMACRO        
#define XSTR(TESTMACRO) STR(TESTMACRO)   

#ifndef BUILD_VERSION
#define BUILD_VERSION        "    0.1"                /*strlen: 16 (15 + '/0') */
#endif

#ifndef BUILD_APPNAME
#define BUILD_APPNAME        " foxBMS"                /*strlen: 16 (15 + '/0') */
#endif

#ifndef BUILD_VENDOR
#define BUILD_VENDOR         "Fraunhofer IISB"        /*strlen: 16 (15 + '/0') */
#endif

//#pragma message XSTR(BUILD_VERSION)
//#pragma message XSTR(BUILD_APPNAME)
//#pragma message XSTR(BUILD_VENDOR)



/*================== Constant and Variable Definitions ====================*/


/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/







#endif /* GENERAL_H_ */
