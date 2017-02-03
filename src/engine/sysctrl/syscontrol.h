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
 * @file    syscontrol.h
 * @author  foxBMS Team
 * @date    21.09.2015 (date of creation)
 * @ingroup ENGINE
 * @prefix  SYSCTRL
 *
 * @brief   Syscontrol driver header
 *
 * ==============================================================================
 *                  Purpose of the SYSCONTROL-module
 * ==============================================================================
 * The syscontrol module handles the requested state transitions from the
 * bmsctrl-module. Depending on the current state and substate of the battery
 * management system the sysctrl performs the state transition or denies it.
 * The syscontrol module manages the following main states
 *  - uninitialized BMS (SYSCTRL_STATE_UNINITIALIZED)
 *  - permission to initialize the BMS (SYSCTRL_STATE_INITIALIZING)
 *  - sleeping mode (SYSCTRL_STATE_SLEEP)
 *  - idle mode (SYSCTRL_STATE_IDLE)
 *  - standby mode (SYSCTRL_STATE_STANDBY)
 *  - normal mode (SYSCTRL_STATE_NORMAL)
 *  - general error state (SYSCTRL_STATE_ERROR)
 *  - and an error while precharging (SYSCTRL_STATE_PRECHARGE_ERROR)
 *
 * and the numbered substates
 *  - 0
 *  - 1
 *  - 2
 *  - 3
 *  - 4
 *
 * The substates and their meaning is explained in the function SYSCTRL_StateControl().
 */


#ifndef SYS_CONTROL_H_
#define SYS_CONTROL_H_

/*================== Includes =============================================*/
#include "syscontrol_cfg.h"


/*================== Macros and Definitions ===============================*/

/**
 * Symbolic names for busyness of the syscontrol
 */
typedef enum {
    SYSCTRL_CHECK_OK        = 0,    /*!< syscontrol ok      */
    SYSCTRL_CHECK_BUSY      = 1,    /*!< syscontrol busy    */
    SYSCTRL_CHECK_NOT_OK    = 2,    /*!< syscontrol not ok  */
} SYSCTRL_CHECK_e;



typedef enum {
  SYS_MODE_STARTUP_EVENT    = 0,    /*!< syscontrol startup                 */
// SYS_MODE_EVENT_INIT      = 1,    /*!< todo                               */
  SYS_MODE_CYCLIC_EVENT     = 2,    /*!< for cyclic events                  */
  SYS_MODE_TRIGGERED_EVENT  = 3,    /*!< for triggered events               */
  SYS_MODE_ABNORMAL_EVENT   = 4,    /*!< for abnormal (error etc.) events   */
  SYS_MODE_EVENT_RESERVED   = 0xFF, /*!< do not use                         */
} SYSCTRL_TRIG_EVENT_e;

/**
 * Symbolic names of BMS states of statemachine
 */
// TODO Review: mixing of SYSCTRL_STATE and BmsState_STATE, what is what?
// TODO Review: why not use the mapping in SYSCTRL_STATEMACH_REQ_e and keep this way more clean, readable and understandable
typedef enum {
    SYSCTRL_STATE_UNINITIALIZED                             = 0,    /*!< BmsState_NO_STATE                                                      */
    SYSCTRL_STATE_INITIALIZING                              = 1,    /*!< BmsState_STATE_TOP                                                     */
    BmsState_STATE_NormalOperation                          = 2,    /*!< BmsState_STATE_NormalOperation                                         */  // TODO Review: enum names have to be in upper case
    BmsState_STATE_NormalOperation_State_Drive              = 3,    /*!< BmsState_STATE_NormalOperation_State_Drive                             */  //TODO Review: enum names have to be in upper case
    BmsState_STATE_NormalOperation_State_Charge             = 4,    /*!< BmsState_STATE_NormalOperation_State_Charge                            */  // TODO Review: enum names have to be in upper case
    BmsState_STATE_ManualOperation                          = 5,    /*!< BmsState_STATE_ManualOperation                                         */  // TODO Review: enum names have to be in upper case
    SYSCTRL_STATE_AWAKE                                     = 6,    /*!< BmsState_STATE_Awake                                                   */
    SYSCTRL_STATE_SLEEP                                     = 7,    /*!< BmsState_STATE_Sleep                                                   */
    SYSCTRL_STATE_IDLE                                      = 8,    /*!< BmsState_STATE_NormalOperation_State_ParkIdle                          */
    SYSCTRL_STATE_STANDBY                                   = 20,   /*!<                                                                        */
    SYSCTRL_STATE_NORMAL                                    = 9,    /*!< BmsState_STATE_NormalOperation_State_Drive_Precharge                   */
    SYSCTRL_STATE_NORMAL_OPERATION                          = 10,   /*!< BmsState_STATE_NormalOperation_State_Drive_StateDrive                  */
    BmsState_STATE_NormalOperation_State_Charge_Precharge   = 11,   /*!< BmsState_STATE_NormalOperation_State_Charge_State_Precharge_Charger    */ // TODO Review: enum names have to be in upper case
    BmsState_STATE_NormalOperation_State_Charge_Charge      = 12,   /*!< BmsState_STATE_NormalOperation_State_Charge_State_Charge               */  // TODO Review: enum names have to be in upper case
    BmsState_STATE_ManualOperation_State_ErrorChecking      = 13,   /*!< BmsState_STATE_ManualOperation_State_ErrorChecking                     */  // TODO Review: enum names have to be in upper case
    BmsState_STATE_ManualOperation_State_Diagnostics        = 14,   /*!< BmsState_STATE_ManualOperation_State_Diagnostics                       */  // TODO Review: enum names have to be in upper case
    BmsState_STATE_ManualOperation_State_Bootloader         = 15,   /*!< BmsState_STATE_ManualOperation_State_Bootloader                        */  // TODO Review: enum names have to be in upper case
    BmsState_STATE_AwakeState_Check                         = 16,   /*!< BmsState_STATE_AwakeState_Check                                        */ // TODO Review: enum names have to be in upper case
    SYSCTRL_STATE_ERROR                                     = 17,   /*!< SYSYCTRL_STATE_ERROR                                                   */
    SYSCTRL_STATE_PRECHARGE_ERROR                           = 18    /*!< SYSYCTRL_STATE_PRECHARGE_ERROR                                         */
    // TODO Review: state for re-initialization is missing
} SYSCTRL_STATEMACH_e;

