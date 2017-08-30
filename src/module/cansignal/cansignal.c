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
 * @file    cansignal.c
 * @author  foxBMS Team
 * @date    01.10.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  CANS
 *
 * @brief   Messages and signal settings for the CAN driver
 *
 * generic conversion module of Can signals from CAN buffered reception to
 * DATA Manager and vice versa
 *
 */

/*================== Includes =============================================*/
/* recommended include order of header files:
 * 
 * 1.    include general.h
 * 2.    include module's own header
 * 3...  other headers
 *
 */
#include "general.h"
#include "cansignal.h"

#include "can.h"
#include "diag.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/
static STD_RETURN_TYPE_e CANS_PeriodicTransmit(void);
static STD_RETURN_TYPE_e CANS_PeriodicReceive(void);
static void CANS_SetSignalData(CANS_signal_s signal, uint64_t value, uint8_t *dataPtr);
static void CANS_GetSignalData(uint64_t *dst, CANS_signal_s signal, uint8_t *dataPtr);
static void CANS_ComposeMessage(CAN_NodeTypeDef_e canNode, CANS_messagesTx_e msgIdx, uint8_t dataptr[]);
static void CANS_ParseMessage(CAN_NodeTypeDef_e canNode, CANS_messagesRx_e msgIdx, uint8_t dataptr[]);
/*================== Function Implementations =============================*/

/*================== Public functions =====================================*/
void CANS_Init(void) {
    // custom initialization could be made here. right now no need for any init
}

void CANS_MainFunction(void) {
    (void)CANS_PeriodicTransmit();
    (void)CANS_PeriodicReceive();
    DIAG_SysMonNotify(DIAG_SYSMON_CANS_ID,0);        // task is running, state = ok
}

/*================== Static functions =====================================*/
/**
 * handles the processing of messages that are meant to be transmitted.
 *
 * This function looks for the repetition times and the repetition phase of
 * messages that are intended to be sent periodically. If a comparison with
 * an internal counter (i.e., the counter how often this function has been called)
 * states that a transmit is pending, the message is composed by call of CANS_ComposeMessage
 * and transfered to the buffer of the CAN module. If a callback function
 * is declared in configuration, this callback is called after successful transmission.
 *
 * @return E_OK if a successful transfer to CAN buffer occured, E_NOT_OK otherwise
 */
static STD_RETURN_TYPE_e CANS_PeriodicTransmit(void) {
    static uint32_t counter_ticks = 0;
    uint32_t i = 0;
    STD_RETURN_TYPE_e result = E_NOT_OK;

#if CAN_USE_CAN_NODE0 == TRUE
    for(i = 0; i < can_CAN0_tx_length; i++) {
        if(((counter_ticks * CANS_TICK_MS) % (can_CAN0_messages_tx[i].repetition_time)) == can_CAN0_messages_tx[i].repetition_phase) {
            Can_PduType PduToSend = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x0, 8 };
            CANS_ComposeMessage(CAN_NODE0, (CANS_messagesTx_e)(i), PduToSend.sdu);
            PduToSend.id = can_CAN0_messages_tx[i].ID;

            result = CAN_Send(CAN_NODE0, PduToSend.id, PduToSend.sdu, PduToSend.dlc, 0);

            if (result == E_NOT_OK) {
                DIAG_Handler(DIAG_CH_CANS_CAN_MOD_FAILURE, DIAG_EVENT_NOK, 1, NULL_PTR);
            }
            else {
                DIAG_Handler(DIAG_CH_CANS_CAN_MOD_FAILURE, DIAG_EVENT_OK, 1, NULL_PTR);
            }
            if(can_CAN0_messages_tx[i].cbk_func != NULL_PTR && result == E_OK) {
                can_CAN0_messages_tx[i].cbk_func(i, NULL_PTR);
            }
        }
    }
#endif

#if CAN_USE_CAN_NODE1 == TRUE
    for(i = 0; i < can_CAN1_tx_length; i++) {
        if(((counter_ticks * CANS_TICK_MS) % (can_CAN1_messages_tx[i].repetition_time)) == can_CAN1_messages_tx[i].repetition_phase) {
            Can_PduType PduToSend = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 0x0, 8 };
            CANS_ComposeMessage(CAN_NODE1, (CANS_messagesTx_e)i + can_CAN0_tx_length, PduToSend.sdu);
            PduToSend.id = can_CAN1_messages_tx[i].ID;

            result = CAN_Send(CAN_NODE1, PduToSend.id, PduToSend.sdu, PduToSend.dlc, 0);

            if (result == E_NOT_OK) {
                DIAG_Handler(DIAG_CH_CANS_CAN_MOD_FAILURE, DIAG_EVENT_NOK, 0, NULL_PTR);
            }
            else {
                DIAG_Handler(DIAG_CH_CANS_CAN_MOD_FAILURE, DIAG_EVENT_OK, 0, NULL_PTR);
            }
            if(can_CAN1_messages_tx[i].cbk_func != NULL_PTR && result == E_OK) {
                can_CAN1_messages_tx[i].cbk_func(i, NULL_PTR);
            }
        }
    }
