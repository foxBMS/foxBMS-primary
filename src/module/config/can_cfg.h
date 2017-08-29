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
 * @file    can_cfg.h
 * @author  foxBMS Team
 * @date    12.07.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  CAN
 *
 * @brief   Headers for the configuration for the CAN module
 *
 * The activation and the length of the message buffers as well as the number of
 * the messages that are received are to be configured here
 *
 */

#ifndef CAN_CFG_H_
#define CAN_CFG_H_

/*================== Includes =============================================*/

#include "mcu_cfg.h"


/*================== Macros and Definitions ===============================*/

/* **************************************************************************************
 *  CAN BUFFER OPTIONS
 *****************************************************************************************/
/*fox
 * CAN bus baudrate. CAN peripheral prescaler and time quantums on microcontroller
 * will be configured accordingly. See STM32 Reference Manual p. 1097 for more
 *
 * @var CAN bus baudrate
 * @type select(4)
 * @default 1
 * @level advanced
 * @group CAN
 */

/**
 * defines the BAUD rate of the CAN
 */
//#define CAN_BAUDRATE 1000000
#define CAN_BAUDRATE 500000
//#define CAN_BAUDRATE 250000
//#define CAN_BAUDRATE 125000

/* CAN enabling */
/*fox
 * Enables or disables CAN0
 * @var     CAN_USE_CAN_NODE0
 * @type    int
 * @valid   x == 0 or x == 1
 * @default 1
 * @group   CAN
 * @level   advanced
 */
#define CAN_USE_CAN_NODE0                1

/*fox
 * Enables or disables CAN1
 * @var     CAN_USE_CAN_NODE1
 * @type    int
 * @valid   x == 0 or x == 1
 * @default 1
 * @group   CAN
 * @level   advanced
 */
#define CAN_USE_CAN_NODE1                1

/* transmit buffer */
/*fox
 * Enables or disables CAN0 transmit buffer
 * @var     CAN0_USE_TRANSMIT_BUFFER
 * @type    int
 * @valid   x == 0 or x == 1
 * @default 1
 * @group   CAN
 * @level   advanced
 */
#define CAN0_USE_TRANSMIT_BUFFER         1
/*fox
 * Defines CAN0 transmit buffer length
 * @var     CAN0_TRANSMIT_BUFFER_LENGTH
 * @type    int
 * @valid   0 < x
 * @default 16
 * @group   CAN
 * @level   advanced
 */
#define CAN0_TRANSMIT_BUFFER_LENGTH      16

/*fox
 * Enables or disables CAN1 transmit buffer
 * @var     CAN1_USE_TRANSMIT_BUFFER
 * @type    int
 * @valid   x == 0 or x == 1
 * @default 1
 * @group   CAN
 * @level   advanced
 */
#define CAN1_USE_TRANSMIT_BUFFER         1
/*fox
 * Defines CAN1 transmit buffer length
 * @var     CAN1_TRANSMIT_BUFFER_LENGTH
 * @type    int
 * @valid   0 < x
 * @default 16
 * @group   CAN
 * @level   advanced
 */
#define CAN1_TRANSMIT_BUFFER_LENGTH      16

/* receive buffer */
/*fox
 * Enables or disables CAN0 receive buffer
 * @var     CAN0_USE_RECEIVE_BUFFER
 * @type    int
 * @valid   x == 0 or x == 1
 * @default 1
 * @group   CAN
 * @level   advanced
 */
#define CAN0_USE_RECEIVE_BUFFER          1
/*fox
 * Defines CAN0 receive buffer length
 * @var     CAN0_RECEIVE_BUFFER_LENGTH
 * @type    int
 * @valid   0 < x
 * @default 16
 * @group   CAN
 * @level   advanced
 */
#define CAN0_RECEIVE_BUFFER_LENGTH       16

/*fox
 * Enables or disables CAN1 receive buffer
 * @var     CAN1_USE_RECEIVE_BUFFER
 * @type    int
 * @valid   x == 0 or x == 1
 * @default 1
 * @group   CAN
 * @level   advanced
 */
#define CAN1_USE_RECEIVE_BUFFER          1
/*fox
 * Defines CAN1 receive buffer length
 * @var     CAN1_RECEIVE_BUFFER_LENGTH
 * @type    int
 * @valid   0 < x
 * @default 16
 * @group   CAN
 * @level   advanced
 */
#define CAN1_RECEIVE_BUFFER_LENGTH       16


/* Number of messages that will bypass the receive buffer and will be interpreted right on reception.
 * Set the respective IDs and implement the wished functionality either in individual callback
 * function or in default STD_RETURN_TYPE_e CAN_BufferBypass(CAN_NodeTypeDef_e canNode, uint32_t msgID,
 * uint8_t* data, uint8_t DLC, uint8_t RTR) function in the can.c file. Use bypassing only for
 * important messages because of handling during ISR */
/*fox
 * Defines number of RX messages that bypass receive buffer on CAN0 bus
 * @var     CAN0_BUFFER_BYPASS_NUMBER_OF_IDs
 * @type    int
 * @valid   0 <= x
 * @default 0
 * @group   CAN
 * @level   advanced
 */
