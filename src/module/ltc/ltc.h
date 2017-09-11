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
 * @file    ltc.h
 * @author  foxBMS Team
 * @date    01.09.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  LTC
 *
 * @brief   Headers for the driver for the LTC monitoring chip.
 *
 */

#ifndef LTC_H_
#define LTC_H_

/*================== Includes =============================================*/
#include "ltc_cfg.h"


/*================== Macros and Definitions ===============================*/
/**
 * Number of multiplexer measurements per LTC cycle
 */
#define LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE 1

#if SLAVE_BOARD_VERSION == 1
#elif SLAVE_BOARD_VERSION == 2
#else
#error Please select the slave board version you want to use. Configuration file: \src\module\config\ltc_cfg.h
#endif

/*================== Constant and Variable Definitions ====================*/

/**
 * Voltage measurement mode
 * 1: 01 27kHz Mode (Fast) 14kHz Mode
 * 2: 10 7kHz Mode (Normal) 3kHz Mode
 * 3: 11 26Hz Mode (Filtered) 2kHz Mode
 */
typedef enum {
    LTC_ADCMODE_UNDEFINED       = 0,    /*!< ADC measurement mode is not defined        */
    LTC_ADCMODE_FAST_DCP0       = 1,    /*!< ADC measurement mode: Fast with DCP0       */
    LTC_ADCMODE_NORMAL_DCP0     = 2,    /*!< ADC measurement mode: Normal with DCP0     */
    LTC_ADCMODE_FILTERED_DCP0   = 3,    /*!< ADC measurement mode: Filtered with DCP0   */
    LTC_ADCMODE_FAST_DCP1       = 4,    /*!< ADC measurement mode: Fast with DCP1       */
    LTC_ADCMODE_NORMAL_DCP1     = 5,    /*!< ADC measurement mode: Normal with DCP1     */
    LTC_ADCMODE_FILTERED_DCP1   = 6,    /*!< ADC measurement mode: Filtered with DCP1   */
} LTC_ADCMODE_e;

/**
 * Number of measured channels
 */
typedef enum {
    LTC_ADCMEAS_UNDEFINED       = 0,    /*!< not defined                            */
    LTC_ADCMEAS_ALLCHANNEL      = 1,    /*!< all ADC channels are measured          */
    LTC_ADCMEAS_SINGLECHANNEL   = 2,    /*!< only a single ADC channel is measured  */
} LTC_ADCMEAS_CHAN_e;

/**
 * States of the LTC state machine
 */
typedef enum {
    // Init-Sequence
    LTC_STATEMACH_UNINITIALIZED             = 0,    /*!<    */
    LTC_STATEMACH_INITIALIZATION            = 1,    /*!<    */
    LTC_STATEMACH_REINIT                    = 2,    /*!<    */
    LTC_STATEMACH_INITIALIZED               = 3,    /*!< LTC is initialized                             */
    // Voltage Measurement Cycle
    LTC_STATEMACH_IDLE                      = 4,    /*!<    */
    LTC_STATEMACH_STARTMEAS                 = 5,    /*!<    */
    LTC_STATEMACH_READVOLTAGE               = 6,    /*!<    */
    LTC_STATEMACH_STARTMUXMEASUREMENT       = 7,    /*!< Mux (Temperature and Balancing) Measurement    */
    LTC_STATEMACH_MUXCONFIGURATION_INIT     = 8,    /*!<    */
    LTC_STATEMACH_MUXMEASUREMENT_CONFIG     = 9,    /*!< Configuration of the multiplexers              */
    LTC_STATEMACH_MUXMEASUREMENT            = 10,   /*!<    */
    LTC_STATEMACH_MUXMEASUREMENT_FINISHED   = 11,   /*!<    */
    LTC_STATEMACH_BALANCECONTROL            = 14,   /*!<    */
    LTC_STATEMACH_ALLGPIOMEASUREMENT        = 15,   /*!<    */
    LTC_STATEMACH_READALLGPIO               = 16,   /*!<    */
    LTC_STATEMACH_UNDEFINED                 = 20,   /*!< undefined state                                */
    LTC_STATEMACH_RESERVED1                 = 0x80, /*!< reserved state                                 */
    LTC_STATEMACH_ERROR_SPIFAILED           = 0xF0, /*!< Error-State: SPI error                         */
    LTC_STATEMACH_ERROR_PECFAILED           = 0xF1, /*!< Error-State: PEC error                         */
    LTC_STATEMACH_ERROR_MUXFAILED           = 0xF2, /*!< Error-State: multiplexer error                 */
} LTC_STATEMACH_e;

