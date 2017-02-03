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
 * @file    contactor.c
 * @author  foxBMS Team
 * @date    23.09.2015
 * @ingroup DRIVERS
 * @prefix  CONT
 *
 * @brief   Driver for the contactors
 *
 * This module is designed as a wrapper of the io-module, to easily handle
 * contactors. It allows an easy switching off the contactor state and
 * measuring the feedback and storing these data.
 * In case of opening the contactors under load an error entry is performed.
 *
 */

/*================== Includes =============================================*/
#include "contactor.h"
#include "io.h"
#include "diag.h"
#include "cmsis_os.h"
#include "timing.h"
/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/
/**
 * used to locally copy the current-sensor value from the global database
 * current table
 */
static DATA_BLOCK_CURRENT_s cont_current_tab = {0};

/*================== Function Prototypes ==================================*/
static STD_RETURN_TYPE_e CONT_CheckForAllContactorsOpen(void);
static STD_RETURN_TYPE_e CONT_CheckTimeViolation(CONT_NAMES_e contactor);

/*================== Function Implementations =============================*/

/*================== Public functions =====================================*/

STD_RETURN_TYPE_e CONT_Init(void) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    if((NR_OF_CONTACTORS != cont_contactors_cfg_length) ||
       (NR_OF_CONTACTORS != cont_contactors_states_length)) {
        retVal = E_NOT_OK;  /* configuration error*/
    }
    else {
        uint8_t allContactorsOpen = FALSE;
        allContactorsOpen = CONT_CheckForAllContactorsOpen();
        if (TRUE == allContactorsOpen) {
            retVal = E_OK;
        }
        else {
            CONT_SwitchAllContactorsOff();
            allContactorsOpen = FALSE;
            allContactorsOpen = CONT_CheckForAllContactorsOpen();
            if (TRUE == allContactorsOpen) {
                retVal = E_OK;
            }
            else {
                retVal = E_NOT_OK;
            }
        }
    }
#ifdef CONT_IGNORE_CONT_INIT_CHECK
// https://gcc.gnu.org/onlinedocs/gcc-5.1.0/gcc/Diagnostic-Pragmas.html
#define DO_PRAGMA(x) _Pragma (#x)
#define WARNING(x) DO_PRAGMA(message ("Warning:" #x))
WARNING(The contactor states is not checked on startup!)
    retVal = E_OK;
#endif
    return retVal;
}

CONT_STATE_SET_s CONT_GetContactorSetValue(CONT_NAMES_e contactor) {
    CONT_STATE_SET_s contactorSetInformation = {FALSE, 0};
    taskENTER_CRITICAL();
    contactorSetInformation.setvalue = cont_contactor_states[contactor].set.setvalue;
    contactorSetInformation.timestamp = cont_contactor_states[contactor].set.timestamp;
    taskEXIT_CRITICAL();
    return contactorSetInformation;
}

CONT_STATE_SET_s CONT_GetInterlockSetValue() {
    CONT_STATE_SET_s interlockSetInformation = {FALSE, 0};
    taskENTER_CRITICAL();
    interlockSetInformation.setvalue = cont_interlock_state.set.setvalue;
    interlockSetInformation.timestamp = cont_interlock_state.set.timestamp;
    taskEXIT_CRITICAL();
    return interlockSetInformation;
}

CONT_STATE_EXPECTATION_s CONT_GetContactorExpectedValue(CONT_NAMES_e contactor) {
    CONT_STATE_EXPECTATION_s expectedContactorValue = {CONT_SWITCH_UNDEF, 0};
    taskENTER_CRITICAL();
    expectedContactorValue.feedback = cont_contactor_states[contactor].expectation.feedback;
    expectedContactorValue.timestamp = cont_contactor_states[contactor].expectation.timestamp;
    taskEXIT_CRITICAL();
    return expectedContactorValue;
}

CONT_STATE_EXPECTATION_s CONT_GetInterlockExpectedValue() {
    CONT_STATE_EXPECTATION_s expectedInterlockValue =  {CONT_SWITCH_UNDEF, 0};
    taskENTER_CRITICAL();
    expectedInterlockValue.feedback = cont_interlock_state.expectation.feedback;
    expectedInterlockValue.timestamp = cont_interlock_state.expectation.timestamp;
    taskEXIT_CRITICAL();
    return expectedInterlockValue;
}

