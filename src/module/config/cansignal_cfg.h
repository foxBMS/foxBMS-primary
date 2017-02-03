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
 * This file has been automatically generated. Do not modify!
 *
 * @file                cansignal_cfg.h
 * @date                2015-09-16
 * @ingroup DRIVERS_CONF
 * @brief Headers for the configuration of the messages and signal settings for the CAN driver.
 *
 **/

#ifndef CANIF_CFG_H
#define CANIF_CFG_H

/*================== Includes =============================================*/
#include "general.h"
/*================== Macros and Definitions ===============================*/

/*fox
 * defines if the CAN Node 1 used in foxBMS. This is the node wired on the
 * BMS master extension board
 * @var use CAN Node 1
 * @type select(2)
 * @default 0
 * @level advanced
 * @group CAN
 */
#define CANS_USE_CAN_NODE1 TRUE
//#define CANS_USE_CAN_NODE1 FALSE

/*fox
 * defines if the CAN Node 2 used in foxBMS. This is the node wired on the
 * BMS master basic board
 * @var use CAN Node 2
 * @type select(2)
 * @default 0
 * @level advanced
 * @group CAN
 */
#define CANS_USE_CAN_NODE2 TRUE
//#define CANS_USE_CAN_NODE2 FALSE

/*fox
 * task timeslot where the CANS main function is called. Repetition time of periodic CAN messages must be multiple of this
 * @var CANSIGNAL Tick Time
 * @level read-only
 * @group CAN
 * @type select(3)
 * @default 1
 */
//#define CANS_TICK_MS 1
#define CANS_TICK_MS 10
//#define CANS_TICK_MS 100

#define NR_SIGNALS_TX 89
#define NR_SIGNALS_RX 15
#define NR_MESSAGES_TX 14
#define NR_MESSAGES_RX 6

/**
 * symbolic names for TX CAN messages
 */
typedef enum {
    CANS_MSG_BMS1, //!< BMS state
    CANS_MSG_BMS2, //!< Cell Voltages of Cells 0 1 2
    CANS_MSG_BMS3, //!< Cell Voltages of Cells 3 4 5
    CANS_MSG_BMS4, //!< Cell Voltages of Cells 6 7 8
    CANS_MSG_BMS5, //!< Cell Voltages of Cells 9 10 11
    CANS_MSG_BMS6, //!< temperture/tempering
    CANS_MSG_BMS7, //!< SOC/SOH
    CANS_MSG_BMS8, //!< SOF
    CANS_MSG_BMS9, //!< Isolation/Balancing
    CANS_MSG_BMS11, //!< Temperatures of modules
    CANS_MSG_BMS12, //!< Cell Voltages of Cells 12 13 14
    CANS_MSG_BMS13, //!< Temperatures extended of modules
    CANS_MSG_BMS14, //!< Cell Voltages Max Min Average
    CANS_MSG_BMS_CurrentTrigger    //!< Cell Voltages Max Min Average
} CANS_messagesTx_e;

/**
 * symbolic names for RX CAN messages
 */
typedef enum {
    //CANS_MSG_BMS_Dummy, //!< DummyMsg for Testing
    CANS_MSG_BMS10,                           //!< state request
    CANS_MSG_ISENS0,                         //!< current sensing
    CANS_MSG_ISENS1,                         //!< current sensor voltage 1
    CANS_MSG_ISENS2,                         //!< current sensor voltage 2
    CANS_MSG_ISENS3,                         //!< current sensor voltage 3
    CANS_MSG_DEBUG                           //!< debug messages
} CANS_messagesRx_e;

/**
 * symbolic names for CAN transmission signals
 */