#endif

    result = E_NOT_OK;

    counter_ticks++;
    return TRUE;
}

/**
 * handles the processing of received CAN messages.
 *
 * This function gets the messages in the receive buffer
 * of the CAN module. If a message ID is
 * matching one of the IDs in the configuration of
 * CANS module, the signal processing is executed
 * by call to CANS_ParseMessage.
 *
 * @return E_OK, if a message has been received and parsed, E_NOT_OK otherwise
 */
static STD_RETURN_TYPE_e CANS_PeriodicReceive(void) {
    Can_PduType msg = {};
    STD_RETURN_TYPE_e result_node0 = E_NOT_OK, result_node1 = E_NOT_OK;
    uint32_t i = 0;

#if CAN_USE_CAN_NODE0 == TRUE
    while(CAN_ReceiveBuffer(CAN_NODE0, &msg)  ==  E_OK) {
        for(i = 0; i < can_CAN0_rx_length; i++) {
            if(msg.id  ==  can0_RxMsgs[i].ID) {
                CANS_ParseMessage(CAN_NODE0, (CANS_messagesRx_e)i, msg.sdu);
                result_node0 =E_OK;
            }
        }
    }
#else
    result_node0 = E_OK;
#endif

#if CAN_USE_CAN_NODE1 == TRUE
    while(CAN_ReceiveBuffer(CAN_NODE1, &msg) == E_OK) {
        for(i = 0; i < can_CAN1_rx_length; i++) {
            if(msg.id == can1_RxMsgs[i].ID) {
                CANS_ParseMessage(CAN_NODE1, (CANS_messagesRx_e)i + can_CAN0_rx_length, msg.sdu);
                result_node1 = E_OK;
            }
        }
    }
#else
    result_node1 = E_OK;
#endif

    return result_node0 && result_node1;
}
/**
 * @brief   generates bitfield, which masks the bits where the actual signal (defined by its bitlength) is located
 *
 * @param   bitlength   length of the signal in bits
 *
 * @return  bitmask     bitfield mask
 */
static uint64_t CANS_GetBitmask(uint8_t bitlength) {
    uint64_t bitmask = 0x00000000;
    uint8_t i = 0;
    for(i = 0; i < bitlength; i++) {
        bitmask = bitmask << 1;
        bitmask = bitmask | 0x00000001;
    }
    return bitmask;
}

/**
 * extracts signal data from CAN message data
 *
 * @param[out] dst       pointer where the signal data should be copied to
 * @param[in]  signal    signal identifier
 * @param[in]  dataPtr   CAN message data, from which signal data is extracted
 */
static void CANS_GetSignalData(uint64_t *dst, CANS_signal_s signal, uint8_t *dataPtr) {
    uint8_t bitposition = signal.bit_position;
    uint8_t bitlength = (uint8_t)signal.bit_length;
    uint64_t bitmask = 0x00000000;
    uint64_t *dataPtr64 = (uint64_t *)dataPtr;

    bitmask = CANS_GetBitmask(bitlength);
    *dst = (((*dataPtr64) >> bitposition) & bitmask);
    //*dst = signal.factor*(float)(((*dataPtr64) >> bitposition) & bitmask) + signal.offset;
    if (*dst > signal.max) {
        DIAG_Handler(DIAG_CH_CANS_MAX_VALUE_VIOLATE, DIAG_EVENT_NOK, 0, NULL);
    }
    else{
        DIAG_Handler(DIAG_CH_CANS_MAX_VALUE_VIOLATE, DIAG_EVENT_OK, 0, NULL);
    }
    if (*dst < signal.min) {
        DIAG_Handler(DIAG_CH_CANS_MIN_VALUE_VIOLATE, DIAG_EVENT_NOK, 0, NULL);
    }
    else{
        DIAG_Handler(DIAG_CH_CANS_MIN_VALUE_VIOLATE, DIAG_EVENT_OK, 0, NULL);
    }
}
/**
 * assembles signal data in CAN message data
 *
 * @param signal    signal identifier
 * @param value     signal value data
 * @param dataPtr   CAN message data, in which the signal data is inserted
 */