CONT_STATE_MEASUREMENT_s CONT_GetContactorFeedback(CONT_NAMES_e contactor) {

    CONT_STATE_MEASUREMENT_s measuredContactorState = {CONT_SWITCH_UNDEF, 0};
    if (CONT_HAS_NO_FEEDBACK == cont_contactors_cfg[contactor].feedback_pin) {
        uint32_t localTimer = osKernelSysTick();
        uint32_t timeContactorStateWasSet = cont_contactor_states[contactor].expectation.timestamp;
        // using the timing violation function to detected a time overflow...not pretty but ok...
        if (E_NOT_OK == TMG_TimingViolationSimple(timeContactorStateWasSet, localTimer, CONT_HAS_NO_FEEDBACK_SWICHTING_TIME)) {
            measuredContactorState.timestamp = localTimer;
            measuredContactorState.feedback = cont_contactor_states[contactor].expectation.feedback;
        }
    }
    else {
        // the contactor has a feedback pin, but it has to be differenced if the feedback pin is normally open or normally closed
        if (CONT_FEEDBACK_NORMALLY_OPEN == cont_contactors_cfg[contactor].feedback_pin_electrical_contact) {
            IO_PIN_STATE_e pinstate = IO_PIN_RESET;
            uint32_t localTimer = osKernelSysTick();
            taskENTER_CRITICAL();
            pinstate = IO_ReadPin(cont_contactors_cfg[contactor].feedback_pin);
            measuredContactorState.timestamp = localTimer;
            taskEXIT_CRITICAL();
            if (IO_PIN_RESET == pinstate) {
                measuredContactorState.feedback = CONT_SWITCH_ON;
            }
            else if (IO_PIN_SET == pinstate) {
                measuredContactorState.feedback = CONT_SWITCH_OFF;
            }
            else {
                measuredContactorState.feedback = CONT_SWITCH_UNDEF;
            }
        }
        if (CONT_FEEDBACK_NORMALLY_CLOSED == cont_contactors_cfg[contactor].feedback_pin_electrical_contact) {
            IO_PIN_STATE_e pinstate = IO_PIN_SET;
            uint32_t localTimer = osKernelSysTick();
            taskENTER_CRITICAL();
            pinstate = IO_ReadPin(cont_contactors_cfg[contactor].feedback_pin);
            measuredContactorState.timestamp = localTimer;
            taskEXIT_CRITICAL();
            if (IO_PIN_SET == pinstate) {
                measuredContactorState.feedback = CONT_SWITCH_ON;
            }
            else if (IO_PIN_RESET == pinstate) {
                measuredContactorState.feedback = CONT_SWITCH_OFF;
            }
            else {
                measuredContactorState.feedback = CONT_SWITCH_UNDEF;
            }
        }
    }
    return measuredContactorState;
}

CONT_STATE_MEASUREMENT_s CONT_GetInterlockFeedback(void) {
    CONT_STATE_MEASUREMENT_s measuredInterlockState = {CONT_SWITCH_UNDEF, 0};
    IO_PIN_STATE_e pinstate = IO_PIN_RESET;
    taskENTER_CRITICAL();
    pinstate = IO_ReadPin(cont_interlock_cfg.feedback_pin);
    taskEXIT_CRITICAL();
    if (IO_PIN_SET == pinstate) {
        measuredInterlockState.feedback = CONT_SWITCH_ON;
        measuredInterlockState.timestamp = osKernelSysTick();
    }
    else if (IO_PIN_RESET == pinstate) {
        measuredInterlockState.feedback = CONT_SWITCH_OFF;
        measuredInterlockState.timestamp = osKernelSysTick();
    }
    return measuredInterlockState;
}

STD_RETURN_TYPE_e CONT_AcquireContactorAndInterlockFeedbacks(void) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    taskENTER_CRITICAL();
    cont_interlock_state.measurement = CONT_GetInterlockFeedback();
    for(CONT_NAMES_e i = 0; i < (CONT_NAMES_e) cont_contactors_cfg_length; i++) {
        cont_contactor_states[i].measurement = CONT_GetContactorFeedback(i);
    }
    retVal = E_OK;
    taskEXIT_CRITICAL();
    return retVal;
}

