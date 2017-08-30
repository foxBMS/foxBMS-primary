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
 * @file    syscontrol.c
 * @author  foxBMS Team
 * @date    21.09.2015 (date of creation)
 * @ingroup ENGINE
 * @prefix  SYSCTRL
 *
 * @brief   Syscontrol driver implementation
 */

//  // TODO Review: did not change braces according to our styleguide

/*================== Includes =============================================*/
#include "general.h"
#include "syscontrol.h"
#include "database.h"
#include "cmsis_os.h"
#include "diag.h"

/*================== Macros and Definitions ===============================*/
SYSCTRL_STATE_s sysctrl = {
           .state       = SYSCTRL_STATE_UNINITIALIZED,
           .statereq    = SYSCTRL_STATE_REQ_NOREQUEST,
           .statereqACK = TRUE,                         // TODO Review: see todo at -->typedef struct {...}SYSCTRL_STATE_s;<--
           .timer       = 0,
           .timeout     = 0,
           .timeoutidle = 0,
           .seqErr      = 0,
};

/*================== Function Prototypes ==================================*/
static STD_RETURN_TYPE_e SYSCTRL_CheckReEntrance(void);
static void SYSCTRL_SetState(SYSCTRL_STATEMACH_e state, uint16_t transition_timer);
static SYSCTRL_STATEMACH_REQ_e SYSCTRL_GetPendigRequest(void);
static STD_RETURN_TYPE_e SYSCTRL_CheckPendigRequest(void);
static void SYSCTRL_StateControl(void);
static Diag_ReturnType SYSCTRL_CheckPrecharge(void);

/*================== Function Implementations =============================*/

/*================== Public functions =====================================*/
void SYSCRTL_VCUPresent(SYSCTRL_VCUPRESENCE_e vcupresent)
{
    if (vcupresent)
    {
        sysctrl.timeoutidle = SYSCTRL_IDLE_TIMEOUT;
    }
    else
    {
        sysctrl.timeoutidle = 0;
    }
    sysctrl.vcupresent = vcupresent;
}


SYSCTRL_VCUPRESENCE_e SYSCTRL_GetVCUPresent(void)
{
    return sysctrl.vcupresent;
}

SYSCTRL_STATEMACH_REQ_e SYSCTRL_GetStateRequest(void)
{
    SYSCTRL_STATEMACH_REQ_e retval; //TODO Review: initialize variable with SYSCTRL_STATE_ERROR
    retval = sysctrl.statereq;
    return (retval);
}

STD_RETURN_TYPE_e SYSCTRL_SetStateRequest(SYSCTRL_STATEMACH_REQ_e statereq)
{
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    taskENTER_CRITICAL();

    if(sysctrl.statereq == SYSCTRL_STATE_REQ_NOREQUEST)
    {
        if( (sysctrl.state == SYSCTRL_STATE_SLEEP) ||
            (sysctrl.state == SYSCTRL_STATE_STANDBY) ||
            (sysctrl.state == SYSCTRL_STATE_PRECHARGE_ERROR) ||
            (sysctrl.state == SYSCTRL_STATE_NORMAL) ||
#if BS_SEPARATE_POWERLINES == 1
            (sysctrl.state == SYSCTRL_STATE_NORMAL_CHARGE) ||
#endif // BS_SEPARATE_POWERLINES == 1
            (sysctrl.state == SYSCTRL_STATE_ERROR) ||
            (sysctrl.state == SYSCTRL_STATE_UNINITIALIZED)
          )
        {
            sysctrl.statereq = statereq;
            sysctrl.statereqACK = TRUE;
            retVal = E_OK;
        }
        else
        {  /* illegal state request */
            sysctrl.statereqACK = FALSE;
            retVal = E_NOT_OK;
        }
    }

    taskEXIT_CRITICAL();
    return (retVal);
}

SYSCTRL_STATEMACH_e SYSCTRL_GetState(void)
{
    return (sysctrl.state);
}

