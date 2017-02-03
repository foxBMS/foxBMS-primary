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
 * @file    bmsctrl.c
 * @author  foxBMS Team
 * @date    04.12.2015 (date of creation)
 * @ingroup ENGINE
 * @prefix  BMSCTRL
 *
 * @brief   Bmscontrol driver implementation
 *
 *
 */

/*================== Includes =============================================*/

#include "general.h"
#include "mcu.h"
#include "syscontrol.h"
#include "bmsctrl.h"
#include "sox.h"
#include "bal.h"
#include "diag.h"
#include "os.h"

/*================== Macros and Definitions ===============================*/

/**
 * save state and substate in members laststate and lastsubstate
 */
#define BMSCTRL_SAVELASTSTATES()    BMSCTRL_state.laststate     = BMSCTRL_state.state; \
                                    BMSCTRL_state.lastsubstate  = BMSCTRL_state.substate
/**
 * sets timer, state and substate to which BMSCTRL state machine should go
 */
#define BMSCTRL_MOVE_ON_TO_STATE(a, b, c)   BMSCTRL_state.timer     = a;\
                                            BMSCTRL_state.state     = b;\
                                            BMSCTRL_state.substate  = c

/*================== Constant and Variable Definitions ====================*/

/**
 * is indicator of running BMSCTRL module
 */
static uint8_t BMSCTRL_taskcycle = 0;

/**
 * is central control element of BMSCTRL state machine
 */
static BMSCTRL_STATE_s BMSCTRL_state = {
    .timer              = 0,
    .statereq           = BMSCTRL_STATE_NO_REQUEST,
    .state              = BMSCTRL_STATEMACH_UNINITIALIZED,
    .substate           = 0,
    .laststate          = BMSCTRL_STATEMACH_UNINITIALIZED,
    .lastsubstate       = 0,
    .triggerentry       = 0,
    .errMeasCnt         = BMSCTRL_MAX_MEAS_FAIL_WAIT,
    .errCANCnt          = BMSCTRL_MAX_CAN_FAIL_WAIT,
    .errRequestCnt      = 0,
    .timer_after_open   = 0,
};

/**
 * is module local copy of SOF values
 */
static DATA_BLOCK_SOX_s bmsctrl_sof_tab;
static DATA_BLOCK_CANERRORSIG_s canerr_tab;

/*================== Function Prototypes ==================================*/

static BMSCTRL_STATEMACH_e BMSCTRL_GetState(void);
static BMSCTRL_STATE_REQUEST_e BMSCTRL_TransferStateRequest(void);
static BMSCTRL_STATE_REQUEST_e BMSCTRL_GetStateRequest(void);
static uint8_t BMSCTRL_CheckReEntrance(void);
static uint8_t BMSCTRL_CheckNewCanReq(void);
static uint8_t BMSCTRL_CheckCanTiming(void);
static uint8_t BMSCTRL_CheckMeasDiag(void);
static void BMSCTRL_GetContactorFeedback(void);

/*================== Function Implementations =============================*/

/*================== Public functions =====================================*/