/**
 * Symbolic name for default substate 0, which is initial state for every main state
 */
typedef enum {
    SYSCTRL_STATE_DEFAULT_SUBSTATE  = 0,    /*!< Default substate; at every state, the first state has to be default*/
} SYSCTRL_STATE_DEFAULT_SUBSTATE_e;

/**
 * Symbolic names for substates in main-state SYSCTRL_STATE_INITIALIZING
 */
typedef enum {
    SYSCTRL_STATE_INITIALIZING_SUBSTATE_BASIC_SELF_CHECK    = 0,    /*!< Substate for basic self checks     */
    SYSCTRL_STATE_INITIALIZING_SUBSTATE_POWERON_SELF_CHECK  = 1     /*!< Substate for going to awake state  */
} SYSCTRL_STATE_INITIALIZING_SUBSTATES_e;

/**
 * Symbolic names for substates in main-state AWAKE
 */
typedef enum {
    SYSCTRL_STATE_AWAKE_SUBSTATE_AWAKE_SELF_CHECK       = 0,    /*!< Substate for */
    SYSCTRL_STATE_AWAKE_SUBSTATE_WAITING_SELF_CHECK_OK  = 1     /*!< Substate for */
} SYSCTRL_STATE_AWAKE_SUBSTATES_e;

/**
 * Symbolic names for substates in main-state IDLE
 */
typedef enum {
    SYSCTRL_STATE_IDLE_SUBSTATE_OPEN_INTERLOCK_AND_CONTACTORS   = 0,    /*!< Substate for */
    SYSCTRL_STATE_IDLE_SUBSTATE_CHECK_VCU                       = 1     /*!< Substate for */
} SYSCTRL_STATE_IDLE_SUBSTATES_e;

/**
 * Symbolic names for substates in main-state STANDBY
 */
typedef enum {
    SYSCTRL_STATE_STANDBY_SUBSTATE_OPEN_CONTACTORS  = 0,    /*!< Substate for */
    SYSCTRL_STATE_STANDBY_SUBSTATE_CHECK_VCU        = 1     /*!< Substate for */
} SYSCTRL_STATE_STANDBY_SUBSTATES_e;

/**
 * Symbolic names for substates in main-state NORMAL
 */
typedef enum {
    SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_MINUS_MAIN_CONTACTOR_ON        = 0,    /*!< Substate for closing minus main contactor                                      */
    SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_MINUS_MAIN_CONTACTOR            = 1,    /*!< Substate for checking if minus main contactor is closed                        */
    SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_ON    = 2,    /*!< Substate for closing plus precharge contactor                                  */
    SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PLUS_PRECHARGE_CONTACTOR        = 3,    /*!< Substate for checking if plus precharge contactor is closed                    */
    SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PRECHARGE                       = 4,    /*!< Substate for checking if precharging is successful                             */
    SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_OFF   = 5,    /*!< Substate for opening plus precharge contactor, after successful precharge      */
    SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_POWERLINE                       = 6,    /*!< Substate for checking if the powerline is closed (main contactors are closed)  */
    SYSCTRL_STATE_NORMAL_SUBSTATE_LOOP                                  = 7     /*!< Substate for normal mode, therefore main contactors are closed                 */
} SYSCTRL_STATE_NORMAL_SUBSTATES_e;


/**
 * Symbolic names of BMS state requests of Statemachine
 */
