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
 * @file    syscontrol_cfg.h
 * @author  foxBMS Team
 * @date    21.09.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  SYSCTRL
 *
 * @brief   Syscontrol driver configuration header
 */

#ifndef SYS_CONTROL_CFG_H_
#define SYS_CONTROL_CFG_H_

/*================== Includes =============================================*/

#include "mcu.h"
#include "contactor.h"

/*================== Macros and Definitions ===============================*/
#define SYSCTRL_ECU_GOTOSLEEP()         // CPU_GoToSleep(60)/* EcuMode_GoToSleep();         //TODO Review: dummy macro with no effect
#define SYSCTRL_STARTAWAKESELFCHECK()   // Start SelfCheck in case of awaking from sleep    //TODO Review: dummy macro with no effect
#define SYSCTRL_STARTPOWERONSELFCHECK() // Start SelfCheck in case of starting by power on  //TODO Review: dummy macro with no effect

// #define SYSCTRL_RESETSLEEPFLAG() RTC_IF_ResetSleepFlag () //TODO Review: function RTC_IF_ResetSleepFlag not existing
#define SYSCTRL_VCUPRESENT_CHECK() 0//!(SYSCTRL_GetVCUPresent()) /* CanIf_isVcuPresent() */ TODO 

/*fox
 * @var     timeout of self power-on check
 * @type    int
 * @default 10
 * @valid   [5,15]
 * @unit    10*ms
 * @group   SYCONTROL
 * @level   advanced
 */
#define SYSCTRL_SELFPOWERONCHECK_TIMEOUT    10 // 100ms

/*fox
 * @var     timeout of self awake check
 * @type    int
 * @default 10
 * @valid   [5,15]
 * @unit    10*ms
 * @group   SYCONTROL
 * @level   advanced
 */
#define SYSCTRL_SELFAWAKECHECK_TIMEOUT      10 // 100ms

/*fox
 * @var     timeout of vcu present
 * @type    int
 * @default 30
 * @valid   [20,40]
 * @unit    10*ms
 * @group   SYCONTROL
 * @level   advanced
 */
#define SYSCTRL_VCUPRESENT_TIMEOUT          30 // 300ms

/*fox
 * @var     idle timeout
 * @type    int
 * @default 50
 * @valid   [40,60]
 * @unit    10*ms
 * @group   SYCONTROL
 * @level   advanced
 */
#define SYSCTRL_IDLE_TIMEOUT                500 // 5s timeout to go to sleep or power off in idle state

/*fox
 * @var     Precharge voltage threshold
 * @type    int
 * @default 10
 * @valid   [5,15]
 * @unit    V
 * @group   SYCONTROL
 * @level   user
 */
#define SYSCTRL_PRECHARGE_VOLTAGE_THRESHOLD 10 // V

/*fox
 * @var     Precharge current threshold
 * @type    int
 * @default 10
 * @valid   [5,15]
 * @unit    mA
 * @group   SYCONTROL
 * @level   user
 */
#define SYSCTRL_PRECHARGE_CURRENT_THRESHOLD 10 // mA

/*fox
 * @var     precharge timeout
 * @type    int
 * @default 600
 * @valid   [500,700]
 * @unit    10*ms
 * @group   SYCONTROL
 * @level   user
 */
#define SYSCTRL_PRECHARGE_TIMEOUT 600 // 6s

/*fox
 * @var     timer main minus
 * @type    int
 * @default 100
 * @valid   [50,150]
 * @unit    10*ms
 * @group   SYCONTROL
 * @level   user
 */
#define SYSCTRL_TIMER_MAINMINUS 100 // 1s

/*fox
 * @var     timer main precharge
 * @type    int
 * @default 200
 * @unit    10*ms
 * @valid   [150,250]
 * @group   SYCONTROL
 * @level   user
 */
#define SYSCTRL_TIMER_MAINPRECHARGE 200 // 2s