void BMSCTRL_Trigger(void) {
    BMSCTRL_STATE_REQUEST_e statereq = BMSCTRL_STATE_NO_REQUEST;
    DATA_BLOCK_STATEREQUEST_s canstatereq_tab;

    if (BMSCTRL_CheckReEntrance()) return;

    DIAG_SysMonNotify(DIAG_SYSMON_BMSCTRL_ID, 0);        // task is running, state = ok

    if(BMSCTRL_state.timer) {
        if(--BMSCTRL_state.timer) {
            BMSCTRL_state.triggerentry--;
            return;    // handle state machine only if timer has elapsed
        }
    }
    switch(BMSCTRL_state.state) {
        case BMSCTRL_STATEMACH_UNINITIALIZED:
            BMSCTRL_SAVELASTSTATES();
            statereq = BMSCTRL_TransferStateRequest();
            if (statereq == BMSCTRL_STATE_IDLE_REQUEST){
                BMSCTRL_MOVE_ON_TO_STATE(0, BMSCTRL_STATEMACH_INITIALIZATION, BMSCTRL_INIT_ENTRY);
            }
            break;
        case BMSCTRL_STATEMACH_INITIALIZATION:
            BMSCTRL_SAVELASTSTATES();
            // do things here you always want to do when initializing
            if (BMSCTRL_state.substate == BMSCTRL_INIT_ENTRY){
                // If needed, initializations or checks can be done here
                // Currently the state machine simply goes on
                if (TRUE) {
                    BMSCTRL_MOVE_ON_TO_STATE(0, BMSCTRL_STATEMACH_INITIALIZATION, BMSCTRL_INIT_STEP1);
                }
                else{
                    BMSCTRL_MOVE_ON_TO_STATE(0, BMSCTRL_STATEMACH_INITIALIZATION, BMSCTRL_INIT_ENTRY);
                }
            }
            else if (BMSCTRL_state.substate == BMSCTRL_INIT_STEP1){
                // do some other initialization things for example waiting
                //BMSCTRL_check something //always go on
                if (TRUE) { //OK
                    BMSCTRL_MOVE_ON_TO_STATE(3000, BMSCTRL_STATEMACH_INITIALIZATION, BMSCTRL_INIT_STEP2);
                }
                else{ //NOK
                    BMSCTRL_MOVE_ON_TO_STATE(3000, BMSCTRL_STATEMACH_INITIALIZATION, BMSCTRL_INIT_STEP1);
                }
            }
            else if (BMSCTRL_state.substate == BMSCTRL_INIT_STEP2){
                // do some final settings here for initialization
                BMSCTRL_MOVE_ON_TO_STATE(0, BMSCTRL_STATEMACH_INITIALIZED, 0);
            }
            else {
                BMSCTRL_state.errRequestCnt++;
            }
            break;
        case BMSCTRL_STATEMACH_INITIALIZED:
            BMSCTRL_SAVELASTSTATES();
            BMSCTRL_MOVE_ON_TO_STATE(0, BMSCTRL_STATEMACH_IDLE, 0);
            break;
        case BMSCTRL_STATEMACH_IDLE:
            BMSCTRL_SAVELASTSTATES();
            BMSCTRL_MOVE_ON_TO_STATE(0, BMSCTRL_STATEMACH_SOC_INIT_CHECK_MEAS_DIAG, 0);
            break;
        case BMSCTRL_STATEMACH_SOC_INIT_CHECK_MEAS_DIAG:
            BMSCTRL_SAVELASTSTATES();
            if (BMSCTRL_CheckMeasDiag()) {
                BMSCTRL_state.errMeasCnt = 0;
                SOC_Init();
                BAL_Init();
            }
            else {
                SOC_Init();
            }
            BMSCTRL_MOVE_ON_TO_STATE(0, BMSCTRL_STATEMACH_CHECK_MEAS_DIAG, 0);
            break;
        case BMSCTRL_STATEMACH_CHECK_MEAS_DIAG:
            BMSCTRL_SAVELASTSTATES();
            if (BMSCTRL_CheckMeasDiag()) {
                if (BMSCTRL_state.errMeasCnt > 0) {
                    BMSCTRL_state.errMeasCnt--;
                }
            }
            else {
                if (BMSCTRL_state.errMeasCnt <= BMSCTRL_MAX_MEAS_FAIL_WAIT) {
                    BMSCTRL_state.errMeasCnt++;
                }
            }
            // to prevent opening contactors from single measurement error the following comparison is done
            if (BMSCTRL_state.errMeasCnt >= BMSCTRL_MAX_MEAS_FAIL_WAIT) {
                BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_BAD_MEAS_DIAG, 0);
            }
            else {
                BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_CHECK_CANTIMING, 0);
            }
            break;
        case BMSCTRL_STATEMACH_CHECK_NEWCANREQ:
            BMSCTRL_SAVELASTSTATES();
            if (BMSCTRL_CheckNewCanReq()) {
                BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_EVERYTHINGOK, 0);
            }
            else {
                BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_CHECK_MEAS_DIAG, 0);
            }
            break;
        case BMSCTRL_STATEMACH_CHECK_CANTIMING:
            BMSCTRL_SAVELASTSTATES();
            if (BMSCTRL_CheckCanTiming()) {
                if (BMSCTRL_state.errCANCnt > 0) {
                    BMSCTRL_state.errCANCnt--;
                } else {
                    DATA_GetTable(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
                    canerr_tab.error_cantiming = 0;
                    DATA_StoreDataBlock(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
                }
            } else {
                if (BMSCTRL_state.errCANCnt <= BMSCTRL_MAX_CAN_FAIL_WAIT) {
                    BMSCTRL_state.errCANCnt++;
                    DATA_GetTable(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
                    canerr_tab.error_cantiming = 1;
                    DATA_StoreDataBlock(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
                } else {
                    DATA_GetTable(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
                    canerr_tab.error_cantiming = 2;
                    DATA_StoreDataBlock(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
                }
            }
            if (BMSCTRL_state.errCANCnt >= BMSCTRL_MAX_CAN_FAIL_WAIT) {
                BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_BAD_CANTIMING, 0);

            }
            else if (BMSCTRL_state.errCANCnt <= 2){ //less than 2 false CAN timings in a row seems ok
                BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_CHECK_NEWCANREQ, 0);
            }
            else{
                BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_CHECK_MEAS_DIAG, 0);
            }
            break;
        case BMSCTRL_STATEMACH_BAD_MEAS_DIAG:
            BMSCTRL_SAVELASTSTATES();
            SYSCTRL_SetStateRequest(SYSCTRL_STATE_REQ_ERROR);       // FIXME check return value whether request is accepted and stay here and retry if not
            BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_CHECK_MEAS_DIAG, 0);
            break;
        case BMSCTRL_STATEMACH_BAD_CANTIMING:
            BMSCTRL_SAVELASTSTATES();
            SYSCTRL_SetStateRequest(SYSCTRL_STATE_REQ_ERROR);       // FIXME check return value whether request is accepted and stay here and retry if not
            BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_CHECK_MEAS_DIAG, 0);
            break;
        case BMSCTRL_STATEMACH_EVERYTHINGOK:
            DATA_GetTable(&canstatereq_tab, DATA_BLOCK_ID_STATEREQUEST);
            switch(canstatereq_tab.state_request_pending) {
                case BMSCTRL_REQ_ID_STANDBY:
                    SYSCTRL_SetStateRequest(SYSCTRL_STATE_REQ_STANDBY);
                    BMSCTRL_state.errMeasCnt = 0;
                    BMSCTRL_MOVE_ON_TO_STATE(3500, BMSCTRL_STATEMACH_CHECK_MEAS_DIAG, 0);
                    canstatereq_tab.state_request_pending = 0;
                    break;
                case BMSCTRL_REQ_ID_NORMAL:
                    SYSCTRL_SetStateRequest(SYSCTRL_STATE_REQ_NORMAL);
                    BMSCTRL_state.errMeasCnt = 0;
                    BMSCTRL_MOVE_ON_TO_STATE(10, BMSCTRL_STATEMACH_CHECK_MEAS_DIAG, 0);
                    canstatereq_tab.state_request_pending = 0;

                    break;
                default:
                    // do nothing
                    canstatereq_tab.state_request_pending = 0;
                    break;
            }
            DATA_StoreDataBlock(&canstatereq_tab, DATA_BLOCK_ID_STATEREQUEST);
            break;
        default:
            BMSCTRL_state.errRequestCnt++;
            break;
    }
    BMSCTRL_state.triggerentry--;   // reentrance counter;
}