typedef enum {
    CAN_SIG_BMS1_general_error, // only 2 bits
    CAN_SIG_BMS1_current_state,
    CAN_SIG_BMS1_error_hightemp_bat, //  0:good, 1:error
    CAN_SIG_BMS1_error_lowtemp_bat, //  0:good, 1:error
    CAN_SIG_BMS1_error_overcurrent_charge, // 0:good, 1:error
    CAN_SIG_BMS1_error_overcurrent_discharge, // 0:good, 1:error
    CAN_SIG_BMS1_error_overvoltage, //  0:good, 1:error
    CAN_SIG_BMS1_error_undervoltage, //  0:good, 1:error
    CAN_SIG_BMS1_error_contactor, //  0:good, 1:error
    CAN_SIG_BMS1_error_selftest, // 0:good, 1:error
    CAN_SIG_BMS1_states_relays,  // bitfield 0:off, 1:on
    CAN_SIG_BMS1_error_cantiming, // 0:good, 1:warning, 2:error
    CAN_SIG_BMS1_balancing_active, // 0:off, 1:on
    CAN_SIG_BMS1_status_msg_overflowing_counter, //!< CAN_SIGNAL_status_msg_overflowing_counter
    CANS_SIG_BMS1_reserved, //!<
    CANS_SIG_BMS2_reserved, //!<
    CANS_SIG_BMS2_Mux, //!< Cell Voltages of Cells 0 1 2 of Module 0
    CANS_SIG_BMS2__Mod0_Cell0_Value, //!< Voltage of Cell 0 of Module 0
    CANS_SIG_BMS2__Mod0_Cell1_Value, //!< Voltage of Cell 1 of Module 0
    CANS_SIG_BMS2__Mod0_Cell2_Value, //!< Voltage of Cell 2 of Module 0
    CANS_SIG_BMS3_Mux, //!< Cell Voltages of Cells 3 4 5 of Module 0
    CANS_SIG_BMS3__Mod0_Cell3_Value, //!< Voltage of Cell 3 of Module 0
    CANS_SIG_BMS3__Mod0_Cell4_Value, //!< Voltage of Cell 4 of Module 0
    CANS_SIG_BMS3__Mod0_Cell5_Value, //!< Voltage of Cell 5 of Module 0
    CANS_SIG_BMS4_Mux, //!< Cell Voltages of Cells 6 7 8 of Module 0
    CANS_SIG_BMS4__Mod0_Cell6_Value, //!< Voltage of Cell 6 of Module 0
    CANS_SIG_BMS4__Mod0_Cell7_Value, //!< Voltage of Cell 7 of Module 0
    CANS_SIG_BMS4__Mod0_Cell8_Value, //!< Voltage of Cell 8 of Module 0
    CANS_SIG_BMS5_Mux, //!< Cell Voltages of Cells 9 10 11 of Module 0
    CANS_SIG_BMS5__Mod0_Cell9_Value, //!< Voltage of Cell 9 of Module 0
    CANS_SIG_BMS5__Mod0_Cell10_Value, //!< Voltage of Cell 10 of Module 0
    CANS_SIG_BMS5__Mod0_Cell11_Value, //!< Voltage of Cell 11 of Module 0
    CANS_SIG_BMS6_Cooling_Needed, //!< Flag Signal if Cooling needed
    CANS_SIG_BMS6_Heating_Needed, //!< Flag Signal if Heating needed
    CANS_SIG_BMS6_Tempering_Demand, //!< Need for Tempering
    CANS_SIG_BMS6_Average_Temp, //!< Average Temp
    CANS_SIG_BMS6_Minimum_Temp, //!< Minimum Temp
    CANS_SIG_BMS6_Maximum_Temp, //!< Maximum Temp
    CANS_SIG_BMS6_MaxTemp_Module_Number, //!< Module Number of Module with highest temperature
    CANS_SIG_BMS6_MinTemp_Module_Number, //!< Module Number of Module with lowest temperature
    CANS_SIG_BMS6_reserved, //!< reserved
    CANS_SIG_BMS7_SOC_Average, //!< State of Charge averaged over all cells
    CANS_SIG_BMS7_SOC_Minimum, //!< State of Charge minimum of all cells
    CANS_SIG_BMS7_SOC_Maximum, //!< State of Charge maximum of all cells
    CANS_SIG_BMS7_SOH_Average, //!< State of Health averaged over all cells
    CANS_SIG_BMS7_SOH_Minimum, //!< State of Health minimum of all cells
    CANS_SIG_BMS7_SOH_Maximum, //!< State of Health maximum of all cells
    CANS_SIG_BMS7_reserved, //!< reserved
    CANS_SIG_BMS8_Current_Charge_Max,              //!< SOF charge continuous
    CANS_SIG_BMS8_Current_Charge_Max_Peak,         //!< SOF charge peak
    CANS_SIG_BMS8_Current_Discharge_Max,           //!< SOF discharge continuous
    CANS_SIG_BMS8_Current_Discharge_Max_Peak,      //!< SOF discharge peak
    CANS_SIG_BMS9_Isolation_Good, //!< State of IsoGood Pin of Bender
    CANS_SIG_BMS9_Isolation_Value, //!< Isolation PWM value of Bender
    CANS_SIG_BMS9_Balancing_Module_Number, //!< Module number of balancing active indicator
    CANS_SIG_BMS9_Balancing_Active_Cells, //!< Balancing Active indication bitfield
    CANS_SIG_BMS9_reserved, //!<
    CANS_SIG_BMS9_Selftest_state, //!< tbd
    CANS_SIG_BMS9_Selftest_Debug, //!<
    CANS_SIG_BMS11_Mux, //!< Cell Temperatures of Modules
    CANS_SIG_BMS11_reserved, //!< reserved
    CANS_SIG_BMS11_Temp_Mod0_Temp0, //!< Temp 0 of Module 0
    CANS_SIG_BMS11_Temp_Mod0_Temp1, //!< Temp 1 of Module 0
    CANS_SIG_BMS11_Temp_Mod0_Temp2, //!< Temp 2 of Module 0
    CANS_SIG_BMS11_Temp_Mod0_Temp3, //!< Temp 3 of Module 0
    CANS_SIG_BMS11_Temp_Mod0_Temp4, //!< Temp 4 of Module 0
    CANS_SIG_BMS11_Temp_Mod0_Temp5, //!< Temp 5 of Module 0
    CANS_SIG_BMS11_Temp_Mod0_Temp6, //!< Temp 6 of Module 0
    CANS_SIG_BMS11_Temp_Mod0_Temp7, //!< Temp 7 of Module 0
    CANS_SIG_BMS12_reserved, //!<
    CANS_SIG_BMS12_Mux, //!< Cell Voltages of Cells 12 13 14 of Module 0
    CANS_SIG_BMS12__Mod0_Cell12_Value, //!< Voltage of Cell 13 of Module 0
    CANS_SIG_BMS12__Mod0_Cell13_Value, //!< Voltage of Cell 14 of Module 0
    CANS_SIG_BMS12__Mod0_Cell14_Value, //!< Voltage of Cell 15 of Module 0
    CANS_SIG_BMS13_Mux, //!<
    CANS_SIG_BMS13_reserved, //!< reserved
    CANS_SIG_BMS13_Temp_Mod0_Temp8, //!< Temp 8 of Module 0
    CANS_SIG_BMS13_Temp_Mod0_Temp9, //!< Temp 9 of Module 0
    CANS_SIG_BMS13_Temp_Mod0_Temp10, //!< Temp 10 of Module 0
    CANS_SIG_BMS13_Temp_Mod0_Temp11, //!< Temp 11 of Module 0
    CANS_SIG_BMS13_Temp_Mod0_Temp12, //!< Temp 12 of Module 0
    CANS_SIG_BMS13_Temp_Mod0_Temp13, //!< Temp 13 of Module 0
    CANS_SIG_BMS13_Temp_Mod0_Temp14, //!< Temp 14 of Module 0
    CANS_SIG_BMS14_VoltageAverage, //!< Average Voltage of Cells
    CANS_SIG_BMS14_VoltageMinimum, //!< Minimum Voltage of Cells
    CANS_SIG_BMS14_VoltageMaximum, //!< Maximum Voltage of Cells
    CANS_SIG_BMS14_ModuleNrMinVolt, //!< ModuleNr with Minimum Voltage
    CANS_SIG_BMS14_ModuleNrMaxVolt,               //!< ModuleNr with Maximum Voltage
    CANS_SIGNAL_NONE = 0xFFFF
} CANS_signalsTx_e;