/**
 * General substates
 */
typedef enum {
    LTC_ENTRY               = 0,    /*!< Substate entry state       */
    LTC_ERROR_ENTRY         = 1,    /*!< Substate entry error       */
    LTC_ERROR_PROCESSED     = 2,    /*!< Substate error processed   */
} LTC_STATEMACH_SUB_e;

/**
 * Substates for the uninitialized state
 */
typedef enum {
    LTC_ENTRY_UNINITIALIZED     = 0,    /*!< Initialize-sequence */
} LTC_STATEMACH_UNINITIALIZED_SUB_e;

/**
 * Substates for the initialization state
 */
typedef enum {
    // Init-Sequence
    LTC_ENTRY_INITIALIZATION        = 0,    /*!<    */
    LTC_START_INIT_INITIALIZATION   = 1,    /*!<    */
    LTC_RE_ENTRY_INITIALIZATION     = 2,    /*!<    */
    LTC_EXIT_INITIALIZATION         = 3,    /*!<    */
} LTC_STATEMACH_INITIALIZATION_SUB_e;

/**
 * Substates for the uninitialized state
 */
typedef enum {
    LTC_ENTRY_INITIALIZED   = 0,    /*!<    */
} LTC_STATEMACH_INITIALIZED_SUB_e;

/**
 * Substates for the read voltage state
 */
typedef enum {
    // Init-Sequence
    LTC_READ_VOLTAGE_REGISTER_A_RDCVA_READVOLTAGE   = 0,    /*!<    */
    LTC_READ_VOLTAGE_REGISTER_B_RDCVB_READVOLTAGE   = 1,    /*!<    */
    LTC_READ_VOLTAGE_REGISTER_C_RDCVC_READVOLTAGE   = 2,    /*!<    */
    LTC_READ_VOLTAGE_REGISTER_D_RDCVD_READVOLTAGE   = 3,    /*!<    */
    LTC_EXIT_READVOLTAGE                            = 4,    /*!<    */
} LTC_STATEMACH_READVOLTAGE_SUB_e;

/**
 * Substates for the balance control state
 */
typedef enum {
    // Init-Sequence
    LTC_ENTRY_BALANCECONTROL    = 0,    /*!<    */
    LTC_EXIT_BALANCECONTROL     = 1,    /*!<    */
} LTC_STATEMACH_BALANCECONTROL_SUB;

/**
 * Substates for the multiplexer measurement configuration state
 */
typedef enum {
    // Init-Sequence
    LTC_SET_MUX_CHANNEL_WRCOMM_MUXMEASUREMENT_CONFIG                = 0,    /*!<    */
    LTC_SEND_CLOCK_STCOMM_MUXMEASUREMENT_CONFIG                     = 1,    /*!<    */
    LTC_READ_I2C_TRANSMISSION_RESULT_RDCOMM_MUXMEASUREMENT_CONFIG   = 2,    /*!<    */
    LTC_START_GPIO_MEASUREMENT_MUXMEASUREMENT_CONFIG                = 3,    /*!<    */
} LTC_STATEMACH_MUXMEASUREMENT_CONFIG_SUB_e;

/**
 * Substates for the all-GPIO multiplexer measurement state
 */
typedef enum {
    // Init-Sequence
    LTC_READ_AUXILIARY_REGISTER_A_RAUXA_READALLGPIO = 0,    /*!<    */
    LTC_READ_AUXILIARY_REGISTER_B_RAUXB_READALLGPIO = 1,    /*!<    */
    LTC_EXIT_READALLGPIO                            = 2,    /*!<    */
} LTC_STATEMACH_READALLGPIO_SUB_e;

/**
 * Substates for the single GPIO multiplexer measurement state
 */
typedef enum {
    // Init-Sequence
    LTC_READ_AUXILIARY_REGISTER_A_RAUXA_MUXMEASUREMENT  = 0,    /*!<    */
    LTC_SAVE_MUX_MEASUREMENT_MUXMEASUREMENT             = 1,    /*!<    */
} LTC_STATEMACH_MUXMEASUREMENT_SUB_e;