BMSCTRL_RETURN_TYPE_e BMSCTRL_Ctrl(void) {
    BMSCTRL_STATEMACH_e BMSCTRLstate;
    BMSCTRL_RETURN_TYPE_e retVal = BMSCTRL_NOTHING_TO_DO;

    if(BMSCTRL_GetStateRequest() == BMSCTRL_STATE_NO_REQUEST) {
        BMSCTRLstate = BMSCTRL_GetState();

        if(BMSCTRLstate == BMSCTRL_STATEMACH_UNINITIALIZED) {
            /* set initialization request*/
            retVal = BMSCTRL_SetStateRequest(BMSCTRL_STATE_INIT_REQUEST);
            BMSCTRL_taskcycle = 0;
        }
        else if(BMSCTRLstate == BMSCTRL_STATEMACH_IDLE) {
            /* set state requests for starting measurement cycles if BMSCTRL is in IDLE state*/
            ++BMSCTRL_taskcycle;
            retVal = BMSCTRL_OK;

        }
        else {
            retVal = BMSCTRL_NOTHING_TO_DO;
        }
    }
    else {
        retVal = BMSCTRL_BUSY_OK;
    }

    BMSCTRL_GetContactorFeedback();
    return retVal;
}

BMSCTRL_RETURN_TYPE_e BMSCTRL_SetStateRequest(BMSCTRL_STATE_REQUEST_e statereq) {
    BMSCTRL_state.statereq = statereq;
    return BMSCTRL_OK;
}