static void CANS_SetSignalData(CANS_signal_s signal, uint64_t value, uint8_t *dataPtr) {
    uint8_t bitposition = signal.bit_position;
    uint8_t bitlength = (uint8_t)signal.bit_length;
    uint64_t bitmask = 0x0000000000000000;
    uint64_t *dataPtr64 = (uint64_t *)dataPtr;

    bitmask = CANS_GetBitmask(bitlength);
    dataPtr64[0] &= ~(((uint64_t)bitmask) << bitposition);
    dataPtr64[0] |= ((((uint64_t)value) & bitmask) << bitposition);
}

/**
 * composes message data from all signals associated with this msgIdx
 *
 * signal data is received by callback getter functions
 *
 * @param[in] msgIdx   message index for which the data should be composed
 * @param[out] dataptr  pointer where the message data should be stored to
 */
static void CANS_ComposeMessage(CAN_NodeTypeDef_e canNode, CANS_messagesTx_e msgIdx, uint8_t dataptr[]) {
    uint32_t i = 0;
    uint32_t muxorIdx = 0;
    uint32_t nrTxSignals = 0;
    // find multiplexor if multiplexed signal

    CANS_signal_s *cans_signals_tx;

    if(canNode == CAN_NODE0) {
        cans_signals_tx = (CANS_signal_s *)&cans_CAN0_signals_tx;
        nrTxSignals = cans_CAN0_signals_tx_length;
    }
    else if(canNode == CAN_NODE1) {
        cans_signals_tx = (CANS_signal_s *)&cans_CAN1_signals_tx;
        nrTxSignals = cans_CAN1_signals_tx_length;
    }
    for(i = 0; i < nrTxSignals; i++) {
        if((cans_signals_tx[i].msgIdx.Tx  ==  msgIdx) &&
          (cans_signals_tx[i].isMuxor) &&
          (cans_signals_tx[(uint32_t)cans_signals_tx[i].muxor.Tx].getter != NULL_PTR)) {
              cans_signals_tx[(uint32_t)cans_signals_tx[i].muxor.Tx].getter((uint32_t)cans_signals_tx[i].muxor.Tx, &muxorIdx);
        }
    }
    for(i = 0; i < nrTxSignals; i++) {
        if(cans_signals_tx[i].msgIdx.Tx  ==  msgIdx) {
            if(cans_signals_tx[i].isMuxed && !cans_signals_tx[i].isMuxor) {
                // multiplexed signal
                if(muxorIdx == cans_signals_tx[i].muxValue) {
                    uint64_t value = 0;
                    if(cans_signals_tx[i].getter != NULL_PTR) {
                        cans_signals_tx[i].getter(i, &value);
                    }
                    CANS_SetSignalData(cans_signals_tx[i], value, dataptr);
                }
                else {
                    // do nothing
                }
            }
            else {
                // simple, not multiplexed signal
                uint64_t value = 0;
                if(cans_signals_tx[i].getter != NULL_PTR) {
                    cans_signals_tx[i].getter(i, &value);
                }
                CANS_SetSignalData(cans_signals_tx[i], value, dataptr);
            }
        }
        else {
            // do nothing
        }
    }
}
/**
 * @brief   parses signal data from message associated with this msgIdx
 *
 * signal data is received by callback setter functions
 *
 * @param[in]   msgIdx   message index for which the data should be parsed
 * @param[in]   dataptr  pointer where the message data is stored
*/
static void CANS_ParseMessage(CAN_NodeTypeDef_e canNode, CANS_messagesRx_e msgIdx, uint8_t dataptr[]){
    uint32_t i = 0;

    if(canNode == CAN_NODE0) {
        for(i = 0; i < cans_CAN0_signals_rx_length; i++){
            /* Iterate over CAN0 rx signals and find message */

            if(cans_CAN0_signals_rx[i].msgIdx.Rx  ==  msgIdx) {
                uint64_t value = 0;
                CANS_GetSignalData(&value, cans_CAN0_signals_rx[i], dataptr);
                if(cans_CAN0_signals_rx[i].setter != NULL_PTR) {
                    cans_CAN0_signals_rx[i].setter(i, &value);
                }
            }
        }

    }
    else if(canNode == CAN_NODE1) {
        for(i = 0; i < cans_CAN1_signals_rx_length; i++){
            /* Iterate over CAN1 rx signals and find message */

            if(cans_CAN1_signals_rx[i].msgIdx.Rx  ==  msgIdx) {
                uint64_t value = 0;
                CANS_GetSignalData(&value, cans_CAN1_signals_rx[i], dataptr);
                if(cans_CAN1_signals_rx[i].setter != NULL_PTR) {
                    cans_CAN1_signals_rx[i].setter(cans_CAN0_signals_rx_length + i, &value);
                }
            }
        }
    }

}