/**
 * State requests for the LTC statemachine
 */
typedef enum {
    LTC_STATE_INIT_REQUEST                = LTC_STATEMACH_INITIALIZATION,           /*!<    */
    LTC_STATE_REINIT_REQUEST              = LTC_STATEMACH_REINIT,                   /*!<    */
    LTC_STATE_IDLE_REQUEST                = LTC_STATEMACH_IDLE,                     /*!<    */
    LTC_STATE_VOLTAGEMEASUREMENT_REQUEST  = LTC_STATEMACH_STARTMEAS,                /*!<    */
    LTC_STATE_READVOLTAGE_REQUEST         = LTC_STATEMACH_READVOLTAGE,              /*!<    */
    LTC_STATE_MUXMEASUREMENT_REQUEST      = LTC_STATEMACH_STARTMUXMEASUREMENT,      /*!<    */
    LTC_STATE_BALANCECONTROL_REQUEST      = LTC_STATEMACH_BALANCECONTROL,           /*!<    */
    LTC_STATE_ALLGPIOMEASUREMENT_REQUEST  = LTC_STATEMACH_ALLGPIOMEASUREMENT,       /*!<    */
    LTC_STATE_NO_REQUEST                  = LTC_STATEMACH_RESERVED1,                /*!<    */
} LTC_STATE_REQUEST_e;

/**
 * Possible return values when state requests are made to the LTC statemachine
 */
typedef enum {
    LTC_OK                  = 0,    /*!< LTC --> ok                             */
    LTC_BUSY_OK             = 1,    /*!< LTC under load --> ok                  */
    LTC_REQUEST_PENDING     = 2,    /*!< requested to be executed               */
    LTC_ILLEGAL_REQUEST     = 3,    /*!< Request can not be executed            */
    LTC_SPI_ERROR           = 4,    /*!< Error state: Source: SPI               */
    LTC_PEC_ERROR           = 5,    /*!< Error state: Source: PEC               */
    LTC_MUX_ERROR           = 6,    /*!< Error state: Source: MUX               */
    LTC_INIT_ERROR          = 7,    /*!< Error state: Source: Initialization    */
    LTC_OK_FROM_ERROR       = 8,    /*!< Return from error --> ok               */
    LTC_ERROR               = 20,   /*!< General error state                    */
    LTC_ALREADY_INITIALIZED = 30,   /*!< Initialization of LTC already finished */
    LTC_ILLEGAL_TASK_TYPE   = 99,   /*!< Illegal                                */
} LTC_RETURN_TYPE_e;

/**
 * Variable to define if the LTC should measure are restart the initialization sequence
 */
typedef enum {
    LTC_HAS_TO_MEASURE  = 0,    /*!< measurement state of the LTC       */
    LTC_HAS_TO_REINIT   = 1,    /*!< re-initialization state of the LTC */
} LTC_TASK_TYPE_e;

/**
 * Table indicating which LTC chips and which multiplexers do not respond in the daisy-chain
 */
typedef struct {
    int LTC[LTC_NUMBER_OF_LTC_PER_MODULE];  /*!<    */
    int mux0;                               /*!<    */
    int mux1;                               /*!<    */
    int mux2;                               /*!<    */
    int mux3;                               /*!<    */

} LTC_ERRORTABLE_s;

/**
 * This structure contains all the variables relevant for the LTC state machine.
 * The user can get the current state of the LTC state machine with this variable
 */