/*================== Static functions =====================================*/
/**
 * @brief   getter of state of BMSCTRL state machine
 *
 * @return  BMSCTRL_state.state     current state of the BMS control
 */
static BMSCTRL_STATEMACH_e BMSCTRL_GetState(void) {
    return (BMSCTRL_state.state);
}


/**
 * @brief   Transfers the state request resets the state request back
 *          to no request
 *
 * @return  retval   the state in which is transfered
 */
static BMSCTRL_STATE_REQUEST_e BMSCTRL_TransferStateRequest(void) {
    BMSCTRL_STATE_REQUEST_e retval;

    taskENTER_CRITICAL();
    retval = BMSCTRL_state.statereq;
    BMSCTRL_state.statereq = BMSCTRL_STATE_NO_REQUEST;
    taskEXIT_CRITICAL();

    return (retval);
}


/**
 * @brief   getter of the pending state request
 *
 * @return  retval  the pending state request
 */
static BMSCTRL_STATE_REQUEST_e BMSCTRL_GetStateRequest(void) {
    BMSCTRL_STATE_REQUEST_e retval;

    taskENTER_CRITICAL();
    retval = BMSCTRL_state.statereq;
    taskEXIT_CRITICAL();

    return (retval);
}

/**
 * @brief   Checks the re-entrance of bms state control
 *
 * @return  retval  returns value unequal 0 if multiple calls of the function occurred
 */
static uint8_t BMSCTRL_CheckReEntrance(void) {
    uint8_t retval = 0;

    taskENTER_CRITICAL();
    if(!BMSCTRL_state.triggerentry) {
        BMSCTRL_state.triggerentry++;
    }
    else {
        retval = 0xFF;    // multiple calls of function
    }
    taskEXIT_CRITICAL();

    return (retval);
}


/**
 * @brief   Checks if the CAN messages come in the specified time window
 *
 * if actual time stamp- previous time stamp is > 96 and < 104 check is good
 * else the check is bad
 *
 * @return  TRUE if timing is in tolerance range, FLASE if not
 */
static uint8_t BMSCTRL_CheckCanTiming(void) {
    uint8_t retVal = FALSE;
    DATA_BLOCK_STATEREQUEST_s canstatereq_tab;
    uint32_t current_time;

    current_time = MCU_GetTimeStamp();
    DATA_GetTable(&canstatereq_tab, DATA_BLOCK_ID_STATEREQUEST);

    //Is the BMS still getting CAN messages?
    if ((current_time-canstatereq_tab.timestamp) <= 105){
        if (((canstatereq_tab.timestamp - canstatereq_tab.previous_timestamp) >= 95) && \
                ((canstatereq_tab.timestamp - canstatereq_tab.previous_timestamp) <= 105)){
            retVal = TRUE;
        }
        else {
            retVal = FALSE;
        }
    }
    else{
        retVal = FALSE;
    }
#if BMSCTRL_CAN_TIMING_TEST == TRUE

    return retVal;
#else
    retVal = TRUE;
    return retVal;
#endif
}

/**
 * @brief   check if new request is pending via CAN and no precharge error has been detected
 *
 * @return  TRUE if new request is pending, FALSE if not
 */