/**
 * symbolic names for receive signals
 */
typedef enum {
    CANS_SIG_BMS10_Overflow,         //!< current sensor measurement type
    CANS_SIG_BMS10_Request,         //!< current sensor counter
    ISENS0_I_Byte0,         //!< current sensor measurement type
    ISENS0_I_Byte1,         //!< current sensor counter
    ISENS0_I_Measurement,   //!< current sensor measurement I
    ISENS1_U1_Byte0,        //!< current sensor measurement type
    ISENS1_U1_Byte1,        //!< current sensor counter
    ISENS1_U1_Measurement,  //!< current sensor measurement U1
    ISENS2_U2_Byte0,        //!< current sensor measurement type
    ISENS2_U2_Byte1,        //!< current sensor counter
    ISENS2_U2_Measurement,  //!< current sensor measurement U2
    ISENS3_U3_Byte0,        //!< current sensor measurement type
    ISENS3_U3_Byte1,        //!< current sensor counter
    ISENS3_U3_Measurement,  //!< current sensor measurement U3
    DEBUG_Data              //!< Data of debug message
} CANS_signalsRx_e;

typedef enum {
    CAN_RX_DIRECTION = 0,
    CAN_TX_DIRECTION = 1
} CANS_messageDirection_t;

typedef uint32_t (*cans_callback_funcPtr)(uint32_t idx, void * value);