// TODO Review: same as above
typedef enum {
 SYSCTRL_STATE_REQ_NOREQUEST = SYSCTRL_STATE_UNINITIALIZED ,
 BmsState_STATE_REQ_NormalOperation = BmsState_STATE_NormalOperation, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_NormalOperation_State_Drive = BmsState_STATE_NormalOperation_State_Drive, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_NormalOperation_State_Charge = BmsState_STATE_NormalOperation_State_Charge, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_ManualOperation = BmsState_STATE_ManualOperation, // TODO Review: enum names have to be in upper case
 SYSCTRL_STATE_REQ_AWAKE = SYSCTRL_STATE_AWAKE,
 // BmsState_STATE_REQ_GoForSleep = BmsState_STATE_GoForSleep,
 SYSCTRL_STATE_REQ_SLEEP = SYSCTRL_STATE_SLEEP,
 SYSCTRL_STATE_REQ_STANDBY = SYSCTRL_STATE_STANDBY,
 // SYSCTRL_STATE_REQ_NORMAL_PRECHARGE = SYSCTRL_STATE_NORMAL_PRECHARGE,
 SYSCTRL_STATE_REQ_NORMAL = SYSCTRL_STATE_NORMAL,
 BmsState_STATE_REQ_NormalOperation_State_Charge_Precharge = BmsState_STATE_NormalOperation_State_Charge_Precharge, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_NormalOperation_State_Charge_Charge = BmsState_STATE_NormalOperation_State_Charge_Charge, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_ManualOperation_State_ErrorChecking = BmsState_STATE_ManualOperation_State_ErrorChecking, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_ManualOperation_State_Diagnostics = BmsState_STATE_ManualOperation_State_Diagnostics, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_ManualOperation_State_Bootloader = BmsState_STATE_ManualOperation_State_Bootloader, // TODO Review: enum names have to be in upper case
 BmsState_STATE_REQ_AwakeState_Check = BmsState_STATE_AwakeState_Check, // TODO Review: enum names have to be in upper case
 SYSCTRL_STATE_REQ_ERROR = SYSCTRL_STATE_ERROR

} SYSCTRL_STATEMACH_REQ_e;


typedef struct {
    SYSCTRL_STATEMACH_e state;          /*!< actual state of control statemachine           */
    SYSCTRL_STATEMACH_e laststate;      /*!< last state of control statemachine             */  // TODO
    uint8_t substate;                   /*!< sub-state of state                             */
    uint8_t lastsubstate;               /*!< sub-state of state                             */
    SYSCTRL_STATEMACH_REQ_e statereq;   /*!< requested state (next state)                   */
    uint8_t statereqACK;                /*!< 1: Acknowledge, 0: Not Acknowledge             */  //TODO Review: later we use TRUE and FALSE, although its the same we shoudl be consistent
    uint32_t counter;                   /*!<                                                */
    uint16_t timer;                     /*!< e.g. in counts of 1ms                          */
    uint16_t timeout;                   /*!< e.g. in counts of 1ms                          */
    uint16_t timeoutidle;               /*!< e.g. in counts of 1ms                          */
    uint32_t seqErr;                    /*!< statemachine Sequence Error                    */
    uint8_t triggerentry;              /*!< re-entrance protection (function running flag) */
    uint8_t vcupresent;                /*!<                                                */
} SYSCTRL_STATE_s;

/**
 * this is the state used in the state machine
 */
extern SYSCTRL_STATE_s sysctrl;

/*================== Function Prototypes ==================================*/
/**
 * @brief   returns the member state of global sysctrl variable
 * @return  sysctrl.state (type: SYSCTRL_STATEMACH_e)
 */
extern SYSCTRL_STATEMACH_e SYSCTRL_GetState(void);

/**
 * @brief   returns the member statereq from the global sysctrl variable
 * @return  retval (type: SYSCTRL_STATEMACH_REQ_e)
 */
extern SYSCTRL_STATEMACH_REQ_e SYSCTRL_GetStateRequest(void);

/**
 * @brief   to be used by external callers for requesting a state
 *
 * If there is no pending request and the request is valid, the state request is set
 * accordingly to the input and the request is acknowledged . The function returns E_OK.<br>
 * If there is a invalid state request, the requested state is not acknowledged. The
 * function returns E_NOT_OK.
 *
 * @param   statereq (type: SYSCTRL_STATEMACH_REQ_e)
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e SYSCTRL_SetStateRequest(SYSCTRL_STATEMACH_REQ_e statereq);

/**
 * @brief   Calls SYSCTRL_StateControl() if the SYSCTRL_TRIG_EVENT_e event requested
 *          is SYS_MODE_TRIGGERED_EVENT or SYS_MODE_CYCLIC_EVENT
 * @param   event (type: SYSCTRL_TRIG_EVENT_e)
 *
 * return   void
 */
extern void SYSCTRL_Trigger(SYSCTRL_TRIG_EVENT_e event);

/**
 * @brief   sets the member vcupresent in global sysctrl variable depending on the input parameter vcupresent
 * @param   vcupresent (type: SYSCTRL_VCUPRESENCE_e)
 * return   void
 */
extern void SYSCRTL_VCUPresent(SYSCTRL_VCUPRESENCE_e vcupresent);

/**
 * @brief   returns the member vcupresent of global sysctrl
 * @return  sysctrl.vcupresent (type: SYSCTRL_VCUPRESENCE_e)
 */
extern SYSCTRL_VCUPRESENCE_e SYSCTRL_GetVCUPresent(void);

/*================== Function Implementations =============================*/

#endif // SYS_CONTROL_H_
