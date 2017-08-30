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
 * @file    bmsctrl.h
 * @author  foxBMS Team
 * @date    04.12.2015 (date of creation)
 * @ingroup ENGINE
 * @prefix  BMSCTRL
 *
 * @brief   Bmscontrol driver header
 *
 */

#ifndef BMSCTRL_H_
#define BMSCTRL_H_

/*================== Includes =============================================*/
#include "bmsctrl_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/**
 * symbolic names of possible states the BMSCTRL statemachine can be in
 */
typedef enum {
    BMSCTRL_STATEMACH_UNINITIALIZED             = 0,     /*!< Init-Sequence: BMS control state machine uninitialized   */
    BMSCTRL_STATEMACH_INITIALIZATION            = 1,     /*!< Init-Sequence: BMS control state machine initializing    */
    BMSCTRL_STATEMACH_REINIT                    = 2,     /*!< Init-Sequence: BMS control state machine reinitializing  */
    BMSCTRL_STATEMACH_INITIALIZED               = 3,     /*!< Init-Sequence: BMS control state machine initialized     */
    // States                                            /*!< */
    BMSCTRL_STATEMACH_IDLE                      = 4,     /*!< */
    BMSCTRL_STATEMACH_CHECK_MEAS_DIAG           = 5,     /*!< */
    BMSCTRL_STATEMACH_CHECK_NEWCANREQ           = 6,     /*!< */
    BMSCTRL_STATEMACH_CHECK_CANTIMING           = 7,     /*!< */
    BMSCTRL_STATEMACH_BAD_CANTIMING             = 8,     /*!< */
    BMSCTRL_STATEMACH_BAD_MEAS_DIAG             = 9,     /*!< */
    BMSCTRL_STATEMACH_EVERYTHINGOK              = 10,    /*!< */
    BMSCTRL_STATEMACH_SOC_INIT_CHECK_MEAS_DIAG  = 11,    /*!< */
    // Reserved States                                   /*!< */
    BMSCTRL_STATEMACH_RESERVED_abc              = 0x80,  /*!< */
    BMSCTRL_STATEMACH_ERROR_abc                 = 0xF0,  /*!< Error-State:  */
    BMSCTRL_STATEMACH_ERROR_def                 = 0xF1,  /*!< Error-State:  */
    BMSCTRL_STATEMACH_ERROR_xyz                 = 0xF2,  /*!< Error-State:  */
} BMSCTRL_STATEMACH_e;

/**
 * symbolic names of possible requests that can be set up to the BMSCTRL statemachine
 */
typedef enum {
    BMSCTRL_STATE_INIT_REQUEST      = BMSCTRL_STATEMACH_INITIALIZATION, /*!< */
    BMSCTRL_STATE_REINIT_REQUEST    = BMSCTRL_STATEMACH_REINIT,         /*!< */
    BMSCTRL_STATE_IDLE_REQUEST      = BMSCTRL_STATEMACH_IDLE,           /*!< */
    BMSCTRL_STATE_NO_REQUEST        = BMSCTRL_STATEMACH_RESERVED_abc,   /*!< */
} BMSCTRL_STATE_REQUEST_e;

/**
 * symbolic names of substates of init state, until now only used for demonstration
 */
typedef enum {
    // Init-Sequence
    BMSCTRL_INIT_ENTRY  = 0,    /*!< Initialization entry point */
    BMSCTRL_INIT_STEP1  = 1,    /*!< */
    BMSCTRL_INIT_STEP2  = 2,    /*!< */
} BMSCTRL_SUBSTATE_INIT_e;

/**
 * possible action states of the BMSCTRL statemachine
 */
typedef enum {
    BMSCTRL_OK                  = 0,    /*!< */
    BMSCTRL_BUSY_OK             = 1,    /*!< */
    BMSCTRL_REQUEST_PENDING     = 2,    /*!< */
    BMSCTRL_NOTHING_TO_DO       = 3,    /*!< */
    BMSCTRL_ERROR               = 20,   /*!< */
    BMSCTRL_ILLEGAL_TASK_TYPE   = 99,   /*!< Illegal BMS control task*/
} BMSCTRL_RETURN_TYPE_e;

/**
 * main information of BMSCTRL statemachine is packed in variable of this type
 *
 */
typedef struct {
    uint16_t timer;                     /*!< e.g. in counts of 1ms                          */
    BMSCTRL_STATE_REQUEST_e statereq;   /*!< state of Driver State Machine                  */
    BMSCTRL_STATEMACH_e state;          /*!< state of Driver State Machine                  */
    uint8_t substate;                   /*!< sub-state                                      */
    BMSCTRL_STATEMACH_e laststate;      /*!< state of State Machine                         */
    uint8_t lastsubstate;               /*!< sub-state of state                             */
    uint8_t errMeasCnt;                 /*!< Measurement error counter                      */
    uint16_t errCANCnt;                 /*!< CAN error counter                              */
    uint32_t errRequestCnt;             /*!< Error request counter                          */
    uint16_t timer_after_open;          /*!< e.g. in counts of 1ms                          */
    uint8_t triggerentry;               /*!< re-entrance protection (function running flag) */
} BMSCTRL_STATE_s;

/*================== Function Prototypes ==================================*/
/**
 * @brief   periodically handles the actions in the statemachine's state of the module BMSCTRL
 *
 * @return  void
 */
extern void BMSCTRL_Trigger(void);

/**
 * @brief   handles the transition from one state to another in the statemachine of the module BMSCTRL
 *
 * @return  the state of the action the BMSCTRL statemachine is actually performing
 */
extern BMSCTRL_RETURN_TYPE_e BMSCTRL_Ctrl(void);

/**
 * @brief   sets a request for next state in statemachine of the module BMSCTRL that will be
 *          handled by next call to BMSCTRL_Ctrl
 *
 * @param   statereq the state that someone wants to be requested
 */
extern BMSCTRL_RETURN_TYPE_e BMSCTRL_SetStateRequest(BMSCTRL_STATE_REQUEST_e statereq);

/*================== Function Implementations =============================*/

#endif /* BMSCTRL_H_ */