/**
 * type definition for structure of a CAN message with its
 *  ID,
 *  data length code,
 *  repetition rate (stated in number of calls of CANS mainfunction = ticks),
 *  the initial phase,
 *  a callback function if transfer of TX message to CAN module is successful
 */
typedef struct  {
    uint32_t id;                    //!< CAN message id
    uint8_t dlc;                    //!< CAN message data length code
    uint32_t repetition_time;       //!< CAN message cycle time
    uint32_t repetition_phase;      //!< CAN message startup (first send) offset
    cans_callback_funcPtr cbk_func; //!< CAN message callback after message is sent or received
} CANS_message_s;

typedef union {
    CANS_messagesTx_e Tx;
    CANS_messagesRx_e Rx;
} CANS_messages_t;

typedef union {
    CANS_signalsTx_e Tx;
    CANS_signalsRx_e Rx;
} CANS_signals_t;

/**
 * type definition for structure of a CAN signal
 *
 * until now, multiplexed signal handling is hard coded
 * in the corresponding getters/setters. For use of multiplexed
 * signals refer to description in documentation.
 *
 * support for automatic scaling is planned, but not implemented yet,
 * so min, max, factor and offset are not relevant.
 */
typedef struct  {
    CANS_messages_t msgIdx;
    uint8_t bit_position;
    uint8_t bit_length;
    float max;
    float min;
    float factor;
    float offset;
    boolean isMuxed;
    boolean isMuxor;
    uint8_t muxValue;
    CANS_signals_t muxor;
    cans_callback_funcPtr setter;
    cans_callback_funcPtr getter;
} CANS_signal_s;

/*================== Constant and Variable Definitions ====================*/

/**
 * array for transmission CAN message definition
 */
extern const CANS_message_s cans_messages_tx[];
/**
 * array for received CAN message definition
 */
extern const CANS_message_s cans_messages_rx[];

/**
 * array for transmission CAN signals definition
 */
extern const CANS_signal_s cans_signals_tx[];
/**
 * array for received CAN signals definition
 */
extern const CANS_signal_s cans_signals_rx[];

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/

#endif