void SYSCTRL_Trigger(SYSCTRL_TRIG_EVENT_e event)
{
    SYSCTRL_STATEMACH_REQ_e statereq; // TODO Review: uninitialized variable

    DIAG_SysMonNotify(DIAG_SYSMON_SYSCTRL_ID, 0);        // task is running, state = ok

    if (SYSCTRL_CheckReEntrance()==E_NOT_OK)
    {
        return;
    }

    ++sysctrl.counter;            // regular trigger call counter

    statereq = SYSCTRL_GetPendigRequest();    // get pending state request
    if(statereq != SYSCTRL_STATE_REQ_NOREQUEST)
    {// if request is pending check it
        SYSCTRL_CheckPendigRequest();            // check pending state request
        if(SYSCTRL_GetState()==SYSCTRL_STATE_ERROR)
        {
            sysctrl.timer = 0;  // reset timer for entering error state as fast as possible
        }
    }

    if(sysctrl.timer)
    {
        if(--sysctrl.timer)
        {
            sysctrl.triggerentry--;
            return;    // handle state machine only if timer has elapsed
        }
    }

    switch(event)
    {
        case (SYS_MODE_STARTUP_EVENT):
            // wait for startup diagnosis
            break;

        case (SYS_MODE_TRIGGERED_EVENT):
        case (SYS_MODE_CYCLIC_EVENT):
            // actual call of the BMS state machine
            // statereq=SYS_ControlGetStateRequest(); // TODO Review: delete
            SYSCTRL_StateControl();
            break;

        case (SYS_MODE_ABNORMAL_EVENT):
            break;

        default:
            break;
    }
    --sysctrl.triggerentry;
}

/*================== Static functions =====================================*/
/**
 * @brief       Reentry check of system state machine trigger function
 *
 * @return      retval (type: STD_RETURN_TYPE_e)
 */
static STD_RETURN_TYPE_e SYSCTRL_CheckReEntrance(void)
{
    STD_RETURN_TYPE_e retval = E_OK;
    taskENTER_CRITICAL();
    if(!sysctrl.triggerentry)
    {
        sysctrl.triggerentry++;
    }
    else
    {
        retval = E_NOT_OK;    // multiple calls of function
    }
    taskEXIT_CRITICAL();
    return (retval);
}

/**
 * @brief       sets the members state, substate and timer of the global sysctrl variable
 * @param       state (type: SYSCTRL_STATEMACH_e)
 * @param       transition_timer (type: uint16_t)
 * return void
 */
static void SYSCTRL_SetState(SYSCTRL_STATEMACH_e state, uint16_t transition_timer) {
    sysctrl.laststate = sysctrl.state;
    sysctrl.state = state;
    sysctrl.substate = 0;
    sysctrl.timer = transition_timer;
}

/**
 * @brief   returns the member statereq of global sysctrl variable
 * @return  sysctrl.statereq (type: SYSCTRL_STATEMACH_REQ_e)
 */
static SYSCTRL_STATEMACH_REQ_e SYSCTRL_GetPendigRequest(void) {
    return (sysctrl.statereq);
}

/**
 * @brief   The requested state is fetched from the global sysctrl the requested state is
 *          set by calling SYSCTRL_SetState if the transition is valid.
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
static STD_RETURN_TYPE_e SYSCTRL_CheckPendigRequest(void)
{
    STD_RETURN_TYPE_e retVal= E_OK;
    SYSCTRL_STATEMACH_REQ_e statereq;

    statereq = sysctrl.statereq;
    sysctrl.statereq = SYSCTRL_STATE_REQ_NOREQUEST;    // reset pending request after reception

    /*REQUEST HANDLING*/
    if(statereq == SYSCTRL_STATE_REQ_ERROR)
    {
        /*
         * Intermediate or Main Transition    (coming from any state)
         * Transition: any state  -> SYSYCTRL_STATE_ERROR (or any other error state)
         * */
        SYSCTRL_SetState((SYSCTRL_STATEMACH_e)statereq, 0);
    }
    else
    {
        if(statereq == SYSCTRL_STATE_REQ_STANDBY)
        {
            if((sysctrl.state == SYSCTRL_STATE_SLEEP))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_AWAKE, 0);
            }
            else if((sysctrl.state == SYSCTRL_STATE_NORMAL))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_STANDBY, 0);
            }
#if BS_SEPARATE_POWERLINES == 1
            else if((sysctrl.state == SYSCTRL_STATE_NORMAL_CHARGE))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_STANDBY, 0);
            }