STD_RETURN_TYPE_e CONT_SetContactorState(CONT_NAMES_e contactor, CONT_SWITCH_e requestedContactorState) {

    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    uint8_t legalState = FALSE;
    CONT_SWITCH_e currentContactorState = (CONT_GetContactorFeedback(contactor)).feedback;

    if (requestedContactorState == cont_contactor_states[contactor].expectation.feedback) {
        if (E_OK == CONT_CheckTimeViolation(contactor)) {
            retVal = E_OK;
        }
        if (E_OK != CONT_CheckTimeViolation(contactor)) {
            retVal = E_NOT_OK;
        }
    }
    else if (currentContactorState != requestedContactorState) {
        boolean setValue = FALSE;
        IO_PIN_STATE_e pinstate = IO_PIN_RESET;

        if(requestedContactorState  ==  CONT_SWITCH_ON) {
            setValue = TRUE;
            pinstate = IO_PIN_SET;
            legalState = TRUE;
        }
        else if(requestedContactorState  ==  CONT_SWITCH_OFF) {
            setValue = FALSE;
            pinstate = IO_PIN_RESET;
            legalState = TRUE;
        }
        else {
            retVal = E_NOT_OK;
            legalState = FALSE;
        }
        if (TRUE == legalState) {
            taskENTER_CRITICAL();
            /*
             * if the contactors are opened while the current is bigger than
             * BAD_SWITCHOFF_CURRENT_POS or BAD_SWITCHOFF_CURRENT_NEG this is counted
             * as a switch-off damaging the contactor.
             */
             if (requestedContactorState  ==  CONT_SWITCH_OFF) {
                 DATA_GetTable(&cont_current_tab, DATA_BLOCK_ID_CURRENT);
                 float currentAtSwitchOff = cont_current_tab.current;
                 if ( ((BAD_SWITCHOFF_CURRENT_POS < currentAtSwitchOff) && (0 < currentAtSwitchOff)) ||
                      ((BAD_SWITCHOFF_CURRENT_NEG > currentAtSwitchOff) && (0 > currentAtSwitchOff))) {
                     if(DIAG_HANDLER_RETURN_OK != DIAG_Handler(DIAG_CH_CONTACTOR_DAMAGED, DIAG_EVENT_NOK, (uint8_t) contactor, &currentAtSwitchOff)) {
                         /* currently no error handling, just logging */
                     }
                 }
                 else {
                     if(DIAG_HANDLER_RETURN_OK != DIAG_Handler(DIAG_CH_CONTACTOR_OPENING, DIAG_EVENT_OK, (uint8_t) contactor, NULL)) {
                         ;
                     }
                }
             } else {
                 if(DIAG_HANDLER_RETURN_OK != DIAG_Handler(DIAG_CH_CONTACTOR_CLOSING, DIAG_EVENT_OK, (uint8_t) contactor, NULL)) {
                     ;
                 }
             }
             uint32_t localTimer = osKernelSysTick();
             IO_WritePin(cont_contactors_cfg[contactor].control_pin, pinstate);

             cont_contactor_states[contactor].set.setvalue = setValue;
             cont_contactor_states[contactor].set.timestamp = localTimer;

             cont_contactor_states[contactor].expectation.feedback = requestedContactorState;
             cont_contactor_states[contactor].expectation.timestamp = localTimer;

             cont_contactor_states[contactor].measurement.feedback = CONT_SWITCH_UNDEF;
             cont_contactor_states[contactor].measurement.timestamp = 0;

            retVal = E_OK;

            taskEXIT_CRITICAL();
        }
    }
    else {
        retVal = E_NOT_OK;
    }

    return retVal;
}