static uint8_t BMSCTRL_CheckNewCanReq(void) {
    DATA_BLOCK_STATEREQUEST_s canstatereq_tab;
    SYSCTRL_STATEMACH_e sysctrlstate;
    sysctrlstate = SYSCTRL_GetState();
    DATA_GetTable(&canstatereq_tab, DATA_BLOCK_ID_STATEREQUEST);
    if (canstatereq_tab.state_request_pending != 0) {
        if (sysctrlstate == SYSCTRL_STATE_PRECHARGE_ERROR) {
            if (canstatereq_tab.state_request == 8) { // FIXME what is this 8? Better to use a define
                return TRUE;
            }
            else
                return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else
        return FALSE;
}

/**
 * @brief   checks the abidance by the safe operating area
 *
 * verify for cell measurements (U, I, T), if minimum and maximum values are out of range,
 * that currents are below specified values or current deratings
 *
 * @return  TRUE if within safe operating area
 */
static uint8_t BMSCTRL_CheckMeasDiag(void) {
    uint8_t retVal = FALSE;
    static DATA_BLOCK_MINMAX_s minmax;
    static DATA_BLOCK_CURRENT_s curr_tab;
    static DATA_BLOCK_CANERRORSIG_s error_tab;

    DATA_GetTable(&minmax, DATA_BLOCK_ID_MINMAX);
    DATA_GetTable(&curr_tab, DATA_BLOCK_ID_CURRENT);
    DATA_GetTable(&bmsctrl_sof_tab, DATA_BLOCK_ID_SOX);
    DATA_GetTable(&error_tab, DATA_BLOCK_ID_CANERRORSIG);

    retVal = TRUE;

    error_tab.error_lowtemp = 0;
    error_tab.error_hightemp = 0;
    error_tab.error_highvolt = 0;
    error_tab.error_lowvolt = 0;
    error_tab.error_overcurrent_charge = 0;
    error_tab.error_overcurrent_discharge = 0;

#if BMSCTRL_TEST_CELL_SOF_LIMITS == TRUE
    if (((curr_tab.current < (-1000*(bmsctrl_sof_tab.sof_continuous_charge))) ||
            (curr_tab.current > (1000*bmsctrl_sof_tab.sof_continuous_discharge)))) {
        retVal = FALSE;
    }
#endif

    if (minmax.temperature_max > BMSCTRL_TEMPMAX){
        error_tab.error_hightemp = 1;
        retVal = FALSE;
    }
    if (minmax.temperature_min < BMSCTRL_TEMPMIN) {
        error_tab.error_lowtemp = 1;
        retVal = FALSE;
    }
    if (minmax.voltage_max > BMSCTRL_VOLTMAX) {
        error_tab.error_highvolt = 1;
        retVal = FALSE;
    }
    if (minmax.voltage_min < BMSCTRL_VOLTMIN) {
        error_tab.error_lowvolt = 1;
        retVal = FALSE;
    }
    if (curr_tab.current > BMSCTRL_CURRENTMAX) {
        error_tab.error_overcurrent_charge = 1;
        retVal = FALSE;
    }
    if (curr_tab.current < BMSCTRL_CURRENTMIN) {
        error_tab.error_overcurrent_discharge = 1;
        retVal = FALSE;
    }

    DATA_StoreDataBlock(&error_tab, DATA_BLOCK_ID_CANERRORSIG);

#if BMSCTRL_TEST_CELL_LIMITS == TRUE
    return retVal;
#else
    retVal = TRUE;
    return retVal;
#endif
}


/*
 * @brief   check if the contactors are what their feedback line says and write error entry in database
 *
 * @return void
 */
static void BMSCTRL_GetContactorFeedback(void) {
    uint8_t contactor_state = 0;
    CONT_STATE_MEASUREMENT_s Plus_Main, Plus_Main_Precharge, Minus_Main;
    (void) CONT_AcquireContactorAndInterlockFeedbacks();
    /*
     * after calling CONT_AcquireContactorAndInterlockFeedbacks()
     * you can use
     *      Plus_Main = use cont_contactor_states[contactor].measurement.feedback
     * to get the latest measurement
     */
    Plus_Main = CONT_GetContactorFeedback(CONT_PLUS_MAIN);
    Plus_Main_Precharge = CONT_GetContactorFeedback(CONT_PLUS_PRECHARGE);
    Minus_Main = CONT_GetContactorFeedback(CONT_MINUS_MAIN);

    DATA_GetTable(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);

    if ((CONT_GetInterlockFeedback()).feedback == CONT_SWITCH_OFF){
        canerr_tab.interlock_open = 1;
    } else {
        canerr_tab.interlock_open = 0;
    }

    if ( (Plus_Main.feedback != CONT_SWITCH_UNDEF) &&
         (Plus_Main_Precharge.feedback != CONT_SWITCH_UNDEF) &&
         (Minus_Main.feedback != CONT_SWITCH_UNDEF) ) {

        contactor_state = (Plus_Main.feedback << 2) |
                          (Plus_Main_Precharge.feedback << 0) |
                          (Minus_Main.feedback << 3);

        canerr_tab.contactor_state_feedback = contactor_state;
        canerr_tab.contactor_error = 0;
        DATA_StoreDataBlock(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);

    } else {
        canerr_tab.contactor_error = 1;
        DATA_StoreDataBlock(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
    }
}
