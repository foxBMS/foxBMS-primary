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
 * @file    contactor.h
 * @author  foxBMS Team
 * @date    23.09.2015
 * @ingroup DRIVERS
 * @prefix  CONT
 *
 * @brief   Header for the driver for the contactors
 *
 * This file makes the declaration of the public functions of the contactor
 * driver.
 *
 */

#ifndef CONTACTOR_H_
#define CONTACTOR_H_
/*================== Includes =============================================*/
#include "contactor_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/
/**
 * @brief   Checks the configuration of the contactor-module
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e CONT_Init(void);

/**
 * @brief   Gets the latest value (TRUE, FALSE) the contactors were set to.
 *
 * Meaning of the return value:
 *   - FALSE means contactor was set to be opened
 *   - TRUE means contactor was set to be closed
 *
 * @param   contactor (type: CONT_NAMES_e)
 * @return  setInformation (type: CONT_STATE_SET_s)
 */
extern CONT_STATE_SET_s CONT_GetContactorSetValue(CONT_NAMES_e contactor);

/**
 * @brief   Gets the latest value (TRUE, FALSE) the interlock was set to.
 *
 * Meaning of the return value:
 *   - FALSE means interlock was set to be opened
 *   - TRUE means interlock was set to be closed
 *
 * @return  setInformation (type: CONT_STATE_SET_s)
 */
extern CONT_STATE_SET_s CONT_GetInterlockSetValue(void);

/**
 * @brief   Gets the value (CONT_SWITCH_OFF/CONT_SWITCH_ON) the specified contactor
 *          was set to.
 * @param   contactor (type: CONT_NAMES_e)
 * @return  expectedContactorValue (type: CONT_STATE_EXPECTATION_s)
 */
extern CONT_STATE_EXPECTATION_s CONT_GetContactorExpectedValue(CONT_NAMES_e contactor);

/**
 * @brief   Gets the value (CONT_SWITCH_OFF/CONT_SWITCH_ON) the interlock was set to.
 *
 * @return  expectedContactorValue (type: CONT_STATE_EXPECTATION_s)
 */
extern CONT_STATE_EXPECTATION_s CONT_GetInterlockExpectedValue(void);

/**
 * @brief   Reads the feedback pin of every contactor and returns its current value
 *          (CONT_SWITCH_OFF/CONT_SWITCH_ON).
 *
 * If the contactor has a feedback pin the measured feedback is returned. If the
 * contactor has no feedback pin, it is assumed that after a certain time the
 * contactor has reached the requested state.
 *
 * @param   contactor (type: CONT_NAMES_e)
 * @return  measuredContactorState (type: CONT_STATE_MEASUREMENT_s)
 */
extern CONT_STATE_MEASUREMENT_s CONT_GetContactorFeedback(CONT_NAMES_e contactor);

/**
 * @brief   Reads the feedback pin of the interlock and returns its current value
 *          (CONT_SWITCH_OFF/CONT_SWITCH_ON)
 * @return  measuredInterlockState (type: CONT_STATE_MEASUREMENT_s)
 */
extern CONT_STATE_MEASUREMENT_s CONT_GetInterlockFeedback(void);

/**
 * @brief   Reads the feedback pins of the interlock and all contactors and updates the
 *          interlock_cfg variable and contactors_cfg[] array with their current states.
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e CONT_AcquireContactorAndInterlockFeedbacks(void);

/**
 * @brief   Sets the contactor state to its requested state, if the contactor is at that time not in the requested state.
 *
 * If the new state was already requested, but not reached (meaning the measured feedback does not return the requested state),
 * there are two states: it can be still ok (E_OK), because the contactor has some time left to get physically in the requested state
 * (passed time since the request is lower than the limit) or it can be not ok (E_NOT_OK), because there is timing violation, i.e. the
 * contactor has surpassed the maximum time for getting in the requested state.
 * It returns E_OK if the requested state was successfully set or if the contactor was at the requested state before.
 *
 * @param   contactor (type: CONT_NAMES_e)
 * @param   requestedContactorState (type: CONT_SWITCH_e)
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e CONT_SetContactorState(CONT_NAMES_e contactor, CONT_SWITCH_e requestedContactorState);

/**
 * @brief   Sets the interlock state to its requested state, if the interlock is at that time not in the requested state.
 *
 * It returns E_OK if the requested state was successfully set or if the interlock was at the requested state before.
 * @param   requstedInterlockState (type: CONT_SWITCH_e)
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e CONT_SetInterlockState(CONT_SWITCH_e requstedInterlockState);

/**
 * @brief   Runs over the contactor array and switches all contactors off
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e CONT_SwitchAllContactorsOff(void);

/**
 * @brief   Switches the interlock off and returns E_NOT_OK on success.
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e CONT_SwitchInterlockOff(void);

/**
 * @brief   Switches the interlock on and returns E_OK on success.
 * @return  retVal (type: STD_RETURN_TYPE_e)
 */
extern STD_RETURN_TYPE_e CONT_SwitchInterlockOn(void);

/*================== Function Implementations =============================*/
#endif // CONTACTOR_H_