typedef struct {
    uint16_t timer;                         /*!< time in ms before the state machine processes the next state, e.g. in counts of 1ms    */
    LTC_STATE_REQUEST_e statereq;           /*!< current state request made to the state machine                                        */
    LTC_STATEMACH_e state;                  /*!< state of Driver State Machine                                                          */
    uint8_t substate;                       /*!< current substate of the state machine                                                  */
    LTC_STATEMACH_e laststate;              /*!< previous state of the state machine                                                    */
    uint8_t lastsubstate;                   /*!< previous substate of the state machine                                                 */
    LTC_ADCMODE_e adcMode;                  /*!< current LTC ADCmeasurement mode (fast, normal or filtered)                                             */
    LTC_ADCMODE_e adcModereq;               /*!< requested LTC ADCmeasurement mode (fast, normal or filtered)                                           */
    LTC_ADCMEAS_CHAN_e adcMeasCh;           /*!< current number of channels measured for GPIOS (one at a time for multiplexers or all five GPIOs)       */
    LTC_ADCMEAS_CHAN_e adcMeasChreq;        /*!< requested number of channels measured for GPIOS (one at a time for multiplexers or all five GPIOs)     */
    uint8_t numberOfMeasuredMux;            /*!< number of multiplexer channels measured by the LTC chip before a voltage measurement is made           */
    uint32_t ErrPECCounter;                 /*!< counts the number of times there was A PEC (CRC) error during communication with LTC*/
    uint8_t ErrRetryCounter;                /*!< counts how many times the drivers retried to communicate with LTC in case of a communication error*/
    uint32_t ErrRequestCounter;             /*!< counts the number of illegal requests to the LTC state machine */
    uint8_t triggerentry;                   /*!< counter for re-entrance protection (function running flag) */
    uint32_t commandDataTransferTime;       /*!< time needed for sending an instruction to the LTC, followed by data transfer from the LTC   */
    uint32_t commandTransferTime;           /*!< time needed for sending an instruction to the LTC                                           */
    uint32_t gpioClocksTransferTime;        /*!< time needed for sending 72 clock signal to the LTC, used for I2C communication              */
    uint32_t VoltageSampleTime;             /*!< time stamp at which the cell voltage were measured                                          */
    uint32_t muxSampleTime;                 /*!< time stamp at which a multiplexer input was measured                                        */
    LTC_MUX_CH_CFG_s *muxmeas_seqptr;       /*!< pointer to the multiplexer sequence to be measured (contains a list of elements [multiplexer id, multiplexer channels]) (1,-1)...(3,-1),(0,1),...(0,7)*/
    LTC_MUX_CH_CFG_s *muxmeas_seqendptr;    /*!< point to the end of the multiplexer sequence                                                   */     // pointer to ending point of sequence
    uint8_t muxmeas_nr_end;                 /*!< number of multiplexer channels that have to be measured^                                       */     // end number of sequence, where measurement is finished
} LTC_STATE_s;

/*================== Function Prototypes ==================================*/

/**
 * @brief   trigger function for the LTC driver state machine.
 *
 * This function contains the sequence of events in the LTC state machine.
 * It must be called time-triggered, every 1ms.
 *
 * @return  void
 */
extern void LTC_Trigger(void);

/**
 * @brief   makes the requests to the LTC state machine.
 *
 * This function determines how the measurement cycle is made by the LTCs, by using the adequate state requests.
 *
 * @param   LTC_Todo       what kind of task the daisy-chain should do, taken from LTC_TASK_TYPE_e
 *
 * @return  result of the state request that was made, taken from LTC_RETURN_TYPE_e
 */
extern LTC_RETURN_TYPE_e LTC_Ctrl(LTC_TASK_TYPE_e LTC_Todo);

/**
 * @brief   sets the current state request of the state variable ltc_state.
 *
 * This function is used to make a state request to the state machine,e.g, start voltage measurement,
 * read result of voltage measurement, re-initialization
 * It calls LTC_CheckStateRequest() to check if the request is valid.
 * The state request is rejected if is not valid.
 * The result of the check is returned immediately, so that the requester can act in case
 * it made a non-valid state request.
 *
 * @param   statereq                state request to set
 * @param   adcModereq              LTC ADCmeasurement mode (fast, normal or filtered)
 * @param   adcMeasChreq            number of channels measured for GPIOS (one at a time for multiplexers or all five GPIOs)
 * @param   numberOfMeasuredMux     number of  multiplexer inputs measured between two cell voltage measurements
 *
 * @return  retVal                  current state request, taken from LTC_STATE_REQUEST_e
 */
extern LTC_RETURN_TYPE_e LTC_SetStateRequest(LTC_STATE_REQUEST_e statereq, LTC_ADCMODE_e adcModereq, LTC_ADCMEAS_CHAN_e adcMeasChreq, uint8_t numberOfMeasuredMux);

/*================== Function Implementations =============================*/

#endif /* LTC_H_ */
