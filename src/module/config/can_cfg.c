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
 * @file    can_cfg.c
 * @author  foxBMS Team
 * @date    2015-07-12
 * @ingroup DRIVERS_CONF
 * @prefix  CAN
 *
 * @brief   Configuration for the CAN module.
 *
 * The CAN bus settings and the received messages and their
 * reception handling are to be specified here.
 *
 */

/*================== Includes =============================================*/
#include "can_cfg.h"
#include "rcc_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

/* ***************************************
 *  Set CAN settings here
 ****************************************/

CAN_HandleTypeDef hcan1 = {
        .Instance = CAN1,
        .Lock = HAL_UNLOCKED,
        .State = HAL_CAN_STATE_RESET,
        .ErrorCode = HAL_CAN_ERROR_NONE,
#if (CAN_BAUDRATE == 1000000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 3,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/3/14 = 1.0MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 4,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/4/8 = 1.0MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#elif (CAN_BAUDRATE == 500000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 6,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/6/14 = 0.5MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 8,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/8/8 = 0.5MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#elif (CAN_BAUDRATE == 250000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 12,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/12/14 = 0.25MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 16,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/16/8 = 0.25MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#elif (CAN_BAUDRATE == 125000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 24,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/12/14 = 0.125MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 32,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/16/8 = 0.125MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#endif
        .Init.Mode = CAN_MODE_NORMAL,    // for test purpose, without connected can-bus use LOOPBACK mode
        .Init.SJW = CAN_SJW_1TQ,
        .Init.TTCM = DISABLE,    // time triggerd communication mode
                            // DISABLE: no influence
                            // ENABLE: saves timestamps for received and transmitted messages. See reference manual for more information.
        .Init.ABOM = ENABLE,    // automatic bus-off management
                            // DISABLE: Manually re-initialize CAN and wait for 128 * 11 recessive bits
                            // ENABLE: automatically leave bus-off mode after 128 * 11 recessive bits
        .Init.AWUM = ENABLE,     // automatic wake-up mode
                            // ENABLE: automatically leave sleep mode on message receiving
                            // DISABLE: SLEEP bit needs to be deleted by software
        .Init.NART = DISABLE,     // automatic retransition mode;
                            // DISABLE: retransmit the message until it has been successfully transmitted
                            // ENABLE: transmit only once, independently of transmission result
        .Init.RFLM = ENABLE,    // Receive FIFO locked against overrun.
                            // DISABLE: A new incoming message overwrites the last received message.
                            // ENABLE: Once a receive FIFO is full the next incoming message will be discarded.
        .Init.TXFP = DISABLE,     // Transmit FIFO priority
                            // DISABLE: driven by identifier of message. Lower identifier equals higher priority
                            // ENABLE: driven chronologically
};


CAN_HandleTypeDef hcan2 = {
        .Instance = CAN2,
        .Lock = HAL_UNLOCKED,
        .State = HAL_CAN_STATE_RESET,
        .ErrorCode = HAL_CAN_ERROR_NONE,
#if (CAN_BAUDRATE == 1000000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 3,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/12/14 = 1.0MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 4,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/16/8 = 1.0MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#elif (CAN_BAUDRATE == 500000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 6,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/6/14 = 0.5MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 8,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/8/8 = 0.5MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#elif (CAN_BAUDRATE == 250000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 12,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/12/14 = 0.25MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 16,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/16/8 = 0.25MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#elif (CAN_BAUDRATE == 125000)
#if (RCC_APB1_CLOCK  ==  42000000)
        .Init.Prescaler = 24,        // CAN_CLOCK = APB1 = 42MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_6TQ,    // --> CAN = 42MHz/12/14 = 0.125MHz
        .Init.BS2 = CAN_BS2_7TQ,
#elif RCC_APB1_CLOCK  ==  32000000
        .Init.Prescaler = 32,        // CAN_CLOCK = APB1 = 32MHz
                                    // resulting CAN speed: APB1/prescaler/sumOfTimequants
                                    // sum: 1tq for sync + BS1 + BS2
        .Init.BS1 = CAN_BS1_5TQ,    // --> CAN = 32MHz/16/8 = 0.125MHz
        .Init.BS2 = CAN_BS2_2TQ,
#else
#error "Please configure CAN Baudrate according to your clock configuration "
#endif
#endif
        .Init.Mode = CAN_MODE_NORMAL,  // for test purpose, without connected can-bus use LOOPBACK mode
        .Init.SJW = CAN_SJW_1TQ,
        .Init.TTCM = DISABLE,   // time triggerd communication mode
                            // DISABLE: no influence
                            // ENABLE: saves timestamps for received and transmitted messages. See reference manual for more information.
        .Init.ABOM = ENABLE,    // automatic bus-off management
                            // DISABLE: Manually re-initialize CAN and wait for 128 * 11 recessive bits
                            // ENABLE: automatically leave bus-off mode after 128 * 11 recessive bits
        .Init.AWUM = ENABLE,    // automatic wake-up mode
                            // ENABLE: automatically leave sleep mode on message receiving
                            // DISABLE: SLEEP bit needs to be deleted by software
        .Init.NART = DISABLE,   // automatic retransition mode;
                            // DISABLE: retransmit the message until it has been successfully transmitted
                            // ENABLE: transmit only once, independently of transmission result
        .Init.RFLM = ENABLE,    // Receive FIFO locked against overrun.
                            // DISABLE: A new incoming message overwrites the last received message.
                            // ENABLE: Once a receive FIFO is full the next incoming message will be discarded.
        .Init.TXFP = DISABLE,   // Transmit FIFO priority
                            // DISABLE: driven by identifier of message. Lower identifier equals higher priority
                            // ENABLE: driven chronologically
};


/* ***************************************
 *  Configure RX messages here
 ****************************************/

/* Bypassed messages are --- ALSO --- to be configured here. See further down for bypass ID setting!  */
CAN_MSG_RX_TYPE_s can1_RxMsgs[CAN1_NUMBER_OF_RX_IDs] = {
        { 0x152, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< state request      */
        { 0x35C, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor I   */
        { 0x35D, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor U1  */
        { 0x35E, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor U2  */
        { 0x35F, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor U3  */
        { 0x55E, 0xFFFF, 8, 0, CAN_FIFO0, NULL }    /*!< debug message      */
};

CAN_MSG_RX_TYPE_s can2_RxMsgs[CAN2_NUMBER_OF_RX_IDs] = {
        { 0x152, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< state request      */
        { 0x35C, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor I   */
        { 0x35D, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor U1  */
        { 0x35E, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor U2  */
        { 0x35F, 0xFFFF, 8, 0, CAN_FIFO0, NULL },   /*!< current sensor U3  */
        { 0x55E, 0xFFFF, 8, 0, CAN_FIFO0, NULL }    /*!< debug message      */};
/* ***************************************
 *  Set bypass message IDs here
 ****************************************/

/* These IDs have to be included in the configuration for the filters in can_RxMsgs[]! */
uint32_t can1_bufferBypass_RxMsgs[CAN1_BUFFER_BYPASS_NUMBER_OF_IDs] = { };

uint32_t can2_bufferBypass_RxMsgs[CAN2_BUFFER_BYPASS_NUMBER_OF_IDs] = { };