#endif // BS_SEPARATE_POWERLINES == 1
            else if((sysctrl.state == SYSCTRL_STATE_PRECHARGE_ERROR))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_STANDBY, 0);
            }
            else if((sysctrl.state == SYSCTRL_STATE_UNINITIALIZED))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_INITIALIZING, 0);
            }
            else if((sysctrl.state == SYSCTRL_STATE_ERROR))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_AWAKE, 0);
            }
            else
            {
                sysctrl.seqErr++;
                retVal = E_NOT_OK;
            }
        }
        else if(statereq == SYSCTRL_STATE_REQ_NORMAL)
        {
            if((sysctrl.state == SYSCTRL_STATE_STANDBY))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_NORMAL, 0);
            }
            else
            {
            sysctrl.seqErr++;
                retVal=E_NOT_OK;
            }
        }
#if BS_SEPARATE_POWERLINES == 1
        else if (statereq == SYSCTRL_STATE_REQ_NORMAL_CHARGE) {

            if((sysctrl.state == SYSCTRL_STATE_STANDBY))
            {
                SYSCTRL_SetState(SYSCTRL_STATE_NORMAL_CHARGE, 0);
            }
            else
            {
                sysctrl.seqErr++;
                retVal=E_NOT_OK;
            }
        }
#endif // BS_SEPARATE_POWERLINES == 1
    }
    /* END OF REQUEST HANDLING*/
    return (retVal);
}

/**
 * @brief   The SYSCTRL_StateControl handles all state transitions of the syscontrol module.
 *
 * @warning After exiting the normal mode (SYSCTRL_STATE_NORMAL) and transitioning into another state,
 * the valid state transition to that state must first open the contactors.
 *
 * SYSCTRL_StateControl() reads the state-member of the global sysctrl-struct (type: SYSCTRL_STATE_s).
 * The states (type: SYSCTRL_STATEMACH_e) handled by the state machine are:
 *  - SYSCTRL_STATE_INITIALIZING handles the substates: **0**, **1** <br>
 *      **state machine behavior:**
 *      - If it is substate == 0:
 *        0 is the initial value of sysctrl.substate. A startup-check routine is performed
 *            and substate is set to sysctrl.substate = 1
 *      - If it is substate == 1:
 *          - and the power self check returns OK:
 *            systrl.state is set to awake (SYSCTRL_STATE_AWAKE)
 *          - and the power self check returns NOT_OK:
 *            systrl.state is set to error mode (SYSCTRL_STATE_ERROR)
 *          - and the power self check returns BUSY:
 *            systrl.state is set to error mode (SYSCTRL_STATE_ERROR)
 *  - SYSCTRL_STATE_AWAKE handles the substate: **0**, **1** <br>
 *      **state machine behavior:**
 *      - If it is substate == 0:
 *         - perform wake up check and substate is set to sysctrl.substate = 1
 *      - If it is substate == 1:
 *         - TODO
 *  - SYSCTRL_STATE_IDLE handles the substates: **0**, **1** <br>
 *      **state machine behavior:**
 *      - If it is substate == 0:
 *         - The Interlock and all contactors are switched off, , and it is set substate = 1.
 *      - If it is substate == 1:
 *         - TODO
 *  - SYSCTRL_STATE_STANDBY handles the substates: **0**, **1** <br>
 *      **state machine behavior:**
 *      - If it is substate == 0:
 *         - The interlock is still switched on, while all contactors are switched off, and it is set substate = 1
 *      - If it is substate == 1:
 *         - TODO
 *  - SYSCTRL_STATE_NORMAL handles the substates: **0**, **1**, **2**, **3**, **4** <br>
 *      **state machine behavior:**
 *      - If it is substate == 0:
 *         - main minus contactor is closed and it is set substate = 1.
 *      - If it is substate == 1:
 *         - plus precharge contactor is closed and it is set substate = 2.
 *      - If it is substate == 2:
 *         - if precharge check is successful (i.e. the precharge current is lower than the configured threshold),
 *           it is set substate = 3. The function has a timing limit for the maximum precharge time, where the current
 *           has to be lower than the configured current threshold. If the time is exceeded, the bms is set to error
 *           mode (SYSCTRL_STATE_PRECHARGE_ERROR).
 *      - If it is substate == 3:
 *         - The plus precharge contactor is closed
 *         - go to substate 4
 *      - If it is substate == 4:
 *         - Precharge check
 *         - If precharge check ok, go to substate 5
 *      - If it is substate == 5:
 *        - Open the precharge contactor
          - go to substate 6
 *      - If it is substate == 6:
 *        - check plus main contactor
 *        - if plus main contactor closed go to substate 7
 *      - If it is substate == 7:
 *         - Normal operation mode. Main contactors are closed. **After exiting the
 *           normal mode (SYSCTRL_STATE_NORMAL) and transitioning into another state,
 *           the valid state transition to that state must first open the contactors.**
 *  - SYSCTRL_STATE_ERROR handles any error state as follows <br>
 *      **state machine behavior:**
 *      - The Interlock is opened and therefore the hardware opens all contactors.
 *        Nevertheless are all contactors are going to be switched off. (For a list of the
 *        contactors see cont_contactors_cfg [] in contactor_cfg.c)
 *  - SYSCTRL_STATE_PRECHARGE_ERROR <br>
 *      **state machine behavior:**
 *      - All contactors are going to be opened (For a list of the contactors see
 *        cont_contactors_cfg [] in contactor_cfg.c)
 *
 * return void
 */
