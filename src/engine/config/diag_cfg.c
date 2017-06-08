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
 * @file    diag_cfg.c
 * @author  foxBMS Team
 * @date    09.11.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  DIAG
 *
 * @brief   Diagnostic module configuration
 *
 * The configuration of the different diagnosis events defined in diag_cfg.h is set in the array
 * diag_ch_cfg[], e.g. initialization errors or runtime errors.
 *
 * Every entry of the diag_ch_cfg[] array consists of
 *  - name of the diagnosis event (defined in diag_cfg.h)
 *  - type of diagnosis event
 *  - diagnosis sensitivity (after how many occurrences event is counted as error)
 *  - enabling of the recording for diagnosis event
 *  - enabling of the diagnosis event
 *  - callback function for diagnosis event if wished, otherwise dummyfu
 *
 * The system monitoring configuration defined in diag_cfg.h is set in the array
 * diag_sysmon_ch_cfg[]. The system monitoring is at the moment only used for
 * supervising the cyclic/periodic tasks.
 *
 * Every entry of the diag_sysmon_ch_cfg[] consists of
 *  - enum of monitored object
 *  - type of monitored object (at the moment only DIAG_SYSMON_CYCLICTASK is supported)
 *  - maximum delay in [ms] in which the object needs to call the DIAG_SysMonNotify function defined in diag.c
 *  - enabling of the recording for system monitoring
 *  - enabling of the system monitoring for the monitored object
 *  - callback function if system monitoring notices an error if wished, otherwise dummyfu2
 */

/*================== Includes =============================================*/
#include "general.h"
#include "diag_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/**
 * Enable and Disable of Error Checks for Testing Purposes
 *
 * Each Bit enables or disables the diagnosis (checking of) the corresponding failure code
 * FIXME struct isn't used anywhere in the code at the moment.
 * FIXME delete if not needed
*/
DIAG_CODE_s diag_mask = {
        .GENERALmsk=0xFFFFFFFF,
        .CELLMONmsk=0xFFFFFFFF,
        .COMmsk=0xFFFFFFFF,
        .ADCmsk=0xFFFFFFFF,
};

/**
 * Callback function of diagnosis error events
 *
*/
void dummyfu(DIAG_CH_ID_e ch_id)
{
    ;
}

/**
 * Callback function of system monitoring error events
 *
*/
void dummyfu2(DIAG_SYSMON_MODULE_ID_e ch_id)
{
    ;
}


DIAG_CH_CFG_s  diag_ch_cfg[] = {

    /* OS-Framework and startup events */
    {DIAG_CH_FLASHCHECKSUM,             "FLASHCHECKSUM",            DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_BKPDIAG_FAILURE,           "BKPDIAG",                  DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_WATCHDOGRESET_FAILURE,     "WATCHDOGRESET",            DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_POSTOSINIT_FAILURE,        "POSTOSINIT",               DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CALIB_EEPR_FAILURE,        "CALIB_EEPR",               DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CAN_INIT_FAILURE,          "CAN_INIT",                 DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_NVIC_INIT_FAILURE,         "NVIC_INIT",                DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    /* HW-/SW-Runtime events */
    {DIAG_CH_DIV_BY_ZERO_FAILURE,       "DIV_BY_ZERO",              DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_UNDEF_INSTRUCTION_FAILURE, "UNDEF_INSTRUCTION",        DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_DATA_BUS_FAILURE,          "DATA_BUS_FAILURE",         DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_INSTRUCTION_BUS_FAILURE,   "INSTRUCTION_BUS",          DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_HARDFAULT_NOTHANDLED,      "HARDFAULT_NOTHANDLED",     DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_CONFIGASSERT,              "CONFIGASSERT",             DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_SYSTEMMONITORING_TIMEOUT,  "SYSTEMMONITORING_TIMEOUT", DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},


    /* Measurement events */
    {DIAG_CH_CANS_MAX_VALUE_VIOLATE,    "CANS_MAX_VALUE_VIOLATE",   DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CANS_MIN_VALUE_VIOLATE,    "CANS_MIN_VALUE_VIOLATE",   DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CANS_CAN_MOD_FAILURE,      "CANS_CAN_MOD_FAILURE",     DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_ISOMETER_TIM_ERROR,        "ISOMETER_TIM_ERROR",       DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_MID,  DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_ISOMETER_GROUNDERROR,      "ISOMETER_GROUNDERROR",     DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_ISOMETER_ERROR,            "ISOMETER_ERROR",           DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_MID,  DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_ISOMETER_MEAS_INVALID,     "ISOMETER_MEAS_INVALID",    DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_CELLVOLTAGE_OVERVOLTAGE,   "CELLVOLTAGE_OVERVOLTAGE",  DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CELLVOLTAGE_UNDERVOLTAGE,  "CELLVOLTAGE_UNDERVOLTAGE", DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_TEMP_MIN_VIOLATE,          "TEMPERATURE_MIN_VIOLATE",  DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_TEMP_MAX_VIOLATE,          "TEMPERATURE_MAX_VIOLATE",  DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_OVERCURRENT_CHARGE,        "OVERCURRENT_CHARGE",       DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_OVERCURRENT_DISCHARGE,     "OVERCURRENT_DISCHARGE",    DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    {DIAG_CH_LTC_SPI,                   "LTC_SPI",                  DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_COM_LTC_PEC,               "COM_LTC_PEC",              DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_LTC_MUX,                   "LTC_MUX",                  DIAG_GENERAL_TYPE, DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

    /* Communication events */
    /* currently no events defined */

    /* Contactor Damage Error*/
    {DIAG_CH_CONTACTOR_DAMAGED,         "CONTACTOR_DAMAGED",         DIAG_CONT_TYPE,    DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CONTACTOR_OPENING,         "CONTACTOR_OPENING",         DIAG_CONT_TYPE,    DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},
    {DIAG_CH_CONTACTOR_CLOSING,         "CONTACTOR_CLOSING",         DIAG_CONT_TYPE,    DIAG_ERROR_SENSITIVITY_HIGH, DIAG_RECORDING_ENABLED, DIAG_ENABLED, dummyfu},

};


DIAG_SYSMON_CH_CFG_s diag_sysmon_ch_cfg[]=
{
    {DIAG_SYSMON_DATABASE_ID,       DIAG_SYSMON_CYCLICTASK,  10, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_SYSCTRL_ID,        DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_LTC_ID,            DIAG_SYSMON_CYCLICTASK,   5, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_ISOGUARD_ID,       DIAG_SYSMON_CYCLICTASK, 400, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_CANS_ID,           DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_APPL_CYCLIC_1ms,   DIAG_SYSMON_CYCLICTASK,   2, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_APPL_CYCLIC_10ms,  DIAG_SYSMON_CYCLICTASK,  20, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_APPL_CYCLIC_100ms, DIAG_SYSMON_CYCLICTASK, 200, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
    {DIAG_SYSMON_BMSCTRL_ID,        DIAG_SYSMON_CYCLICTASK,   5, DIAG_RECORDING_ENABLED, DIAG_SYSMON_HANDLING_SWITCHOFFCONTACTOR, DIAG_ENABLED, dummyfu2},
};


DIAG_DEV_s  diag_dev = {
    .nr_of_ch   = sizeof(diag_ch_cfg)/sizeof(DIAG_CH_CFG_s),
    .ch_cfg     = &diag_ch_cfg[0],
};

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/