STD_RETURN_TYPE_e CONT_SetInterlockState(CONT_SWITCH_e requstedInterlockState) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    uint8_t legalState = FALSE;
    CONT_SWITCH_e currentInterlockState = (CONT_GetInterlockFeedback()).feedback;

    if (currentInterlockState != requstedInterlockState) {
        boolean setValue = FALSE;
        IO_PIN_STATE_e pinstate = IO_PIN_RESET;

        if(requstedInterlockState  ==  CONT_SWITCH_ON) {
            setValue = TRUE;
            pinstate = IO_PIN_SET;
            legalState = TRUE;
        }
        else if(requstedInterlockState  ==  CONT_SWITCH_OFF) {
            setValue = FALSE;
            pinstate = IO_PIN_RESET;
            legalState = TRUE;
        }
        else {
            retVal = E_NOT_OK;
            legalState = FALSE;
        }
        if (TRUE == legalState) {
            taskENTER_CRITICAL();

            IO_WritePin(cont_interlock_cfg.control_pin, pinstate);
            uint32_t localTimer = osKernelSysTick();

            cont_interlock_state.set.setvalue = setValue;
            cont_interlock_state.set.timestamp = localTimer;

            cont_interlock_state.expectation.feedback = requstedInterlockState;
            cont_interlock_state.expectation.timestamp = localTimer;

            cont_interlock_state.measurement.feedback = CONT_SWITCH_UNDEF;
            cont_interlock_state.measurement.timestamp = 0;

            retVal = E_OK;

            taskEXIT_CRITICAL();
        }
    }
    else {
        /*
         * If the interlock is already in the requested state, do nothing.
         */
        retVal = E_OK;
    }

    return retVal;
}


STD_RETURN_TYPE_e CONT_SwitchAllContactorsOff(void) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    uint8_t offCounter = 0;
    STD_RETURN_TYPE_e successfullSet = E_NOT_OK;

    taskENTER_CRITICAL();
    for(CONT_NAMES_e i = 0; i < (CONT_NAMES_e) cont_contactors_cfg_length; i++) {
        successfullSet = CONT_SetContactorState(i, CONT_SWITCH_OFF);
        if (E_OK == successfullSet) {
            offCounter = offCounter + 1;
        }
        successfullSet = E_NOT_OK;
    }
    taskEXIT_CRITICAL();

    if (NR_OF_CONTACTORS == offCounter) {
        retVal = E_OK;
    }
    else {
        retVal = E_NOT_OK;
    }

    return retVal;
}


STD_RETURN_TYPE_e CONT_SwitchInterlockOff(void) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    retVal = CONT_SetInterlockState(CONT_SWITCH_OFF);
    for(CONT_NAMES_e contactor = 0; contactor < (CONT_NAMES_e) cont_contactors_cfg_length; contactor++) {
        cont_contactor_states[contactor].expectation.feedback = CONT_SWITCH_OFF;
        cont_contactor_states[contactor].set.setvalue = FALSE;
        IO_WritePin(cont_contactors_cfg[contactor].control_pin, IO_PIN_RESET);
    }
    return retVal;
}


STD_RETURN_TYPE_e CONT_SwitchInterlockOn(void) {
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    retVal = CONT_SetInterlockState(CONT_SWITCH_ON);
    return retVal;
}

/*================== Static functions =====================================*/

/**
 * @brief   Tries to open all contactors.
 *
 * Returns  TRUE, if all contactors (meaning NR_OF_CONTACTORS) can be are open
 *          or can be opened. If not, returns FALSE.
 *
 * @return  retVal (type: uint8_t/boolean)
 */
static uint8_t CONT_CheckForAllContactorsOpen(void) {
    uint8_t retVal = FALSE;

    CONT_AcquireContactorAndInterlockFeedbacks();
    uint8_t offCounter = 0;
    for(CONT_NAMES_e i = 0; i < (CONT_NAMES_e) cont_contactors_cfg_length; i++) {
        if (CONT_SWITCH_OFF == cont_contactor_states[i].measurement.feedback) {
            offCounter = offCounter + 1;
        }
    }

    if (NR_OF_CONTACTORS == offCounter) {
        retVal = TRUE;
    }

    return retVal;

}

/**
 * @brief   Checks if a contactor has closed in the defined time window
 *
 * Returns  TRUE, if the contactor has closed in the defined time window.
            If not, returns FALSE.
 *
 * @return  retVal (type: uint8_t/boolean)
 */
static STD_RETURN_TYPE_e CONT_CheckTimeViolation(CONT_NAMES_e contactor) {
    STD_RETURN_TYPE_e timingViolation = E_NOT_OK;
    uint32_t timeContactorStateWasSet = cont_contactor_states[contactor].expectation.timestamp;
    timingViolation = TMG_TimingViolationSimple(timeContactorStateWasSet, osKernelSysTick(), CONT_MAXIUMUM_ALLOWED_CONTACTOR_SWITCHING_TIME);
    return timingViolation;
}