// TODO Review: no check if contactor are still in the needed state for the current sysctrl state
static void SYSCTRL_StateControl(void)
{
    STD_RETURN_TYPE_e sysctrlContInit_ok; // TODO Review: uninitialized variable
    switch(sysctrl.state) {
        /****************************SLEEP*******************************************/
        case SYSCTRL_STATE_SLEEP: {
            SYSCTRL_ALL_CONTACTORS_OFF();
            SYSCTRL_ECU_GOTOSLEEP();      // TODO currently a dummy macro
            break;
        }

        /*****************************INITIALIZING**************************************/
        case SYSCTRL_STATE_INITIALIZING: {

            switch(sysctrl.substate) {

                case SYSCTRL_STATE_INITIALIZING_SUBSTATE_BASIC_SELF_CHECK: {
                    /* substate 0: starting selfcheck, ongoing selfcheck */
                    sysctrlContInit_ok = CONT_Init();
                    if(sysctrlContInit_ok == E_OK) {
                        SYSCTRL_STARTPOWERONSELFCHECK(); /* start power on selfcheck, TODO currently a dummy macro */
                        sysctrl.timeout = SYSCTRL_SELFPOWERONCHECK_TIMEOUT;
                        sysctrl.substate = SYSCTRL_STATE_INITIALIZING_SUBSTATE_POWERON_SELF_CHECK;
                    }
                    else {
                        SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 0);
                    }
                    break;
                }

                case SYSCTRL_STATE_INITIALIZING_SUBSTATE_POWERON_SELF_CHECK: {
                    /* selfcheck running */
                    if(SYSCTRL_GETPOWERONSELFCHECK_STATE() == SYSCTRL_CHECK_OK) {
                        SYSCTRL_SetState(SYSCTRL_STATE_AWAKE, 1);
                    }
                    else if(SYSCTRL_GETPOWERONSELFCHECK_STATE() == SYSCTRL_CHECK_NOT_OK) {
                        SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                    }
                    else if(SYSCTRL_GETPOWERONSELFCHECK_STATE() == SYSCTRL_CHECK_BUSY) {
                        if(sysctrl.timeout == 0) {
                            // TODO Timeout-Error Entry?
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                        }
                        else {
                            --sysctrl.timeout;    // keep staying in substate = 0
                        }
                    }
                    break;
                }

                default: {
                    SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                    break;
                }
            }
            break;
        }

        /*****************************AWAKE******************************************/
        case SYSCTRL_STATE_AWAKE: {

            switch(sysctrl.substate) {
                case SYSCTRL_STATE_AWAKE_SUBSTATE_AWAKE_SELF_CHECK: {
                    // substate 0: starting selfcheck, ongoing selfcheck
                    SYSCTRL_STARTAWAKESELFCHECK();                        /* start selfcheck */
                    sysctrl.timeout = SYSCTRL_SELFAWAKECHECK_TIMEOUT;
                    sysctrl.substate = SYSCTRL_STATE_AWAKE_SUBSTATE_WAITING_SELF_CHECK_OK;
                    break;
                }

                case SYSCTRL_STATE_AWAKE_SUBSTATE_WAITING_SELF_CHECK_OK: {
                    /* selfcheck running */
                    if(SYSCTRL_GETSELFCHECK_STATE() == SYSCTRL_CHECK_OK) {
                        SYSCTRL_SetState(SYSCTRL_STATE_IDLE, 1);
                    }
                    else if(SYSCTRL_GETSELFCHECK_STATE() == SYSCTRL_CHECK_NOT_OK) {
                        SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                    }
                    else if(SYSCTRL_GETSELFCHECK_STATE() == SYSCTRL_CHECK_BUSY) {
                        if(sysctrl.timeout == 0) {
                            // todo Timeout-Error Entry?
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                        }
                        else
                            --sysctrl.timeout;    // keep staying in substate = 0
                    }
                    break;
                }

                default: {
                    SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                    break;
                }
            }
            break;
        }

        /*****************************IDLE******************************************/
        case SYSCTRL_STATE_IDLE: {

            switch (sysctrl.substate) {
                case SYSCTRL_STATE_IDLE_SUBSTATE_OPEN_INTERLOCK_AND_CONTACTORS:{
                    sysctrl.timeoutidle = SYSCTRL_IDLE_TIMEOUT;
                    SYSCTRL_SETINTERLOCK_OFF();
                    SYSCTRL_ALL_CONTACTORS_OFF();
                    sysctrl.substate = SYSCTRL_STATE_IDLE_SUBSTATE_CHECK_VCU;
                    break;
                }

                case SYSCTRL_STATE_IDLE_SUBSTATE_CHECK_VCU: {
                    // substate 1: ongoing check VCU presence (external device or software module)
                    if(SYSCTRL_VCUPRESENT_CHECK() == SYSCTRL_VCUPRESENCE_OK) {
                        SYSCTRL_SetState(SYSCTRL_STATE_STANDBY, 1);
                    }
                    else {
                        if(sysctrl.timeoutidle == 0) {
                            SYSCRTL_VCUPresent(0);
                            SYSCTRL_SetState(SYSCTRL_STATE_SLEEP, 0);    // TODO exit Sleep-Funktion....
                        }
                        else {
                            --sysctrl.timeoutidle;
                        }
                    }
                    break;
                }

                default: {
                    SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                    break;
                }
            }
            break;
        }

        /*****************************STANDBY*****************************************/
        case SYSCTRL_STATE_STANDBY: {

            switch(sysctrl.substate) {
                case SYSCTRL_STATE_STANDBY_SUBSTATE_OPEN_CONTACTORS: {

                    sysctrl.timeout = SYSCTRL_VCUPRESENT_TIMEOUT;
                    SYSCTRL_ALL_CONTACTORS_OFF();
                    SYSCTRL_SETINTERLOCK_ON();
                    sysctrl.substate = SYSCTRL_STATE_STANDBY_SUBSTATE_CHECK_VCU;
                    break;
                }

                case SYSCTRL_STATE_STANDBY_SUBSTATE_CHECK_VCU: {
                    // periodically actions
                    if(SYSCTRL_VCUPRESENT_CHECK() == SYSCTRL_VCUPRESENCE_OK) {
                        sysctrl.timeout = SYSCTRL_VCUPRESENT_TIMEOUT;    // refresh timeout timer
                    }
                    else {
                        if(sysctrl.timeout == 0) {
                            SYSCTRL_SetState(SYSCTRL_STATE_IDLE, 0);
                        }
                        else {
                            --sysctrl.timeout;
                        }
                    }
                    break;
                }

                default: {
                    SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                    break;
                }
            }
            break;
        }

        /******************NORMAL**************************************************/
        case SYSCTRL_STATE_NORMAL: {
#if BS_SEPARATE_POWERLINES == 1
            if (sysctrl.laststate == SYSCTRL_STATE_NORMAL_CHARGE) {
                SYSCTRL_SetState(SYSCTRL_STATE_IDLE, 0);
                break;
            }
#endif // BS_SEPARATE_POWERLINES == 1
            if ((CONT_GetInterlockFeedback()).feedback == CONT_SWITCH_OFF){
                SYSCTRL_SETINTERLOCK_OFF();
                SYSCTRL_ALL_CONTACTORS_OFF();
                SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
            }
            else {
                switch(sysctrl.substate) {
                    case SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_MINUS_MAIN_CONTACTOR_ON: {
                       SYSCTRL_CONT_MAINMINUS_ON();
                       sysctrl.timer = SYSCTRL_TIMER_MAINMINUS;
                       sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_MINUS_MAIN_CONTACTOR;//1;
                       sysctrl.timeout = 2; //double check contactor feedback
                       break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_MINUS_MAIN_CONTACTOR: {
                        // check that the negative power line is on
                        CONT_STATE_MEASUREMENT_s feedbackMinusMain = CONT_GetContactorFeedback(CONT_MINUS_MAIN);
                        if (feedbackMinusMain.feedback == CONT_SWITCH_ON){
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_ON;//2;
                        }
                        else if((sysctrl.timeout == 0) && (feedbackMinusMain.feedback != CONT_SWITCH_ON)){
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_ON: {
                        SYSCTRL_CONT_MAINPRECHARGE_ON();
                        sysctrl.timer = SYSCTRL_TIMER_MAINPRECHARGE;
                        sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PLUS_PRECHARGE_CONTACTOR;//3;
                        sysctrl.timeout = 2; //double check contactor feedback
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PLUS_PRECHARGE_CONTACTOR: {
                        // check that the precharge power line is on
                        CONT_STATE_MEASUREMENT_s feedbackPlusPrecharge = CONT_GetContactorFeedback(CONT_PLUS_PRECHARGE);
                        if (feedbackPlusPrecharge.feedback == CONT_SWITCH_ON){
                            sysctrl.timeout = SYSCTRL_PRECHARGE_TIMEOUT;
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PRECHARGE;//4;
                        }
                        else if((sysctrl.timeout == 0) && (feedbackPlusPrecharge.feedback != CONT_SWITCH_ON)){
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PRECHARGE: {
                        uint8_t precharge_check = SYSCTRL_CHECKPRECHARGE();
                        if(precharge_check == DIAG_OK) {
                            SYSCTRL_CONT_MAINPLUS_ON();
                            sysctrl.timer = 50;    // 500ms contactor plus and precharge closed overlap
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_OFF;//5;
                        }
                        else if (precharge_check == DIAG_BUSY) {
                            if(sysctrl.timeout == 0) {
                                SYSCTRL_SetState(SYSCTRL_STATE_PRECHARGE_ERROR,1);
                            }
                            else {
                                --sysctrl.timeout;
                            }
                        }
                        else {
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_OFF: {
                        SYSCTRL_CONT_MAINPRECHARGE_OFF();   // open PreCharge
                        sysctrl.timer = SYSCTRL_TIMER_MAINPLUS;        //
                        sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_POWERLINE;//6;
                        sysctrl.timeout = 2; //double check contactor feedback
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_POWERLINE: {
                        // check that the power line is on
                        CONT_STATE_MEASUREMENT_s feedbackPlusMain = CONT_GetContactorFeedback(CONT_PLUS_MAIN);
                        CONT_STATE_MEASUREMENT_s feedbackMinusMain = CONT_GetContactorFeedback(CONT_MINUS_MAIN);
                        if ((CONT_SWITCH_ON == feedbackPlusMain.feedback) &&
                            (CONT_SWITCH_ON == feedbackMinusMain.feedback)) {
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_LOOP;//7;
                        }
                        else if((0 == sysctrl.timeout) &&
                                (CONT_SWITCH_ON != feedbackPlusMain.feedback) &&
                                (CONT_SWITCH_ON != feedbackMinusMain.feedback)) {
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_LOOP: {
                        /*NORMAL MODE*/
                        /*if(sysctrl.substate == SYSCTRL_STATE_NORMAL_SUBSTATE_LOOP//7)*/
                        CONT_STATE_MEASUREMENT_s feedbackPlusMain = CONT_GetContactorFeedback(CONT_PLUS_MAIN);
                        CONT_STATE_MEASUREMENT_s feedbackMinusMain = CONT_GetContactorFeedback(CONT_MINUS_MAIN);
                        if ((CONT_SWITCH_ON == feedbackPlusMain.feedback) &&
                            (CONT_SWITCH_ON == feedbackMinusMain.feedback)) {
                            sysctrl.timeout = 100;
                        }
                        else if(sysctrl.timeout == 0) {
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        // bei exit zu anderen erst MAINPLUS OFF und MAINMINUS OFF
                        // TODO Review: no
                        break;
                    }

                    default: {
                        SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                        break;
                    }
                }
            }
            break;
        }

#if BS_SEPARATE_POWERLINES == 1
        /******************NORMAL CHARGE*******************************************/
        case SYSCTRL_STATE_NORMAL_CHARGE: {
            if (sysctrl.laststate == SYSCTRL_STATE_NORMAL) {
                SYSCTRL_SetState(SYSCTRL_STATE_IDLE, 0);
                break;
            }
            if ((CONT_GetInterlockFeedback()).feedback == CONT_SWITCH_OFF){
                SYSCTRL_SETINTERLOCK_OFF();
                SYSCTRL_ALL_CONTACTORS_OFF();
                SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
            }
            else {
                switch(sysctrl.substate) {
                    case SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_MINUS_MAIN_CONTACTOR_ON: {
                       SYSCTRL_CONT_CHARGE_MAINMINUS_ON();// SYSCTRL_CONT_MAINMINUS_ON();
                       sysctrl.timer = SYSCTRL_TIMER_MAINMINUS;
                       sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_MINUS_MAIN_CONTACTOR;//1;
                       sysctrl.timeout = 2; //double check contactor feedback
                       break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_MINUS_MAIN_CONTACTOR: {
                        // check that the negative power line is on
                        CONT_STATE_MEASUREMENT_s feedbackChargeMinusMain = CONT_GetContactorFeedback(CONT_CHARGE_MINUS_MAIN); //CONT_MINUS_MAIN
                        if (feedbackChargeMinusMain.feedback == CONT_SWITCH_ON){
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_ON;//2;
                        }
                        else if((sysctrl.timeout == 0) && (feedbackChargeMinusMain.feedback != CONT_SWITCH_ON)){
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_ON: {
                        SYSCTRL_CONT_CHARGE_MAINPRECHARGE_ON();//SYSCTRL_CONT_MAINPRECHARGE_ON();
                        sysctrl.timer = SYSCTRL_TIMER_MAINPRECHARGE;
                        sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PLUS_PRECHARGE_CONTACTOR;//3;
                        sysctrl.timeout = 2; //double check contactor feedback
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PLUS_PRECHARGE_CONTACTOR: {
                        // check that the precharge power line is on
                        CONT_STATE_MEASUREMENT_s feedbackChargePlusPrecharge = CONT_GetContactorFeedback(CONT_CHARGE_PLUS_PRECHARGE); //CONT_PLUS_PRECHARGE
                        if (feedbackChargePlusPrecharge.feedback == CONT_SWITCH_ON){
                            sysctrl.timeout = SYSCTRL_PRECHARGE_TIMEOUT;
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PRECHARGE;//4;
                        }
                        else if((sysctrl.timeout == 0) && (feedbackChargePlusPrecharge.feedback != CONT_SWITCH_ON)){
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_PRECHARGE: {
                        uint8_t precharge_check = SYSCTRL_CHECKPRECHARGE();
                        if(precharge_check == DIAG_OK) {
                            SYSCTRL_CONT_CHARGE_MAINPLUS_ON();//SYSCTRL_CONT_MAINPLUS_ON();
                            sysctrl.timer = 50;    // 500ms contactor plus and precharge closed overlap
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_OFF;//5;
                        }
                        else if (precharge_check == DIAG_BUSY) {
                            if(sysctrl.timeout == 0) {
                                SYSCTRL_SetState(SYSCTRL_STATE_PRECHARGE_ERROR,1);
                            }
                            else {
                                --sysctrl.timeout;
                            }
                        }
                        else {
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_SWITCH_PLUS_PRECHARGE_CONTACTOR_OFF: {
                        SYSCTRL_CONT_CHARGE_MAINPRECHARGE_OFF();//SYSCTRL_CONT_MAINPRECHARGE_OFF();   // open PreCharge
                        sysctrl.timer = SYSCTRL_TIMER_MAINPLUS;        //
                        sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_POWERLINE;//6;
                        sysctrl.timeout = 2; //double check contactor feedback
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_CHECK_POWERLINE: {
                        // check that the power line is on
                        CONT_STATE_MEASUREMENT_s feedbackChargePlusMain = CONT_GetContactorFeedback(CONT_CHARGE_PLUS_MAIN); //CONT_PLUS_MAIN
                        CONT_STATE_MEASUREMENT_s feedbackChargeMinusMain = CONT_GetContactorFeedback(CONT_CHARGE_MINUS_MAIN); //CONT_MINUS_MAIN
                        if ((CONT_SWITCH_ON == feedbackChargePlusMain.feedback) &&
                            (CONT_SWITCH_ON == feedbackChargeMinusMain.feedback)) {
                            sysctrl.substate = SYSCTRL_STATE_NORMAL_SUBSTATE_LOOP;//7;
                        }
                        else if((0 == sysctrl.timeout) &&
                                (CONT_SWITCH_ON != feedbackChargePlusMain.feedback) &&
                                (CONT_SWITCH_ON != feedbackChargeMinusMain.feedback)) {
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR,1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        break;
                    }

                    case SYSCTRL_STATE_NORMAL_SUBSTATE_LOOP: {
                        /*NORMAL MODE*/
                        /*if(sysctrl.substate == SYSCTRL_STATE_NORMAL_SUBSTATE_LOOP//7)*/
                        CONT_STATE_MEASUREMENT_s feedbackChargePlusMain = CONT_GetContactorFeedback(CONT_CHARGE_PLUS_MAIN);
                        CONT_STATE_MEASUREMENT_s feedbackChargeMinusMain = CONT_GetContactorFeedback(CONT_CHARGE_MINUS_MAIN);
                        if ((CONT_SWITCH_ON == feedbackChargePlusMain.feedback) &&
                            (CONT_SWITCH_ON == feedbackChargeMinusMain.feedback)) {
                            sysctrl.timeout = 100;
                        }
                        else if(sysctrl.timeout == 0) {
                            SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                        }
                        else {
                            sysctrl.timeout--;
                        }
                        // bei exit zu anderen erst MAINPLUS OFF und MAINMINUS OFF
                        // TODO Review: no
                        break;
                    }

                    default: {
                        SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
                        break;
                    }
                }
            }
            break;
        }
#endif // BS_SEPARATE_POWERLINES == 1
        /******************ERROR**************************************************/
        case SYSCTRL_STATE_ERROR: {
            SYSCTRL_SETINTERLOCK_OFF();
            SYSCTRL_ALL_CONTACTORS_OFF();
            break;
        }

        /******************PRECHARGEERROR******************************************/
        case SYSCTRL_STATE_PRECHARGE_ERROR: {
            SYSCTRL_ALL_CONTACTORS_OFF();
            break;
        }

        /******************DEFAULT*************************************************/
        default: {
            SYSCTRL_SetState(SYSCTRL_STATE_ERROR, 1);
            break;
        }

    }
    // CPU_RestoreINT(interrupt_status);
}


/**
 * @brief   Returns DIAG_OK if the precharge voltage is smaller than the precharge voltage threshold
 *          and the current is lower than the threshold current. If
 * @return  retVal (type: Diag_ReturnType)
 */
static Diag_ReturnType SYSCTRL_CheckPrecharge(void) {
    Diag_ReturnType retVal = DIAG_BUSY;
    DATA_BLOCK_CURRENT_s current_tab;   // TODO Review: uninitialized variable

    DATA_GetTable(&current_tab, DATA_BLOCK_ID_CURRENT);
    float sysctrl_precharge_threshold;  // TODO Review: uninitialized variable
    float current;                      // TODO Review: uninitialized variable

    if (current_tab.current>0) {
        current = current_tab.current;
    } else {
        current = -current_tab.current;
    }

    if (current_tab.voltage[1] > current_tab.voltage[2]) {
        sysctrl_precharge_threshold = current_tab.voltage[1]-current_tab.voltage[2];
    } else {
        sysctrl_precharge_threshold = current_tab.voltage[2]-current_tab.voltage[1];
    }

    if ((sysctrl_precharge_threshold < SYSCTRL_PRECHARGE_VOLTAGE_THRESHOLD) && (current < SYSCTRL_PRECHARGE_CURRENT_THRESHOLD)) {
        retVal = DIAG_OK;
    } else {
        retVal = DIAG_BUSY;
    }
    return retVal;
}