#define CAN0_BUFFER_BYPASS_NUMBER_OF_IDs 2

/*fox
 * Defines number of RX messages that bypass receive buffer on CAN1 bus
 * @var     CAN1_BUFFER_BYPASS_NUMBER_OF_IDs
 * @type    int
 * @valid   0 <= x
 * @default 0
 * @group   CAN
 * @level   advanced
 */
#define CAN1_BUFFER_BYPASS_NUMBER_OF_IDs 2

/*fox
 * defines if the Isabellenhuette current sensor is used in cyclic or triggered mode
 * @group CAN
 */
#define CAN_ISABELLENHUETTE_CYCLIC
// #define CAN_ISABELLENHUETTE_TRIGGERED

/*fox
 * Defines CAN message ID to perform a software reset
 * @var     CAN_SOFTWARE_RESET_MSG_ID
 * @type    int
 * @vaild   [1, 4094]
 * @default 351
 * @group   CAN
 * @level   advanced
 */
#define CAN_SOFTWARE_RESET_MSG_ID                     0x15F


/*fox
 * When enabled unique device ID is need as can data to perform SW reset
 * @var     CAN_SW_RESET_WITH_DEVICE_ID
 * @type    int
 * @valid   [0,1]
 * @default 0
 * @group   CAN
 * @level   advanced
 */
//  #define CAN_SW_RESET_WITH_DEVICE_ID        1
#define CAN_SW_RESET_WITH_DEVICE_ID        0

/*fox
 * Defines CAN message ID to switch into bootloader and download new software
 * @var     CAN_DL_NEW_APPLICATION_SOFTWARE_MSG_ID
 * @type    int
 * @vaild   [1, 4094]
 * @default 1373
 * @group   CAN
 * @level   advanced
 */
#define CAN_DL_NEW_APPLICATION_SOFTWARE_MSG_ID        0x55D

/* Hardware settings*/
/*fox
 * Number of hardware filterbanks
 * @var     CAN_FILTERBANKS
 * @type    int
 * @default 28
 * @group   CAN
 * @level   advanced
 */
#define CAN_NUMBER_OF_FILTERBANKS       28  // dependable on the MCU (STM32F4 -> 28)

typedef struct CAN_MSG_RX_TYPE {
    uint32_t ID;    /*!< message ID*/
    uint32_t mask;  /*!< mask or 0x0000 to select list mode*/
    uint8_t DLC;    /*!< data length*/
    uint8_t RTR;    /*!< rtr bit*/
    uint8_t fifo;   /*!< selected CAN hardware (CAN_FIFO0 or CAN_FIFO1)*/
    STD_RETURN_TYPE_e (*func)(uint32_t ID, uint8_t*, uint8_t, uint8_t);    /*!< callback function*/
} CAN_MSG_RX_TYPE_s;


typedef uint32_t (*can_callback_funcPtr)(uint32_t idx, void * value);

/**
 * type definition for structure of a CAN message with its
 *  ID,
 *  data length code,
 *  repetition rate (stated in number of calls of CANS mainfunction = ticks),
 *  the initial phase,
 *  a callback function if transfer of TX message to CAN module is successful
 */
typedef struct  {
    uint32_t ID;                    //!< CAN message id
    uint8_t DLC;                    //!< CAN message data length code
    uint32_t repetition_time;       //!< CAN message cycle time
    uint32_t repetition_phase;      //!< CAN message startup (first send) offset
    can_callback_funcPtr cbk_func; //!< CAN message callback after message is sent or received
} CAN_MSG_TX_TYPE_s;

typedef struct CanPdu {
    uint8_t sdu[8];
    uint32_t id;
    uint8_t dlc;
} Can_PduType;


/*================== Constant and Variable Definitions ====================*/
extern CAN_HandleTypeDef hcan0;
extern CAN_HandleTypeDef hcan1;
extern CAN_MSG_RX_TYPE_s can0_RxMsgs[];
extern CAN_MSG_RX_TYPE_s can1_RxMsgs[];
extern uint32_t can0_bufferBypass_RxMsgs[CAN0_BUFFER_BYPASS_NUMBER_OF_IDs];
extern uint32_t can1_bufferBypass_RxMsgs[CAN1_BUFFER_BYPASS_NUMBER_OF_IDs];
extern const CAN_MSG_TX_TYPE_s can_CAN0_messages_tx[];
extern const CAN_MSG_TX_TYPE_s can_CAN1_messages_tx[];


/**
 * array length for receiving CAN0 message definition
 */
extern const uint8_t can_CAN0_rx_length;

/**
 * array length for receiving CAN1 message definition
 */
extern const uint8_t can_CAN1_rx_length;

/**
 * array length for transmission CAN0 message definition
 */
extern const uint8_t can_CAN0_tx_length;

/**
 * array length for transmission CAN1 message definition
 */
extern const uint8_t can_CAN1_tx_length;

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

#endif /* CAN_CFG_H_ */