/*fox
 * @var     timer main plus
 * @type    int
 * @default 50
 * @valid   [40,60]
 * @unit    10*ms
 * @group   SYCONTROL
 * @level   user
 */
#define SYSCTRL_TIMER_MAINPLUS 50 // 500ms

#define SYSCTRL_GETSELFCHECK_STATE()            SYSCTRL_CHECK_OK            // function could return: SYSCTRL_CHECK_NOT_OK or OK SYSCTRL_CHECK_BUSY //TODO Review: dummy
#define SYSCTRL_GETPOWERONSELFCHECK_STATE()     SYSCTRL_CHECK_OK            // function could return: SYSCTRL_CHECK_NOT_OK or OK SYSCTRL_CHECK_BUSY //TODO Review: dummy
#define SYSCTRL_CHECKPRECHARGE()                SYSCTRL_CheckPrecharge()    // DIAG_CheckPrecharge()

/*
 * Mapping the marcos from the contactor-module to
 * syscontrol-macros.
 */
#define SYSCTRL_SETINTERLOCK_ON()           CONT_SwitchInterlockOn()
#define SYSCTRL_SETINTERLOCK_OFF()          CONT_SwitchInterlockOff()

#define SYSCTRL_ALL_CONTACTORS_OFF()        CONT_SwitchAllContactorsOff()

#define SYSCTRL_CONT_MAINMINUS_ON()         CONT_SetContactorState(CONT_MINUS_MAIN, CONT_SWITCH_ON)
#define SYSCTRL_CONT_MAINMINUS_OFF()        CONT_SetContactorState(CONT_MINUS_MAIN, CONT_SWITCH_OFF)

#define SYSCTRL_CONT_MAINPRECHARGE_ON()     CONT_SetContactorState(CONT_PLUS_PRECHARGE, CONT_SWITCH_ON)
#define SYSCTRL_CONT_MAINPRECHARGE_OFF()    CONT_SetContactorState(CONT_PLUS_PRECHARGE, CONT_SWITCH_OFF)

#define SYSCTRL_CONT_MAINPLUS_ON()          CONT_SetContactorState(CONT_PLUS_MAIN, CONT_SWITCH_ON)
#define SYSCTRL_CONT_MAINPLUS_OFF()         CONT_SetContactorState(CONT_PLUS_MAIN, CONT_SWITCH_OFF)

#if BS_SEPARATE_POWERLINES == 1
#define SYSCTRL_CONT_CHARGE_MAINMINUS_ON()         CONT_SetContactorState(CONT_CHARGE_MINUS_MAIN, CONT_SWITCH_ON)
#define SYSCTRL_CONT_CHARGE_MAINMINUS_OFF()        CONT_SetContactorState(CONT_CHARGE_MINUS_MAIN, CONT_SWITCH_OFF)

#define SYSCTRL_CONT_CHARGE_MAINPRECHARGE_ON()     CONT_SetContactorState(CONT_CHARGE_PLUS_PRECHARGE, CONT_SWITCH_ON)
#define SYSCTRL_CONT_CHARGE_MAINPRECHARGE_OFF()    CONT_SetContactorState(CONT_CHARGE_PLUS_PRECHARGE, CONT_SWITCH_OFF)

#define SYSCTRL_CONT_CHARGE_MAINPLUS_ON()          CONT_SetContactorState(CONT_CHARGE_PLUS_MAIN, CONT_SWITCH_ON)
#define SYSCTRL_CONT_CHARGE_MAINPLUS_OFF()         CONT_SetContactorState(CONT_CHARGE_PLUS_MAIN, CONT_SWITCH_OFF)
#endif // BS_SEPARATE_POWERLINES == 1
/**
 * Symbolic name for vcu presence
 */
typedef enum {
    SYSCTRL_VCUPRESENCE_OK      = 0,
    SYSCTRL_VCUPRESENCE_NOT_OK  = 1,
} SYSCTRL_VCUPRESENCE_e;

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

#endif /* SYS_CONTROL_CFG_H_ */
