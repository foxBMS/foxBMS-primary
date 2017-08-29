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
 * @file    ltc.c
 * @author  foxBMS Team
 * @date    01.09.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  LTC
 *
 * @brief   Driver for the LTC monitoring chip.
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
#include "database.h"

#include "ltc.h"
#include "mcu.h"
#include "diag.h"
#include "os.h"
#include "ltc_pec.h"

/*================== Macros and Definitions ===============================*/
/**
 * Saves the last state and the last substate
 */
#define LTC_SAVELASTSTATES()    ltc_state.laststate=ltc_state.state; \
                                ltc_state.lastsubstate = ltc_state.substate

/*================== Constant and Variable Definitions ====================*/
static uint32_t ltc_task_1ms_cnt = 0;
static uint8_t ltc_taskcycle = 0;
static STD_RETURN_TYPE_e ltc_muxcycle_finished = E_NOT_OK;

static DATA_BLOCK_CELLVOLTAGE_s ltc_cellvoltage;
static DATA_BLOCK_CELLTEMPERATURE_s ltc_celltemperature;
static DATA_BLOCK_MINMAX_s ltc_minmax;
static DATA_BLOCK_BALANCING_FEEDBACK_s ltc_balancing_feedback;
static DATA_BLOCK_BALANCING_CONTROL_s ltc_balancing_control;


static LTC_ERRORTABLE_s LTC_ErrorTable[BS_NR_OF_MODULES]; // init in LTC_ResetErrorTable-function


static LTC_STATE_s ltc_state = {
    .timer                  = 0,
    .statereq               = LTC_STATE_NO_REQUEST,
    .state                  = LTC_STATEMACH_UNINITIALIZED,
    .substate               = 0,
    .laststate              = LTC_STATEMACH_UNINITIALIZED,
    .lastsubstate           = 0,
    .adcModereq             = LTC_ADCMODE_NORMAL_DCP0,
    .adcMode                = LTC_ADCMODE_NORMAL_DCP0,
    .adcMeasChreq           = LTC_ADCMEAS_UNDEFINED,
    .adcMeasCh              = LTC_ADCMEAS_UNDEFINED,
    .numberOfMeasuredMux    = 32,
    .triggerentry           = 0,
    .ErrRetryCounter        = 0,
    .ErrRequestCounter      = 0,
    .VoltageSampleTime      = 0,
    .muxSampleTime          = 0,
    .commandDataTransferTime= 3,
    .commandTransferTime    = 3,
    .gpioClocksTransferTime = 3,
    .muxmeas_seqptr         = NULL_PTR,
    .muxmeas_seqendptr      = NULL_PTR,
    .muxmeas_nr_end         = 0,
};

static const uint8_t ltc_cmdDummy[1]={0x00};
static const uint8_t ltc_cmdRefOn[4]={0x00,0x01,0x3D,0x6E};

static const uint8_t ltc_cmdRDCVA[4] = {0x00,0x04,0x07,0xC2};
static const uint8_t ltc_cmdRDCVB[4] = {0x00,0x06,0x9A,0x94};
static const uint8_t ltc_cmdRDCVC[4] = {0x00,0x08,0x5E,0x52};
static const uint8_t ltc_cmdRDCVD[4] = {0x00,0x0A,0xC3,0x04};
static const uint8_t ltc_cmdWRCOMM[4] = {0x07,0x21,0x24,0xB2};
static const uint8_t ltc_cmdSTCOMM[4] = {0x07,0x23,0xB9,0xE4};
static const uint8_t ltc_cmdRDCOMM[4] = {0x07,0x22,0x32,0xD6};
static const uint8_t ltc_cmdRDAUXA[4] = {0x00,0x0C,0xEF,0xCC};
static const uint8_t ltc_cmdRDAUXB[4] = {0x00,0x0E,0x72,0x9A};

/* Cells */
static const uint8_t ltc_cmdADCV_normal_DCP0[4] = {0x03,0x60,0xF4,0x6C};   /*!< All cells, normal mode, discharge not permitted (DCP=0)    */
static const uint8_t ltc_cmdADCV_normal_DCP1[4] = {0x03,0x70,0xAF,0x42};   /*!< All cells, normal mode, discharge permitted (DCP=1)        */
static const uint8_t ltc_cmdADCV_filtered_DCP0[4] = {0x03,0xE0,0xB0,0x4A}; /*!< All cells, filtered mode, discharge not permitted (DCP=0)  */
static const uint8_t ltc_cmdADCV_filtered_DCP1[4] = {0x03,0xF0,0xEB,0x64}; /*!< All cells, filtered mode, discharge permitted (DCP=1)      */
static const uint8_t ltc_cmdADCV_fast_DCP0[4] = {0x02,0xE0,0x38,0x06};     /*!< All cells, fast mode, discharge not permitted (DCP=0)      */
static const uint8_t ltc_cmdADCV_fast_DCP1[4] = {0x02,0xF0,0x63,0x28};     /*!< All cells, fast mode, discharge permitted (DCP=1)          */

/* GPIOs  */
static const uint8_t ltc_cmdADAX_normal_GPIO1[4] = {0x05,0x61,0x58,0x92};      /*!< Single channel, GPIO 1, normal mode   */
static const uint8_t ltc_cmdADAX_filtered_GPIO1[4] = {0x05,0xE1,0x1C,0xB4};    /*!< Single channel, GPIO 1, filtered mode */
static const uint8_t ltc_cmdADAX_fast_GPIO1[4] = {0x04,0xE1,0x94,0xF8};        /*!< Single channel, GPIO 1, fast mode     */
static const uint8_t ltc_cmdADAX_normal_ALLGPIOS[4] = {0x05,0x60,0xD3,0xA0};   /*!< All channels, normal mode             */
static const uint8_t ltc_cmdADAX_filtered_ALLGPIOS[4] = {0x05,0xE0,0x97,0x86}; /*!< All channels, filtered mode           */
static const uint8_t ltc_cmdADAX_fast_ALLGPIOS[4] = {0x04,0xE0,0x1F,0xCA};     /*!< All channels, fast mode               */

static uint8_t ltc_tmpTXbuffer[6*LTC_N_LTC];

static uint8_t ltc_DataBufferSPI_TX_with_PEC_init[LTC_N_BYTES_FOR_DATA_TRANSMISSION];

static uint8_t ltc_tmpTXPECbuffer[LTC_N_BYTES_FOR_DATA_TRANSMISSION];
static uint8_t ltc_DataBufferSPI_RX_with_PEC_voltages[LTC_N_BYTES_FOR_DATA_TRANSMISSION];
static uint8_t LTC_CellVoltages[LTC_N_LTC*2*12];
static uint8_t LTC_MultiplexerVoltages[LTC_N_LTC*2*4*8];
static uint8_t LTC_GPIOVoltages[LTC_N_LTC*2*6];

static uint16_t LTC_allGPIOVoltages[LTC_N_LTC*6];

static uint8_t ltc_DataBufferSPI_TX_temperatures[6*LTC_N_LTC];

static uint8_t ltc_DataBufferSPI_TX_ClockCycles[4+9];
static uint8_t ltc_DataBufferSPI_TX_ClockCycles_with_PEC[4+9];

static uint8_t ltc_DataBufferSPI_TX_with_PEC_temperatures[LTC_N_BYTES_FOR_DATA_TRANSMISSION];
static uint8_t ltc_DataBufferSPI_RX_with_PEC_temperatures[LTC_N_BYTES_FOR_DATA_TRANSMISSION];

/*================== Function Prototypes ==================================*/
static float LTC_Convert_MuxVoltages_to_Temperatures(float Vout);

static void LTC_Initialize_Database(void);
static void LTC_SaveVoltages(void);
static void LTC_SaveTemperatures_SaveBalancingFeedback(void);
static void LTC_Get_BalancingControlValues(void);

static STD_RETURN_TYPE_e LTC_BalanceControl(void);

static void LTC_ResetErrorTable(void) ;
static STD_RETURN_TYPE_e LTC_Init(void);

static STD_RETURN_TYPE_e LTC_StartVoltageMeasurement(LTC_ADCMODE_e adcMode, LTC_ADCMEAS_CHAN_e  adcMeasCh);
static STD_RETURN_TYPE_e LTC_StartGPIOMeasurement(LTC_ADCMODE_e adcMode, LTC_ADCMEAS_CHAN_e  adcMeasCh);
static uint16_t LTC_Get_MeasurementTCycle(LTC_ADCMODE_e adcMode, LTC_ADCMEAS_CHAN_e  adcMeasCh);

static void LTC_SaveRXtoVoltagebuffer(uint8_t registerSet, uint8_t *rxBuffer);

static STD_RETURN_TYPE_e LTC_RX_PECCheck(uint8_t *DataBufferSPI_RX_with_PEC);
static STD_RETURN_TYPE_e LTC_RX(uint8_t *Command, uint8_t *DataBufferSPI_RX_with_PEC);
static STD_RETURN_TYPE_e LTC_TX(uint8_t *Command, uint8_t *DataBufferSPI_TX, uint8_t *DataBufferSPI_TX_with_PEC);
static void LTC_SetMUXChCommand(uint8_t *DataBufferSPI_TX, uint8_t mux, uint8_t channel);
static uint8_t LTC_SetMuxChannel(uint8_t *DataBufferSPI_TX, uint8_t *DataBufferSPI_TX_with_PEC, uint8_t mux, uint8_t channel);

static STD_RETURN_TYPE_e LTC_I2CClock(uint8_t *DataBufferSPI_TX, uint8_t *DataBufferSPI_TX_with_PEC);

static uint8_t LTC_I2CCheckACK(uint8_t *DataBufferSPI_RX,int mux);

static void LTC_SaveMuxMeasurement(uint8_t *DataBufferSPI_RX, LTC_MUX_CH_CFG_s  *muxseqptr);
static void LTC_SaveGPIOMeasurement(uint8_t registerSet, uint8_t *rxBuffer);
static void LTC_SaveAllGPIOs(void);

static uint32_t LTC_GetSPIClock(void);
static void LTC_SetTransferTimes(void);

static LTC_RETURN_TYPE_e LTC_CheckStateRequest(LTC_STATE_REQUEST_e statereq);
static LTC_STATE_REQUEST_e LTC_GetStateRequest(void);
static LTC_STATEMACH_e LTC_GetState(void);

/*================== Function Implementations =============================*/

/*================== Public functions =====================================*/

/*================== Static functions =====================================*/
/**
 * @brief   in the database, initializes the fields related to the LTC drivers.
 *
 * This function loops through all the LTC-related data fields in the database
 * and sets them to 0. It should be called in the initialization or re-initialization
 * routine of the LTC driver.
 *
 * @return   void
 */
static void LTC_Initialize_Database(void) {

    uint16_t i = 0;


    ltc_cellvoltage.state = 0;
    ltc_cellvoltage.timestamp = 0;
    ltc_minmax.voltage_min = 0;
    ltc_minmax.voltage_max = 0;
    ltc_minmax.voltage_module_number_min = 0;
    ltc_minmax.voltage_module_number_max = 0;
    ltc_minmax.voltage_cell_number_min = 0;
    ltc_minmax.voltage_cell_number_max = 0;
    for (i=0; i<BS_NR_OF_BAT_CELLS;i++) {
        ltc_cellvoltage.voltage[i] = 0;
    }

    ltc_celltemperature.state = 0;
    ltc_celltemperature.timestamp = 0;
    ltc_minmax.temperature_min = 0;
    ltc_minmax.temperature_max = 0;
    ltc_minmax.temperature_module_number_min = 0;
    ltc_minmax.temperature_module_number_max = 0;
    ltc_minmax.temperature_sensor_number_min = 0;
    ltc_minmax.temperature_sensor_number_max = 0;
    for (i=0; i<BS_NR_OF_TEMP_SENSORS;i++) {
        ltc_celltemperature.temperature[i] = 0;
    }

    ltc_balancing_feedback.state = 0;
    ltc_balancing_feedback.timestamp = 0;
    ltc_balancing_control.state = 0;
    ltc_balancing_control.timestamp = 0;
    for (i=0; i<BS_NR_OF_BAT_CELLS;i++) {
        ltc_balancing_feedback.value[i] = 0;
        ltc_balancing_control.value[i] = 0;
    }

    DATA_StoreDataBlock(&ltc_cellvoltage, DATA_BLOCK_ID_CELLVOLTAGE);
    DATA_StoreDataBlock(&ltc_celltemperature, DATA_BLOCK_ID_CELLTEMPERATURE);
    DATA_StoreDataBlock(&ltc_minmax, DATA_BLOCK_ID_MINMAX);
    DATA_StoreDataBlock(&ltc_balancing_feedback, DATA_BLOCK_ID_BALANCING_FEEDBACK_VALUES);
    DATA_StoreDataBlock(&ltc_balancing_control, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);

}

/**
 * @brief   stores the measured voltages of the 5 GPIOs of each LTC in daisy chain.
 *
 * Voltages currently stored in LTC_allGPIOVoltages[], but not in database yet.
 * This has to be added by user in case the use of these voltages is needed.
 *
 * The multiplication is used to get the result in mV . For details on conversion see LTC data sheet.
 *
 * @return  void
 *
 */
static void LTC_SaveAllGPIOs(void) {
    for (uint16_t i=0; i<6*LTC_N_LTC;i++) {
        LTC_allGPIOVoltages[i] = (uint16_t)(((float)(*((uint16_t *)(&LTC_GPIOVoltages[2*i]))))*100e-6*1000.0);
    }
}

/**
 * @brief   stores the measured voltages in the database.
 *
 * This function loops through the data of all modules in the LTC daisy-chain that are
 * stored in the LTC_CellVoltages buffer and writes them in the database.
 * At each write iteration, the variable named "state" and related to voltages in the
 * database is incremented.
 *
 * @return  void
 *
 */
static void LTC_SaveVoltages(void) {

    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t val_ui = 0;
    float  val_fl = 0.0;
    uint16_t min = 0;
    uint16_t max = 0;
    uint32_t mean = 0;
    uint8_t module_number_min = 0;
    uint8_t module_number_max = 0;
    uint8_t cell_number_min = 0;
    uint8_t cell_number_max = 0;

    for (i=0;i<BS_NR_OF_MODULES;i++) {
        for (j=0;j<BS_NR_OF_BAT_CELLS_PER_MODULE;j++) {

            val_ui = *((uint16_t *)(&LTC_CellVoltages[2*j+i*LTC_NUMBER_OF_LTC_PER_MODULE*24]));        // raw values
            val_fl = ((float)(val_ui))*100e-6*1000.0;        // Unit V -> in mV
            ltc_cellvoltage.voltage[i*(BS_NR_OF_BAT_CELLS_PER_MODULE)+j]=(uint16_t)(val_fl);
        }
    }

    max = min = ltc_cellvoltage.voltage[0];
    mean = 0;
    for (i=0;i<BS_NR_OF_MODULES;i++) {
        for (j=0;j<BS_NR_OF_BAT_CELLS_PER_MODULE;j++) {
            mean += ltc_cellvoltage.voltage[i*(BS_NR_OF_BAT_CELLS_PER_MODULE)+j];
            if (ltc_cellvoltage.voltage[i*(BS_NR_OF_BAT_CELLS_PER_MODULE)+j] < min) {
                min = ltc_cellvoltage.voltage[i*(BS_NR_OF_BAT_CELLS_PER_MODULE)+j];
                module_number_min = i;
                cell_number_min = j;
            }
            if (ltc_cellvoltage.voltage[i*(BS_NR_OF_BAT_CELLS_PER_MODULE)+j] > max) {
                max = ltc_cellvoltage.voltage[i*(BS_NR_OF_BAT_CELLS_PER_MODULE)+j];
                module_number_max = i;
                cell_number_max = j;
            }
        }
    }
    mean /= (BS_NR_OF_BAT_CELLS);

    DATA_GetTable(&ltc_minmax, DATA_BLOCK_ID_MINMAX);
    ltc_cellvoltage.state++;
    ltc_minmax.state++;
    ltc_minmax.voltage_mean = mean;
    ltc_minmax.previous_voltage_min = ltc_minmax.voltage_min;
    ltc_minmax.voltage_min = min;
    ltc_minmax.voltage_module_number_min = module_number_min;
    ltc_minmax.voltage_cell_number_min = cell_number_min;
    ltc_minmax.previous_voltage_max = ltc_minmax.voltage_max;
    ltc_minmax.voltage_max = max;
    ltc_minmax.voltage_module_number_max = module_number_max;
    ltc_minmax.voltage_cell_number_max = cell_number_max;
    DATA_StoreDataBlock(&ltc_cellvoltage,DATA_BLOCK_ID_CELLVOLTAGE);
    DATA_StoreDataBlock(&ltc_minmax,DATA_BLOCK_ID_MINMAX);

}

/**
 * @brief   stores the measured temperatures and the measured multiplexer feedbacks in the database.
 *
 * This function loops through the temperature and multiplexer feedback data of all modules
 * in the LTC daisy-chain that are stored in the LTC_MultiplexerVoltages buffer and writes
 * them in the database.
 * At each write iteration, the variables named "state" and related to temperatures and multiplexer feedbacks
 * in the database are incremented.
 *
 * @return  void
 *
 */
static void LTC_SaveTemperatures_SaveBalancingFeedback(void) {

    uint16_t i = 0;
    uint16_t j = 0;
    uint8_t sensor_idx = 0;
    uint8_t ch_idx = 0;
    uint16_t val_ui = 0;
    int16_t val_si = 0;
    float  val_fl = 0.0;

    int16_t min = 0;
    uint16_t max = 0;
    int32_t mean = 0;
    uint8_t module_number_min = 0;
    uint8_t module_number_max = 0;
    uint8_t sensor_number_min = 0;
    uint8_t sensor_number_max = 0;

    LTC_MUX_CH_CFG_s *muxseqptr;    // pointer to measurement Sequence of Mux- and Channel-Configurations (1,-1)...(3,-1),(0,1),...(0,7)
    LTC_MUX_CH_CFG_s *muxseqendptr; // pointer to ending point of sequence

    muxseqptr = ltc_mux_seq.seqptr;
    muxseqendptr = ((LTC_MUX_CH_CFG_s *)ltc_mux_seq.seqptr)+ltc_mux_seq.nr_of_steps; // last sequence + 1

    while(muxseqptr < muxseqendptr) {

        // last step of sequence not reached
        if (muxseqptr->muxCh != 0xff) {

            if (muxseqptr->muxID <= 1) {    // muxID 0 or 1, // typically temperature multiplexer type
                for (i=0;i<BS_NR_OF_MODULES;i++)
                {
                    val_ui=*((uint16_t *)(&LTC_MultiplexerVoltages[2*((LTC_NUMBER_OF_LTC_PER_MODULE*i*LTC_N_MUX_CHANNELS_PER_LTC)+muxseqptr->muxID*LTC_N_MUX_CHANNELS_PER_MUX+muxseqptr->muxCh)]));        // raw values, all mux on all LTCs
                    val_fl = ((float)(val_ui))*100e-6;        // Unit -> in V
                    //if multiplexer 1 is used, the indices must be adapted
                    if (muxseqptr->muxID==0) {
                        sensor_idx = ltc_muxsensortemperatur_cfg[muxseqptr->muxCh];
                    }
                    else {
                        sensor_idx = ltc_muxsensortemperatur_cfg[muxseqptr->muxCh+8];
                    }
                    if (sensor_idx >= BS_NR_OF_TEMP_SENSORS_PER_MODULE)
                        return;

                    val_si = (int16_t)(LTC_Convert_MuxVoltages_to_Temperatures(val_fl));
                    ltc_celltemperature.temperature[i*(BS_NR_OF_TEMP_SENSORS_PER_MODULE)+sensor_idx]=val_si;

                }
            }

            if (muxseqptr->muxID >= 2 && muxseqptr->muxID <= 3) // muxID 2 or 3
            {    // typically balancing multiplexer type
                for (i=0;i<BS_NR_OF_MODULES;i++) {
                    if(muxseqptr->muxID == 2)
                        ch_idx = 0 + muxseqptr->muxCh;    // channel index 0..7
                    else
                        ch_idx = 8 + muxseqptr->muxCh;    // channel index 8..15

                    if (ch_idx < BS_NR_OF_BAT_CELLS_PER_MODULE) {
                        val_ui=*((uint16_t *)(&LTC_MultiplexerVoltages[2*(LTC_NUMBER_OF_LTC_PER_MODULE*i*LTC_N_MUX_CHANNELS_PER_LTC+muxseqptr->muxID*LTC_N_MUX_CHANNELS_PER_MUX+muxseqptr->muxCh)]));        // raw values, all mux on all LTCs
                        val_fl = ((float)(val_ui))*100e-6*1000.0;        // Unit -> in V -> in mV
                        ltc_balancing_feedback.value[i*(BS_NR_OF_BAT_CELLS_PER_MODULE)+ch_idx] = (uint16_t)(val_fl);
                    }
                }

            }

        }// end if! 0xff

        ++muxseqptr;

    }// end while

    mean = 0;
    max = min = ltc_celltemperature.temperature[0];
    for (i=0;i<BS_NR_OF_MODULES;i++) {
        for (j=0;j<BS_NR_OF_TEMP_SENSORS_PER_MODULE;j++) {
            mean += ltc_celltemperature.temperature[i*(BS_NR_OF_TEMP_SENSORS_PER_MODULE)+j];
            if (ltc_celltemperature.temperature[i*(BS_NR_OF_TEMP_SENSORS_PER_MODULE)+j] < min) {
                min = ltc_celltemperature.temperature[i*(BS_NR_OF_TEMP_SENSORS_PER_MODULE)+j];
                module_number_min = i;
                sensor_number_min = j;
            }
            if (ltc_celltemperature.temperature[i*(BS_NR_OF_TEMP_SENSORS_PER_MODULE)+j] > max) {
                max = ltc_celltemperature.temperature[i*(BS_NR_OF_TEMP_SENSORS_PER_MODULE)+j];
                module_number_max = i;
                sensor_number_max = j;
            }
        }
    }
    mean /= (BS_NR_OF_TEMP_SENSORS);

    DATA_GetTable(&ltc_minmax, DATA_BLOCK_ID_MINMAX);
    ltc_celltemperature.previous_timestamp = ltc_celltemperature.timestamp;
    ltc_celltemperature.timestamp = MCU_GetTimeStamp();
    ltc_celltemperature.state++;
    ltc_minmax.state++;
    ltc_minmax.temperature_mean = mean;
    ltc_minmax.temperature_min = min;
    ltc_minmax.temperature_module_number_min = module_number_min;
    ltc_minmax.temperature_sensor_number_min = sensor_number_min;
    ltc_minmax.temperature_max = max;
    ltc_minmax.temperature_module_number_max = module_number_max;
    ltc_minmax.temperature_sensor_number_max = sensor_number_max;
    DATA_StoreDataBlock(&ltc_celltemperature,DATA_BLOCK_ID_CELLTEMPERATURE);
    DATA_StoreDataBlock(&ltc_minmax,DATA_BLOCK_ID_MINMAX);

    ltc_balancing_feedback.previous_timestamp = ltc_balancing_feedback.timestamp;
    ltc_balancing_feedback.timestamp = MCU_GetTimeStamp();
    ltc_balancing_feedback.state++;
    DATA_StoreDataBlock(&ltc_balancing_feedback,DATA_BLOCK_ID_BALANCING_FEEDBACK_VALUES);

}


/**
 * @brief   gets the balancing orders from the database.
 *
 * This function gets the balancing control from the database. Balancing control
 * is set by the BMS. The LTC driver only executes the balancing orders.
 *
 * @return  void
 */
static void LTC_Get_BalancingControlValues(void) {
    DATA_GetTable(&ltc_balancing_control, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
}


/**
 * @brief   converts a raw voltage from multiplexer to a temperature value in Celsius.
 *
 * The temperatures are read from NTC elements via voltage dividers.
 * This function implements the look-up table between voltage and temperature,
 * taking into account the NTC characteristics and the voltage divider.
 *
 * @param   Vout            voltage read from the multiplexer in V
 *
 * @return  temperature     temperature value in Celsius
 */
static float LTC_Convert_MuxVoltages_to_Temperatures(float Vout) {

    float temperature = 0.0;

    //Dummy function, must be adapted to the application
    temperature = 10 * Vout;

    return temperature;
}

/**
 * @brief   re-entrance check of LTC state machine trigger function
 *
 * This function is not re-entrant and should only be called time- or event-triggered.
 * It increments the triggerentry counter from the state variable ltc_state.
 * It should never be called by two different processes, so if it is the case, triggerentry
 * should never be higher than 0 when this function is called.
 *
 *
 * @return  retval  0 if no further instance of the function is active, 0xff else
 *
 */
uint8_t LTC_CheckReEntrance(void)
{
    uint8_t retval=0;

    taskENTER_CRITICAL();
    if(!ltc_state.triggerentry)
    {
        ltc_state.triggerentry++;
    }
    else
        retval = 0xFF;    // multiple calls of function
    taskEXIT_CRITICAL();

    return (retval);
}

/**
 * @brief   gets the current state request.
 *
 * This function is used in the functioning of the LTC state machine.
 *
 * @return  retval  current state request, taken from LTC_STATE_REQUEST_e
 */
static LTC_STATE_REQUEST_e LTC_GetStateRequest(void) {

    LTC_STATE_REQUEST_e retval = LTC_STATE_NO_REQUEST;

    taskENTER_CRITICAL();
    retval    = ltc_state.statereq;
    taskEXIT_CRITICAL();

    return (retval);
}

/**
 * @brief   gets the current state.
 *
 * This function is used in the functioning of the LTC state machine.
 *
 * @return  current state, taken from LTC_STATEMACH_e
 */
static LTC_STATEMACH_e LTC_GetState(void) {
    return (ltc_state.state);
}

/**
 * @brief   transfers the current state request to the state machine.
 *
 * This function takes the current state request from ltc_state and transfers it to the state machine.
 * It resets the value from ltc_state to LTC_STATE_NO_REQUEST
 *
 * @param   *busIDptr       bus ID, main or backup (deprecated)
 * @param   *adcModeptr     LTC ADCmeasurement mode (fast, normal or filtered)
 * @param   *adcMeasChptr   number of channels measured for GPIOS (one at a time for multiplexers or all five GPIOs)
 *
 * @return  retVal          current state request, taken from LTC_STATE_REQUEST_e
 *
 */
LTC_STATE_REQUEST_e LTC_TransferStateRequest(uint8_t *busIDptr,LTC_ADCMODE_e *adcModeptr,LTC_ADCMEAS_CHAN_e *adcMeasChptr)
{
    LTC_STATE_REQUEST_e retval = LTC_STATE_NO_REQUEST;

    taskENTER_CRITICAL();
    retval    = ltc_state.statereq;
    *adcModeptr = ltc_state.adcModereq;
    *adcMeasChptr = ltc_state.adcMeasChreq;
    ltc_state.statereq = LTC_STATE_NO_REQUEST;
    taskEXIT_CRITICAL();

    return (retval);
}

LTC_RETURN_TYPE_e LTC_SetStateRequest(LTC_STATE_REQUEST_e statereq, LTC_ADCMODE_e adcModereq, LTC_ADCMEAS_CHAN_e adcMeasChreq, uint8_t numberOfMeasuredMux)
{
    LTC_RETURN_TYPE_e retVal = LTC_STATE_NO_REQUEST;

    taskENTER_CRITICAL();
    retVal=LTC_CheckStateRequest(statereq);

    if (retVal==LTC_OK || retVal==LTC_BUSY_OK || retVal==LTC_OK_FROM_ERROR)
        {
            ltc_state.statereq   = statereq;
            ltc_state.adcModereq = adcModereq;            // measurement mode
            ltc_state.adcMeasChreq = adcMeasChreq;        // measurement channel
            ltc_state.numberOfMeasuredMux = numberOfMeasuredMux;        // measurement channel
        }
    taskEXIT_CRITICAL();

    return (retVal);
}

void LTC_Trigger(void)
{
    uint8_t mux_error=0;

    STD_RETURN_TYPE_e retVal=E_OK;
    LTC_STATE_REQUEST_e statereq=LTC_STATE_NO_REQUEST;
    uint8_t tmpbusID=0;
    LTC_ADCMODE_e tmpadcMode=LTC_ADCMODE_UNDEFINED;
    LTC_ADCMEAS_CHAN_e tmpadcMeasCh=LTC_ADCMEAS_UNDEFINED;

    // Check re-entrance of function
    if (LTC_CheckReEntrance())
        return;

    DIAG_SysMonNotify(DIAG_SYSMON_LTC_ID,0);        // task is running, state = ok

    if(ltc_state.timer)
    {
        if(--ltc_state.timer)
        {
            ltc_state.triggerentry--;
            return;    // handle state machine only if timer has elapsed
        }
    }


    switch(ltc_state.state) {
        /****************************UNINITIALIZED***********************************/
        case LTC_STATEMACH_UNINITIALIZED:
            // waiting for Initialization Request
            statereq = LTC_TransferStateRequest(&tmpbusID, &tmpadcMode, &tmpadcMeasCh);
            if(statereq == LTC_STATE_INIT_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_INITIALIZATION;
                ltc_state.substate = LTC_ENTRY_UNINITIALIZED;
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = tmpadcMode;
                ltc_state.adcMeasCh = tmpadcMeasCh;
            }
            else if(statereq == LTC_STATE_NO_REQUEST)
            {
                // no actual request pending //
            }
            else
            {
                ltc_state.ErrRequestCounter++;   // illegal request pending
            }
            break;

        /****************************INITIALIZATION**********************************/
        case LTC_STATEMACH_INITIALIZATION:

            LTC_SetTransferTimes();
            ltc_state.muxmeas_seqptr = ltc_mux_seq.seqptr;
            ltc_state.muxmeas_nr_end = ltc_mux_seq.nr_of_steps;
            ltc_state.muxmeas_seqendptr = ((LTC_MUX_CH_CFG_s *)ltc_mux_seq.seqptr)+ltc_mux_seq.nr_of_steps; // last sequence + 1

            if(ltc_state.substate == LTC_ENTRY_INITIALIZATION)
            {
                LTC_SAVELASTSTATES();
                LTC_Initialize_Database();
                LTC_ResetErrorTable();
                retVal = LTC_SendWakeUp();        // Send dummy byte to wake up the daisy chain

                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer    = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.ErrRetryCounter=0;
                    // LTC6804 datasheet page 41
                    // If LTC stack too long: a second wake up must be done
                    if (LTC_STATEMACH_DAISY_CHAIN_FIRST_INITIALIZATION_TIME>(LTC_TIDLE_US*1000))
                    {
                        ltc_state.substate=LTC_RE_ENTRY_INITIALIZATION;
                    }
                    else
                    {
                        // LTC stack not too long: second wake up not necessary
                        ltc_state.substate=LTC_START_INIT_INITIALIZATION;
                    }
                    ltc_state.timer = LTC_STATEMACH_DAISY_CHAIN_FIRST_INITIALIZATION_TIME;
                }
            }
            else if(ltc_state.substate == LTC_RE_ENTRY_INITIALIZATION)
            {
                LTC_SAVELASTSTATES();
                retVal = LTC_SendWakeUp();  // Send dummy byte again to wake up the daisy chain

                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.substate = LTC_START_INIT_INITIALIZATION;
                    ltc_state.timer = LTC_STATEMACH_DAISY_CHAIN_SECOND_INITIALIZATION_TIME;
                }
            }
            else if (ltc_state.substate == LTC_START_INIT_INITIALIZATION)
            {
                retVal=LTC_Init();  // Initialize main LTC loop
                ltc_state.lastsubstate = ltc_state.substate;
                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;        // wait
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.ErrRetryCounter=0;
                    ltc_state.substate=LTC_EXIT_INITIALIZATION;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                }
            }
            else if(ltc_state.substate == LTC_EXIT_INITIALIZATION)
            {   // in daisy-chain mode, there is no confirmation of the initialization
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_INITIALIZED;
                ltc_state.substate = LTC_ENTRY_INITIALIZATION;
            }
            break;

        /****************************INITIALIZED*************************************/
        case LTC_STATEMACH_INITIALIZED:
            LTC_IF_INITIALIZED_CALLBACK();
            LTC_SAVELASTSTATES();
            ltc_state.timer = LTC_STATEMACH_SHORTTIME;
            ltc_state.state = LTC_STATEMACH_IDLE;
            ltc_state.substate = LTC_ENTRY_INITIALIZED;
            ltc_state.ErrRetryCounter = 0;
            break;

        /****************************IDLE********************************************/
        case LTC_STATEMACH_IDLE:
            statereq=LTC_TransferStateRequest(&tmpbusID,&tmpadcMode,&tmpadcMeasCh);
            if(statereq == LTC_STATE_VOLTAGEMEASUREMENT_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_STARTMEAS;
                ltc_state.substate = LTC_ENTRY;
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = tmpadcMode;
                ltc_state.adcMeasCh = LTC_ADCMEAS_ALLCHANNEL;
            }
            else if(statereq == LTC_STATE_READVOLTAGE_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_READVOLTAGE;
                ltc_state.substate = LTC_READ_VOLTAGE_REGISTER_A_RDCVA_READVOLTAGE;
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = tmpadcMode;
                ltc_state.adcMeasCh = LTC_ADCMEAS_ALLCHANNEL;
            }
            else if(statereq == LTC_STATE_MUXMEASUREMENT_REQUEST)
            {

                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_STARTMUXMEASUREMENT;
                ltc_state.substate = LTC_SET_MUX_CHANNEL_WRCOMM_MUXMEASUREMENT_CONFIG;
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = tmpadcMode;
                ltc_state.adcMeasCh = LTC_ADCMEAS_SINGLECHANNEL;
            }
            else if(statereq == LTC_STATE_ALLGPIOMEASUREMENT_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_ALLGPIOMEASUREMENT;
                ltc_state.substate = LTC_READ_AUXILIARY_REGISTER_A_RAUXA_READALLGPIO;
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = tmpadcMode;
                ltc_state.adcMeasCh = LTC_ADCMEAS_ALLCHANNEL;
            }
            else if(statereq == LTC_STATE_BALANCECONTROL_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_BALANCECONTROL;
                ltc_state.substate = LTC_ENTRY_BALANCECONTROL;
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = LTC_ADCMODE_UNDEFINED;   // not needed for this state
                ltc_state.adcMeasCh = LTC_ADCMEAS_UNDEFINED; // not needed for this state
            }
            else if(statereq == LTC_STATE_REINIT_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_INITIALIZATION;
                ltc_state.substate = LTC_ENTRY_INITIALIZATION;
                LTC_ResetErrorTable();
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = LTC_ADCMODE_UNDEFINED;   // not needed for this state
                ltc_state.adcMeasCh = LTC_ADCMEAS_UNDEFINED; // not needed for this state
            }
            else if(statereq == LTC_STATE_IDLE_REQUEST)
            {
                // we stay already in requested state, nothing to do
            }
            else if(statereq == LTC_STATE_NO_REQUEST)
            {
                // no actual request pending
            }
            else
            {
                ltc_state.ErrRequestCounter++;   // illegal request pending
            }

            break;

        /****************************START MEASUREMENT*******************************/
        case LTC_STATEMACH_STARTMEAS:

            retVal=LTC_StartVoltageMeasurement(ltc_state.adcMode, ltc_state.adcMeasCh);
            ltc_cellvoltage.previous_timestamp = ltc_cellvoltage.timestamp;
            ltc_cellvoltage.timestamp = MCU_GetTimeStamp();

            LTC_SAVELASTSTATES();
            if((retVal != E_OK))
            {
                ++ltc_state.ErrRetryCounter;
                ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;        // wait
                if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                {
                    ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                    ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                    ltc_state.substate = LTC_ERROR_ENTRY;
                    break;
                }
            }
            else
            {
                ltc_state.ErrRetryCounter = 0;
                ltc_state.timer = ltc_state.commandTransferTime + LTC_Get_MeasurementTCycle(ltc_state.adcMode,ltc_state.adcMeasCh);
                ltc_state.substate = LTC_ENTRY;
                ltc_state.state = LTC_STATEMACH_IDLE;
                ltc_state.ErrPECCounter = 0;
            }

            break;

        /****************************READ VOLTAGE************************************/
        case LTC_STATEMACH_READVOLTAGE:
            if(ltc_state.substate == LTC_READ_VOLTAGE_REGISTER_A_RDCVA_READVOLTAGE)
            {
                LTC_SAVELASTSTATES();
                retVal = LTC_RX((uint8_t*)(ltc_cmdRDCVA), ltc_DataBufferSPI_RX_with_PEC_voltages);
                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.substate = LTC_READ_VOLTAGE_REGISTER_B_RDCVB_READVOLTAGE;
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                }
            }
            else if (ltc_state.substate == LTC_READ_VOLTAGE_REGISTER_B_RDCVB_READVOLTAGE)
            {
                if(ltc_state.lastsubstate == LTC_READ_VOLTAGE_REGISTER_A_RDCVA_READVOLTAGE)
                {
                    if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_voltages) != E_OK)
                    {
                        if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                        {
                            if (LTC_DISCARD_PEC_CHECK == FALSE)
                            {
                                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                                ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                                ltc_state.substate = LTC_ERROR_ENTRY;
                                break;
                            }
                        }
                        else
                        {
                            ltc_state.lastsubstate = ltc_state.substate;
                            ltc_state.substate=LTC_READ_VOLTAGE_REGISTER_A_RDCVA_READVOLTAGE;
                            ltc_state.timer = LTC_STATEMACH_PECERRTIME;
                            break;
                        }
                    }
                    LTC_ResetErrorTable();
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.ErrPECCounter = 0;
                    LTC_SaveRXtoVoltagebuffer(0, ltc_DataBufferSPI_RX_with_PEC_voltages);
                }

                ltc_state.lastsubstate = ltc_state.substate;
                retVal = LTC_RX( (uint8_t*)ltc_cmdRDCVB, ltc_DataBufferSPI_RX_with_PEC_voltages);
                if(retVal != E_OK)
                {
                    ++(ltc_state.ErrRetryCounter);
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.substate = LTC_READ_VOLTAGE_REGISTER_C_RDCVC_READVOLTAGE;
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                }
            }
            else if (ltc_state.substate == LTC_READ_VOLTAGE_REGISTER_C_RDCVC_READVOLTAGE)
            {
                if(ltc_state.lastsubstate == LTC_READ_VOLTAGE_REGISTER_B_RDCVB_READVOLTAGE)
                {
                    if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_voltages)!=E_OK)
                    {
                        if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                        {
                            if (LTC_DISCARD_PEC_CHECK == FALSE)
                            {
                                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                                ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                                ltc_state.substate = LTC_ERROR_ENTRY;
                                break;
                            }
                        }
                        else
                        {
                            ltc_state.lastsubstate = ltc_state.substate;
                            ltc_state.substate = LTC_READ_VOLTAGE_REGISTER_B_RDCVB_READVOLTAGE;
                            ltc_state.timer = LTC_STATEMACH_PECERRTIME;
                            break;
                        }
                    }
                    LTC_ResetErrorTable();
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.ErrPECCounter = 0;
                    LTC_SaveRXtoVoltagebuffer(1, ltc_DataBufferSPI_RX_with_PEC_voltages);
                }

                ltc_state.lastsubstate = ltc_state.substate;
                retVal = LTC_RX( (uint8_t*)ltc_cmdRDCVC, ltc_DataBufferSPI_RX_with_PEC_voltages);
                if(retVal != E_OK)
                {
                    ++(ltc_state.ErrRetryCounter);
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.substate = LTC_READ_VOLTAGE_REGISTER_D_RDCVD_READVOLTAGE;
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                }
            }
            else if (ltc_state.substate == LTC_READ_VOLTAGE_REGISTER_D_RDCVD_READVOLTAGE)
            {
                if(ltc_state.lastsubstate == LTC_READ_VOLTAGE_REGISTER_C_RDCVC_READVOLTAGE)
                {
                    if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_voltages)!=E_OK)
                    {
                        if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                        {
                            if (LTC_DISCARD_PEC_CHECK == FALSE)
                            {
                                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                                ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                                ltc_state.substate = LTC_ERROR_ENTRY;
                                break;
                            }
                        }
                        else
                        {
                            ltc_state.lastsubstate = ltc_state.substate;
                            ltc_state.substate = LTC_READ_VOLTAGE_REGISTER_C_RDCVC_READVOLTAGE;
                            ltc_state.timer = LTC_STATEMACH_PECERRTIME;
                            break;
                        }
                    }
                    LTC_ResetErrorTable();
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.ErrPECCounter = 0;
                    LTC_SaveRXtoVoltagebuffer(2, ltc_DataBufferSPI_RX_with_PEC_voltages);
                }

                ltc_state.lastsubstate = ltc_state.substate;
                retVal = LTC_RX( (uint8_t*)ltc_cmdRDCVD, ltc_DataBufferSPI_RX_with_PEC_voltages);
                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.substate=LTC_EXIT_READVOLTAGE;
                    ltc_state.ErrRetryCounter=0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                }
            }
            else if (ltc_state.substate == LTC_EXIT_READVOLTAGE)
            {
                if(ltc_state.lastsubstate == LTC_READ_VOLTAGE_REGISTER_D_RDCVD_READVOLTAGE)
                {
                    if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_voltages)!=E_OK)
                    {
                        if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                        {
                            if (LTC_DISCARD_PEC_CHECK == FALSE)
                            {
                                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                                ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                                ltc_state.substate = LTC_ERROR_ENTRY;
                                break;
                            }
                        }
                        else
                        {
                            ltc_state.lastsubstate = ltc_state.substate;
                            ltc_state.substate=LTC_READ_VOLTAGE_REGISTER_D_RDCVD_READVOLTAGE;
                            ltc_state.timer = LTC_STATEMACH_PECERRTIME;
                            break;
                        }
                    }
                    LTC_ResetErrorTable();
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.ErrPECCounter = 0;
                    LTC_SaveRXtoVoltagebuffer(3, ltc_DataBufferSPI_RX_with_PEC_voltages);
                }

                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                LTC_SAVELASTSTATES();
                ltc_state.substate = LTC_ENTRY;
                ltc_state.state = LTC_STATEMACH_IDLE;
            }
            break;

        /****************************BALANCE CONTROL*********************************/
        case LTC_STATEMACH_BALANCECONTROL:
            if(ltc_state.substate == LTC_ENTRY_BALANCECONTROL)
            {
                retVal = LTC_BalanceControl();
                ltc_state.lastsubstate = ltc_state.substate;
                ltc_state.laststate = ltc_state.state;
                if(retVal != 0)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }

                }
                else
                {
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.substate = LTC_EXIT_BALANCECONTROL;
                }
            }
            else if(ltc_state.substate == LTC_EXIT_BALANCECONTROL)
            {
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;        // wait 1ms
                ltc_state.ErrRetryCounter = 0;
                LTC_SAVELASTSTATES();
                ltc_state.substate = LTC_ENTRY;
                ltc_state.state = LTC_STATEMACH_IDLE;
            }
            break;

        /****************************START MULTIPLEXED MEASUREMENT*******************/
        case LTC_STATEMACH_STARTMUXMEASUREMENT:

            //first mux sequence pointer initialization was made during the initialization state
            ltc_state.timer = LTC_STATEMACH_SHORTTIME;
            LTC_SAVELASTSTATES();
            ltc_state.substate = LTC_SET_MUX_CHANNEL_WRCOMM_MUXMEASUREMENT_CONFIG;
            ltc_state.state = LTC_STATEMACH_MUXMEASUREMENT_CONFIG;
            ltc_state.ErrRetryCounter = 0;

            break;

        /****************************MULTIPLEXED MEASUREMENT CONFIGURATION***********/
        case LTC_STATEMACH_MUXMEASUREMENT_CONFIG:
            if(ltc_state.muxmeas_seqptr >= ltc_state.muxmeas_seqendptr)
            {    // last step of sequence reached (or no sequence configured)
                //mux sequence re-initialized (if necessary) in sub-state LTC_SAVE_MUX_MEASUREMENT_MUXMEASUREMENT
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                LTC_SAVELASTSTATES();
                ltc_state.substate = LTC_ENTRY;
                ltc_state.state = LTC_STATEMACH_MUXMEASUREMENT_FINISHED;
                ltc_state.ErrRetryCounter = 0;

                break;
            }

            if(ltc_state.substate == LTC_SET_MUX_CHANNEL_WRCOMM_MUXMEASUREMENT_CONFIG)
            {
                retVal = LTC_SetMuxChannel( ltc_DataBufferSPI_TX_temperatures, ltc_DataBufferSPI_TX_with_PEC_temperatures,
                                            ltc_state.muxmeas_seqptr->muxID,  /* mux */
                                            ltc_state.muxmeas_seqptr->muxCh  /* channel */ );
                ltc_state.lastsubstate = ltc_state.substate;
                ltc_state.laststate = ltc_state.state;
                if(retVal != 0)
                {
                    ++(ltc_state.ErrRetryCounter);
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.substate = LTC_SEND_CLOCK_STCOMM_MUXMEASUREMENT_CONFIG;
                }
            }
            else if(ltc_state.substate == LTC_SEND_CLOCK_STCOMM_MUXMEASUREMENT_CONFIG)
            {
                ltc_state.lastsubstate = ltc_state.substate;
                retVal = LTC_I2CClock(ltc_DataBufferSPI_TX_ClockCycles, ltc_DataBufferSPI_TX_ClockCycles_with_PEC);
                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.ErrRetryCounter=0;
                    ltc_state.timer = ltc_state.gpioClocksTransferTime;
                    if(LTC_READCOM == 1)
                        ltc_state.substate = LTC_READ_I2C_TRANSMISSION_RESULT_RDCOMM_MUXMEASUREMENT_CONFIG;
                    else
                        ltc_state.substate = LTC_START_GPIO_MEASUREMENT_MUXMEASUREMENT_CONFIG;
                }
            }
            else if(ltc_state.substate == LTC_READ_I2C_TRANSMISSION_RESULT_RDCOMM_MUXMEASUREMENT_CONFIG)
            {
                ltc_state.lastsubstate=ltc_state.substate;
                retVal = LTC_RX( (uint8_t*)ltc_cmdRDCOMM, ltc_DataBufferSPI_RX_with_PEC_temperatures);
                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.ErrRetryCounter=0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;        // wait for next step
                    ltc_state.substate = LTC_START_GPIO_MEASUREMENT_MUXMEASUREMENT_CONFIG;
                }

            }
            else if(ltc_state.substate == LTC_START_GPIO_MEASUREMENT_MUXMEASUREMENT_CONFIG)
            {
                if(ltc_state.lastsubstate == LTC_READ_I2C_TRANSMISSION_RESULT_RDCOMM_MUXMEASUREMENT_CONFIG)
                {
                    if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_temperatures)!=0)
                    {
                        if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                        {
                            if (LTC_DISCARD_PEC_CHECK == FALSE)
                            {
                                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                                ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                                ltc_state.substate = LTC_ERROR_ENTRY;
                                break;
                            }
                        }
                        else
                        {
                            ltc_state.lastsubstate = ltc_state.substate;
                            ltc_state.substate = LTC_READ_I2C_TRANSMISSION_RESULT_RDCOMM_MUXMEASUREMENT_CONFIG;
                            ltc_state.timer = LTC_STATEMACH_PECERRTIME;
                            break;
                        }
                    }
                    LTC_ResetErrorTable();
                    ltc_state.ErrPECCounter = 0;
                    mux_error=LTC_I2CCheckACK(ltc_DataBufferSPI_RX_with_PEC_temperatures, ltc_state.muxmeas_seqptr->muxID);
                    if(mux_error!=0)
                    {
                        if (LTC_DISCARD_MUX_CHECK == FALSE)
                        {
                            ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                            ltc_state.state = LTC_STATEMACH_ERROR_MUXFAILED;
                            ltc_state.substate = LTC_ERROR_ENTRY;
                            break;
                        }
                    }
                }

                ltc_state.lastsubstate=ltc_state.substate;

                if(ltc_state.muxmeas_seqptr->muxCh == 0xFF)
                {    // actual multiplexer is switched off, so do not make a measurement and follow up with next step (mux configuration)
                    ++ltc_state.muxmeas_seqptr;         /* go further with next step of sequence
                                                           ltc_state.numberOfMeasuredMux not decremented, this does not count as a measurement */
                    ltc_state.lastsubstate = ltc_state.substate;
                    ltc_state.substate = LTC_SET_MUX_CHANNEL_WRCOMM_MUXMEASUREMENT_CONFIG;
                    ltc_state.state = LTC_STATEMACH_MUXMEASUREMENT_CONFIG;
                    ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                    ltc_state.ErrRetryCounter = 0;
                    break;
                }
                else
                {
                    retVal=LTC_StartGPIOMeasurement(ltc_state.adcMode, ltc_state.adcMeasCh);
                    if(retVal != E_OK)
                    {
                        ++ltc_state.ErrRetryCounter;
                        ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                        if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                        {
                            ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                            ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                            ltc_state.substate = LTC_ERROR_ENTRY;
                            break;
                        }
                    }
                    else
                    {
                        LTC_SAVELASTSTATES();
                        ltc_state.state = LTC_STATEMACH_MUXMEASUREMENT;
                        ltc_state.substate = LTC_READ_AUXILIARY_REGISTER_A_RAUXA_MUXMEASUREMENT;
                        ltc_state.timer = ltc_state.commandTransferTime + LTC_Get_MeasurementTCycle(ltc_state.adcMode,ltc_state.adcMeasCh); // wait, ADAX-Command
                        ltc_state.ErrRetryCounter = 0;
                    }
                }
            }
            else
            {
                // state sequence error
                LTC_SAVELASTSTATES();
            }
            break;

        /****************************MULTIPLEXED MEASUREMENT*************************/
        case LTC_STATEMACH_MUXMEASUREMENT:
            if(ltc_state.substate == LTC_READ_AUXILIARY_REGISTER_A_RAUXA_MUXMEASUREMENT)
            {

                ltc_state.lastsubstate=ltc_state.substate;
                retVal = LTC_RX( (uint8_t*)ltc_cmdRDAUXA, ltc_DataBufferSPI_RX_with_PEC_temperatures);
                if(retVal != E_OK)
                {
                    ++ltc_state.ErrRetryCounter;
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                    ltc_state.substate = LTC_SAVE_MUX_MEASUREMENT_MUXMEASUREMENT;
                }

            }
            else if(ltc_state.substate == LTC_SAVE_MUX_MEASUREMENT_MUXMEASUREMENT)
            {
                ltc_state.lastsubstate=ltc_state.substate;
                if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_temperatures)!=E_OK)
                {
                    if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                    {
                        if (LTC_DISCARD_PEC_CHECK == FALSE)
                        {
                            ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                            ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                            ltc_state.substate = LTC_ERROR_ENTRY;
                            break;
                        }
                    }
                }
                LTC_ResetErrorTable();
                LTC_SaveMuxMeasurement(ltc_DataBufferSPI_RX_with_PEC_temperatures, ltc_state.muxmeas_seqptr);

                ++(ltc_state.muxmeas_seqptr);        // go further with next step of sequence
                --(ltc_state.numberOfMeasuredMux);
                if(ltc_state.muxmeas_seqptr >= ltc_state.muxmeas_seqendptr)
                {    // last step of sequence reached
                    // The mux sequence starts again
                    ltc_muxcycle_finished = E_OK;

                    ltc_state.muxmeas_seqptr = ltc_mux_seq.seqptr;
                    ltc_state.muxmeas_nr_end = ltc_mux_seq.nr_of_steps;
                    ltc_state.muxmeas_seqendptr = ((LTC_MUX_CH_CFG_s *)ltc_mux_seq.seqptr)+ltc_mux_seq.nr_of_steps; // last sequence + 1

                    ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                    LTC_SAVELASTSTATES();
                    ltc_state.substate = LTC_ENTRY;
                    ltc_state.state = LTC_STATEMACH_MUXMEASUREMENT_FINISHED;
                }
                else if(ltc_state.numberOfMeasuredMux == 0)
                {    // number of multiplexers reached for this cycle, but multiplexer sequence not finished yet
                    ltc_muxcycle_finished = E_NOT_OK;
                    ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                    LTC_SAVELASTSTATES();
                    ltc_state.substate = LTC_ENTRY;
                    ltc_state.state = LTC_STATEMACH_MUXMEASUREMENT_FINISHED;
                }
                else
                {
                    LTC_SAVELASTSTATES();
                    ltc_state.substate = LTC_SET_MUX_CHANNEL_WRCOMM_MUXMEASUREMENT_CONFIG;
                    ltc_state.state = LTC_STATEMACH_MUXMEASUREMENT_CONFIG;
                    ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                }
                ltc_state.ErrRetryCounter = 0;
                ltc_state.ErrPECCounter = 0;
                break;

            }
            else
            {
                // state sequence error
                LTC_SAVELASTSTATES();
            }
            break;

        /****************************MULTIPLEXER MEASUREMENT FINISHED****************/
        case LTC_STATEMACH_MUXMEASUREMENT_FINISHED:
                LTC_SAVELASTSTATES();
                ltc_state.substate = LTC_ENTRY;
                ltc_state.state = LTC_STATEMACH_IDLE;
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.ErrRetryCounter = 0;
            break;

        /****************************MEASUREMENT OF ALL GPIOS************************/
        case LTC_STATEMACH_ALLGPIOMEASUREMENT:
            retVal = LTC_StartGPIOMeasurement(ltc_state.adcMode, ltc_state.adcMeasCh);
            LTC_SAVELASTSTATES();
            if((retVal != E_OK))
            {
                ++(ltc_state.ErrRetryCounter);
                ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;    // wait
                if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                {
                    ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                    ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                    ltc_state.substate = LTC_ERROR_ENTRY;
                    break;
                }
            }
            else
            {
                ltc_state.ErrRetryCounter = 0;
                ltc_state.timer = ltc_state.commandTransferTime + LTC_Get_MeasurementTCycle(ltc_state.adcMode, ltc_state.adcMeasCh);
                ltc_state.state = LTC_STATEMACH_READALLGPIO;
                ltc_state.ErrPECCounter = 0;
            }
            break;

        /****************************MEASUREMENT OF ALL GPIOS************************/
        // used in case there are no multiplexers, measurement of all GPIOs
        case LTC_STATEMACH_READALLGPIO:
            if(ltc_state.substate == LTC_READ_AUXILIARY_REGISTER_A_RAUXA_READALLGPIO)
            {
                LTC_SAVELASTSTATES();
                retVal = LTC_RX( (uint8_t*)ltc_cmdRDAUXA,ltc_DataBufferSPI_RX_with_PEC_temperatures);
                if(retVal != E_OK)
                {
                    ++(ltc_state.ErrRetryCounter);
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.substate = LTC_READ_AUXILIARY_REGISTER_B_RAUXB_READALLGPIO;
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                }

            }
            else if (ltc_state.substate == LTC_READ_AUXILIARY_REGISTER_B_RAUXB_READALLGPIO)
            {
                if(ltc_state.lastsubstate == LTC_READ_AUXILIARY_REGISTER_A_RAUXA_READALLGPIO)
                {
                    if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_temperatures)!=E_OK)
                    {
                        if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                        {
                            if (LTC_DISCARD_PEC_CHECK == FALSE)
                            {
                                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                                ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                                ltc_state.substate = LTC_ERROR_ENTRY;
                                break;
                            }

                        }
                        else
                        {
                            ltc_state.lastsubstate = ltc_state.substate;
                            ltc_state.substate = LTC_READ_AUXILIARY_REGISTER_A_RAUXA_READALLGPIO;
                            ltc_state.timer = LTC_STATEMACH_PECERRTIME;
                            break;
                        }
                    }
                    LTC_ResetErrorTable();
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.ErrPECCounter = 0;
                    LTC_SaveGPIOMeasurement(0, ltc_DataBufferSPI_RX_with_PEC_temperatures);
                }

                ltc_state.lastsubstate = ltc_state.substate;
                retVal = LTC_RX( (uint8_t*)ltc_cmdRDAUXB, ltc_DataBufferSPI_RX_with_PEC_temperatures);
                if(retVal != E_OK)
                {
                    ++(ltc_state.ErrRetryCounter);
                    ltc_state.timer = LTC_STATEMACH_SEQERRTTIME;
                    if (ltc_state.ErrRetryCounter>LTC_TRANSMIT_SPIERRLIMIT)
                    {
                        ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                        ltc_state.state = LTC_STATEMACH_ERROR_SPIFAILED;
                        ltc_state.substate = LTC_ERROR_ENTRY;
                        break;
                    }
                }
                else
                {
                    ltc_state.substate = LTC_EXIT_READALLGPIO;
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.timer = ltc_state.commandDataTransferTime;
                }

            }
            else if (ltc_state.substate == LTC_EXIT_READALLGPIO)
            {
                if(ltc_state.lastsubstate == LTC_READ_AUXILIARY_REGISTER_B_RAUXB_READALLGPIO)
                {

                    if(LTC_RX_PECCheck(ltc_DataBufferSPI_RX_with_PEC_temperatures)!=E_OK)
                    {
                        if(++ltc_state.ErrPECCounter>LTC_TRANSMIT_PECERRLIMIT)
                        {
                            if (LTC_DISCARD_PEC_CHECK == FALSE)
                            {
                                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                                ltc_state.state = LTC_STATEMACH_ERROR_PECFAILED;
                                ltc_state.substate = LTC_ERROR_ENTRY;
                                break;
                            }
                        }
                        else
                        {
                            ltc_state.lastsubstate = ltc_state.substate;
                            ltc_state.substate=LTC_READ_AUXILIARY_REGISTER_B_RAUXB_READALLGPIO;
                            ltc_state.timer = LTC_STATEMACH_PECERRTIME;
                            break;
                        }
                    }
                    LTC_ResetErrorTable();
                    ltc_state.ErrRetryCounter = 0;
                    ltc_state.ErrPECCounter = 0;
                    LTC_SaveGPIOMeasurement(1,ltc_DataBufferSPI_RX_with_PEC_temperatures);
                }

                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                LTC_SAVELASTSTATES();
                ltc_state.substate = LTC_ENTRY;
                ltc_state.state = LTC_STATEMACH_IDLE;

            }
            break;

        /****************************SPI ERROR***************************************/
        case LTC_STATEMACH_ERROR_SPIFAILED:
            if(ltc_state.substate == LTC_ERROR_ENTRY)
            {
                DIAG_Handler(DIAG_CH_LTC_SPI,DIAG_EVENT_NOK,0, NULL);
                ltc_state.substate =  LTC_ERROR_PROCESSED;
            }

            statereq=LTC_TransferStateRequest(&tmpbusID,&tmpadcMode,&tmpadcMeasCh);
            if(statereq == LTC_STATE_REINIT_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_INITIALIZATION;
                ltc_state.substate = LTC_ENTRY_INITIALIZATION;
                LTC_ResetErrorTable();
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = LTC_ADCMODE_UNDEFINED;   // not needed for this state
                ltc_state.adcMeasCh = LTC_ADCMEAS_UNDEFINED; // not needed for this state
            }
            ltc_state.timer = LTC_STATEMACH_SHORTTIME;

            break;

        /****************************PEC ERROR***************************************/
        case LTC_STATEMACH_ERROR_PECFAILED:
            if(ltc_state.substate == LTC_ERROR_ENTRY)
            {
                DIAG_Handler(DIAG_CH_COM_LTC_PEC,DIAG_EVENT_NOK, 0, NULL);
                ltc_state.substate =  LTC_ERROR_PROCESSED;
            }

            statereq = LTC_TransferStateRequest(&tmpbusID, &tmpadcMode, &tmpadcMeasCh);
            if(statereq == LTC_STATE_REINIT_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_INITIALIZATION;
                ltc_state.substate = LTC_ENTRY_INITIALIZATION;
                LTC_ResetErrorTable();
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = LTC_ADCMODE_UNDEFINED;   // not needed for this state
                ltc_state.adcMeasCh = LTC_ADCMEAS_UNDEFINED; // not needed for this state
            }
            ltc_state.timer = LTC_STATEMACH_SHORTTIME;
            break;

        case LTC_STATEMACH_ERROR_MUXFAILED:
            if(ltc_state.substate == LTC_ERROR_ENTRY)
            {
                DIAG_Handler(DIAG_CH_LTC_MUX, DIAG_EVENT_NOK, 0, NULL);
                ltc_state.substate = LTC_ERROR_PROCESSED;
            }

            statereq=LTC_TransferStateRequest(&tmpbusID,&tmpadcMode,&tmpadcMeasCh);
            if(statereq == LTC_STATE_REINIT_REQUEST)
            {
                LTC_SAVELASTSTATES();
                ltc_state.timer = LTC_STATEMACH_SHORTTIME;
                ltc_state.state = LTC_STATEMACH_INITIALIZATION;
                ltc_state.substate = LTC_ENTRY_INITIALIZATION;
                LTC_ResetErrorTable();
                ltc_state.ErrRetryCounter = 0;
                ltc_state.adcMode = LTC_ADCMODE_UNDEFINED;   // not needed for this state
                ltc_state.adcMeasCh = LTC_ADCMEAS_UNDEFINED; // not needed for this state
            }
            ltc_state.timer = LTC_STATEMACH_SHORTTIME;
            break;

        default:
            break;
    }

    ltc_state.triggerentry--;        // reentrance counter
}



/**
 * @brief   saves the multiplexer values read from the LTC daisy-chain.
 *
 * After a voltage measurement was initiated on GPIO 1 to read the currently selected
 * multiplexer voltage, the results is read via SPI from the daisy-chain.
 * This function is called to store the result from the transmission in a buffer.
 *
 * @param   *DataBufferSPI_RX   buffer containing the data obtained from the SPI transmission
 * @param   muxseqptr           pointer to the multiplexer sequence, which configures the currently selected multiplexer ID and channel
 *
 * @return  void
 *
 */
static void LTC_SaveMuxMeasurement(uint8_t *DataBufferSPI_RX, LTC_MUX_CH_CFG_s  *muxseqptr) {   // pointer to measurement Sequence of Mux- and Channel-Configurations (1,0xFF)...(3,0xFF),(0,1),...(0,7))

    uint16_t i = 0;

    if(muxseqptr->muxCh == 0xFF)
        return; /* Channel 0xFF means that the multiplexer is deactivated, therefore no measurement will be made and saved*/

    for (i=0;i<LTC_N_LTC;i++) {

        LTC_MultiplexerVoltages[2*(i*LTC_N_MUX_CHANNELS_PER_LTC+muxseqptr->muxID*LTC_N_MUX_CHANNELS_PER_MUX+muxseqptr->muxCh)] = DataBufferSPI_RX[4+1*i*8];        // raw values, all multiplexers on all LTCs
        LTC_MultiplexerVoltages[2*(i*LTC_N_MUX_CHANNELS_PER_LTC+muxseqptr->muxID*LTC_N_MUX_CHANNELS_PER_MUX+muxseqptr->muxCh)+1] = DataBufferSPI_RX[4+1*i*8+1];        // raw values, all multiplexers on all LTCs
    }
}


/**
 * @brief   saves the GPIO voltage values read from the LTC daisy-chain.
 *
 * After a voltage measurement was initiated to measure the voltages on all GPIOs,
 * the result is read via SPI from the daisy-chain. In order to read the result of all GPIO measurements,
 * it is necessary to read auxiliary register A and B.
 * Only one register can be read at a time.
 * This function is called to store the result from the transmission in a buffer.
 *
 * @param   registerSet    voltage register that was read (auxiliary register A or B)
 * @param   *rxBuffer      buffer containing the data obtained from the SPI transmission
 *
 * @return  void
 *
 */
static void LTC_SaveGPIOMeasurement(uint8_t registerSet, uint8_t *rxBuffer) {

    uint16_t i = 0;
    uint8_t i_offset = 0;

    if(registerSet == 0) {
    // RDAUXA command -> GPIO register group A
        i_offset=0;
    }
    else if(registerSet == 1) {
    // RDAUXB command -> GPIO register group B
        i_offset=6;
    }
    else
        return;

    /* Retrieve data without command and CRC*/
    for(i=0;i<LTC_N_LTC;i++) {

        LTC_GPIOVoltages[0+i_offset+12*i]= rxBuffer[4+i*8];
        LTC_GPIOVoltages[1+i_offset+12*i]= rxBuffer[5+i*8];
        LTC_GPIOVoltages[2+i_offset+12*i]= rxBuffer[6+i*8];
        LTC_GPIOVoltages[3+i_offset+12*i]= rxBuffer[7+i*8];
        LTC_GPIOVoltages[4+i_offset+12*i]= rxBuffer[8+i*8];
        LTC_GPIOVoltages[5+i_offset+12*i]= rxBuffer[9+i*8];
    }
}

/**
 * @brief   saves the voltage values read from the LTC daisy-chain.
 *
 * After a voltage measurement was initiated to measure the voltages of the cells,
 * the result is read via SPI from the daisy-chain.
 * There are 4 register to read _(A,B,C,D) to get all cell voltages.
 * Only one register can be read at a time.
 * This function is called to store the result from the transmission in a buffer.
 *
 * @param   registerSet    voltage register that was read (voltage register A,B,C or D)
 * @param   *rxBuffer      buffer containing the data obtained from the SPI transmission
 *
 * @return  void
 *
 */
static void LTC_SaveRXtoVoltagebuffer(uint8_t registerSet, uint8_t *rxBuffer) {

    uint16_t i = 0;
    uint8_t i_offset=0;


    if(registerSet == 0) {
      // RDCVA command -> voltage register group A
        i_offset=0;
    }
    else if(registerSet == 1) {
    // RDCVB command -> voltage register group B
        i_offset=6;
    }
    else if(registerSet == 2) {
    // RDCVC command -> voltage register group C
        i_offset=12;
    }
    else if(registerSet == 3) {
    // RDCVD command -> voltage register group D
        i_offset=18;
    }
    else
        return;

    /* Retrieve data without command and CRC*/
    for(i=0;i<LTC_N_LTC;i++) {

        LTC_CellVoltages[0+i_offset+24*i]= rxBuffer[4+i*8];
        LTC_CellVoltages[1+i_offset+24*i]= rxBuffer[5+i*8];
        LTC_CellVoltages[2+i_offset+24*i]= rxBuffer[6+i*8];
        LTC_CellVoltages[3+i_offset+24*i]= rxBuffer[7+i*8];
        LTC_CellVoltages[4+i_offset+24*i]= rxBuffer[8+i*8];
        LTC_CellVoltages[5+i_offset+24*i]= rxBuffer[9+i*8];
    }
}


/**
 * @brief   checks if the multiplexers acknowledged transmission.
 *
 * The RDCOMM command can be used to read the answer of the multiplexers to a
 * I2C transmission.
 * This function determines if the communication with the multiplexers was
 * successful or not.
 * The array LTC_ErrorTable is updated to locate the multiplexers that did not
 * acknowledge transmission.
 *
 * @param   *DataBufferSPI_RX    data obtained from the SPI transmission
 * @param   mux                  multiplexer to be addressed (multiplexer ID)
 *
 * @return  mux_error            0 is there was no error, 1 if there was errors
 */
static uint8_t LTC_I2CCheckACK(uint8_t *DataBufferSPI_RX, int mux) {
    uint8_t mux_error = 0;
    uint16_t i = 0;

    for(i=0;i<BS_NR_OF_MODULES;i++) {
        if (mux == 0){
            if (DataBufferSPI_RX[4+1+LTC_NUMBER_OF_LTC_PER_MODULE*i*8]!=0x07) {    // ACK = 0xX7
                LTC_ErrorTable[i].mux0=1;
                mux_error=1;
            }
        }
        if (mux == 1){
            if (DataBufferSPI_RX[4+1+LTC_NUMBER_OF_LTC_PER_MODULE*i*8]!=0x27) {
                LTC_ErrorTable[i].mux1=1;
                mux_error=1;
            }
        }
        if (mux == 2){
            if (DataBufferSPI_RX[4+1+LTC_NUMBER_OF_LTC_PER_MODULE*i*8]!=0x47) {
                LTC_ErrorTable[i].mux2=1;
                mux_error=1;
            }
        }
        if (mux == 3){
            if (DataBufferSPI_RX[4+1+LTC_NUMBER_OF_LTC_PER_MODULE*i*8]!=0x67) {
                LTC_ErrorTable[i].mux3=1;
                mux_error=1;
            }
        }
    }

    return mux_error;
}



/*
 * @brief   initialize the daisy-chain.
 *
 * To initialize the LTC6804 daisy-chain, a dummy byte (0x00) is sent.
 *
 * @return  retVal  E_OK if dummy byte was sent correctly by SPI, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_Init(void) {

    STD_RETURN_TYPE_e statusSPI = E_NOT_OK;
    STD_RETURN_TYPE_e retVal = E_OK;

    uint8_t PEC_Check[6];
    uint16_t PEC_result = 0;
    uint16_t i = 0;


//The transfer-time is set according to the length of the daisy-chain

    //now set REFON bit to 1
    //data for the configuration
    for (i=0;i<LTC_N_LTC;i++){

        //FC = disable all pull-downs
        ltc_tmpTXbuffer[0+(1*i)*6]=0xFC;
        ltc_tmpTXbuffer[1+(1*i)*6]=0x00;
        ltc_tmpTXbuffer[2+(1*i)*6]=0x00;
        ltc_tmpTXbuffer[3+(1*i)*6]=0x00;
        ltc_tmpTXbuffer[4+(1*i)*6]=0x00;
        ltc_tmpTXbuffer[5+(1*i)*6]=0x00;
    }

    //now construct the message to be sent: it contains the wanted data, PLUS the needed PECs
    ltc_DataBufferSPI_TX_with_PEC_init[0] = ltc_cmdRefOn[0];
    ltc_DataBufferSPI_TX_with_PEC_init[1] = ltc_cmdRefOn[1];
    ltc_DataBufferSPI_TX_with_PEC_init[2] = ltc_cmdRefOn[2];
    ltc_DataBufferSPI_TX_with_PEC_init[3] = ltc_cmdRefOn[3];

    for(i=0;i<LTC_N_LTC;i++){

        PEC_Check[0]=ltc_DataBufferSPI_TX_with_PEC_init[4+i*8]=ltc_tmpTXbuffer[0+i*6];
        PEC_Check[1]=ltc_DataBufferSPI_TX_with_PEC_init[5+i*8]=ltc_tmpTXbuffer[1+i*6];
        PEC_Check[2]=ltc_DataBufferSPI_TX_with_PEC_init[6+i*8]=ltc_tmpTXbuffer[2+i*6];
        PEC_Check[3]=ltc_DataBufferSPI_TX_with_PEC_init[7+i*8]=ltc_tmpTXbuffer[3+i*6];
        PEC_Check[4]=ltc_DataBufferSPI_TX_with_PEC_init[8+i*8]=ltc_tmpTXbuffer[4+i*6];
        PEC_Check[5]=ltc_DataBufferSPI_TX_with_PEC_init[9+i*8]=ltc_tmpTXbuffer[5+i*6];

        PEC_result=LTC_pec15_calc(6, PEC_Check);
        ltc_DataBufferSPI_TX_with_PEC_init[10+i*8]=(uint8_t)((PEC_result>>8)&0xff);
        ltc_DataBufferSPI_TX_with_PEC_init[11+i*8]=(uint8_t)(PEC_result&0xff);

    } //end for

    statusSPI = LTC_SendData(ltc_DataBufferSPI_TX_with_PEC_init);

    if(statusSPI != E_OK)
    {
        retVal = E_NOT_OK;
    }

    retVal = statusSPI;

    return retVal;
}




/*
 * @brief   sets the balancing according to the control values read in the database.
 *
 * To set balancing for the cells, the corresponding bits have to be written in the configuration register.
 * The LTC driver only executes the balancing orders written by the BMS in the database.
 *
 * @return  E_OK if dummy byte was sent correctly by SPI, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_BalanceControl(void) {

    STD_RETURN_TYPE_e retVal = E_OK;

    uint16_t i = 0;
    uint16_t j = 0;

    LTC_Get_BalancingControlValues();


    for (j=0;j<BS_NR_OF_MODULES;j++) {

        i=BS_NR_OF_MODULES-j-1;

        // FC = disable all pull-downs
        ltc_tmpTXbuffer[0+(i)*6]=0xFC;
        ltc_tmpTXbuffer[1+(i)*6]=0x00;
        ltc_tmpTXbuffer[2+(i)*6]=0x00;
        ltc_tmpTXbuffer[3+(i)*6]=0x00;
        ltc_tmpTXbuffer[4+(i)*6]=0x00;
        ltc_tmpTXbuffer[5+(i)*6]=0x00;

        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+0] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x01;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+1] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x02;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+2] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x04;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+3] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x08;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+4] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x10;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+5] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x20;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+6] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x40;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+7] == 1) {
            ltc_tmpTXbuffer[4+(i)*6]|=0x80;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+8] == 1) {
            ltc_tmpTXbuffer[5+(i)*6]|=0x01;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+9] == 1) {
            ltc_tmpTXbuffer[5+(i)*6]|=0x02;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+10] == 1) {
            ltc_tmpTXbuffer[5+(i)*6]|=0x04;
        }
        if (ltc_balancing_control.value[j*(BS_NR_OF_BAT_CELLS_PER_MODULE)+11] == 1) {
            ltc_tmpTXbuffer[5+(i)*6]|=0x08;
        }

    }

    retVal = LTC_TX((uint8_t*)ltc_cmdRefOn, ltc_tmpTXbuffer, ltc_DataBufferSPI_TX_with_PEC_init);

    return retVal;
}


/*
 * @brief   resets the error table.
 *
 * This function should be called during initialization or before starting a new measurement cycle
 *
 * @return  void
 *
 */
static void LTC_ResetErrorTable(void) {
    uint16_t i = 0;
    uint16_t j = 0;

    for (i=0;i<BS_NR_OF_MODULES;i++) {
        for (j=0;j<LTC_NUMBER_OF_LTC_PER_MODULE;j++) {
        LTC_ErrorTable[i].LTC[j]=0;
        }
        LTC_ErrorTable[i].mux0=0;
        LTC_ErrorTable[i].mux1=0;
        LTC_ErrorTable[i].mux2=0;
        LTC_ErrorTable[i].mux3=0;
    }
}


/**
 * @brief   brief missing
 *
 * Gets the measurement time needed by the LTC chip, depending on the measurement mode and the number of channels.
 * For all cell voltages or all 5 GPIOS, the measurement time is the same.
 * For 2 cell voltages or 1 GPIO, the measurement time is the same.
 * As a consequence, this function is used for cell voltage and for GPIO measurement.
 *
 * @param   adcMode     LTC ADCmeasurement mode (fast, normal or filtered)
 * @param   adcMeasCh   number of channels measured for GPIOS (one at a time for multiplexers or all five GPIOs)
 *                      or number of cell voltage measured (2 cells or all cells)
 *
 * @return  retVal      measurement time in ms
 */
static uint16_t LTC_Get_MeasurementTCycle(LTC_ADCMODE_e adcMode, LTC_ADCMEAS_CHAN_e  adcMeasCh) {

    uint16_t retVal= LTC_STATEMACH_MEAS_ALL_NORMAL_TCYCLE;    // default

    if(adcMeasCh == LTC_ADCMEAS_ALLCHANNEL) {

        if(adcMode == LTC_ADCMODE_FAST_DCP0 || adcMode == LTC_ADCMODE_FAST_DCP1) {

            retVal = LTC_STATEMACH_MEAS_ALL_FAST_TCYCLE;
        }
        else if(adcMode == LTC_ADCMODE_NORMAL_DCP0 || adcMode == LTC_ADCMODE_NORMAL_DCP1) {

            retVal = LTC_STATEMACH_MEAS_ALL_NORMAL_TCYCLE;
        }
        else if(adcMode == LTC_ADCMODE_FILTERED_DCP0 || adcMode == LTC_ADCMODE_FILTERED_DCP1) {

            retVal = LTC_STATEMACH_MEAS_ALL_FILTERED_TCYCLE;
        }
    } else if (adcMeasCh == LTC_ADCMEAS_SINGLECHANNEL) {

        if(adcMode == LTC_ADCMODE_FAST_DCP0 || adcMode == LTC_ADCMODE_FAST_DCP1) {

            retVal = LTC_STATEMACH_MEAS_SINGLE_FAST_TCYCLE;
        }
        else if(adcMode == LTC_ADCMODE_NORMAL_DCP0 || adcMode == LTC_ADCMODE_NORMAL_DCP1) {

            retVal = LTC_STATEMACH_MEAS_SINGLE_NORMAL_TCYCLE;
        }
        else if(adcMode == LTC_ADCMODE_FILTERED_DCP0 || adcMode == LTC_ADCMODE_FILTERED_DCP1) {

            retVal = LTC_STATEMACH_MEAS_SINGLE_FILTERED_TCYCLE;
        }
    } else {

        retVal = LTC_STATEMACH_MEAS_ALL_NORMAL_TCYCLE;
    }

    return retVal;
}


/**
 * @brief   tells the LTC daisy-chain to start measuring the voltage on all cells.
 *
 * This function sends an instruction to the daisy-chain via SPI, in order to start voltage measurement for all cells.
 *
 * @param   adcMode     LTC ADCmeasurement mode (fast, normal or filtered)
 * @param   adcMeasCh   number of cell voltage measured (2 cells or all cells)
 *
 * @return  retVal      E_OK if dummy byte was sent correctly by SPI, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_StartVoltageMeasurement(LTC_ADCMODE_e adcMode, LTC_ADCMEAS_CHAN_e adcMeasCh) {

    STD_RETURN_TYPE_e retVal=E_OK;

    if(adcMeasCh != LTC_ADCMEAS_ALLCHANNEL)
        return E_NOT_OK;                        // t.b.d. single channels actually not supported

    if(adcMode == LTC_ADCMODE_FAST_DCP0) {

        retVal = LTC_SendCmd(ltc_cmdADCV_fast_DCP0);
    }
    else if(adcMode == LTC_ADCMODE_NORMAL_DCP0) {

        retVal = LTC_SendCmd(ltc_cmdADCV_normal_DCP0);
    }
    else if(adcMode == LTC_ADCMODE_FILTERED_DCP0) {

        retVal = LTC_SendCmd(ltc_cmdADCV_filtered_DCP0);
    }
    else if(adcMode == LTC_ADCMODE_FAST_DCP1) {

        retVal = LTC_SendCmd(ltc_cmdADCV_fast_DCP1);
    }
    else if(adcMode == LTC_ADCMODE_NORMAL_DCP1) {

        retVal = LTC_SendCmd(ltc_cmdADCV_normal_DCP1);
    }
    else if(adcMode == LTC_ADCMODE_FILTERED_DCP1) {

        retVal = LTC_SendCmd(ltc_cmdADCV_filtered_DCP1);
    }
    else {
        retVal = E_NOT_OK;
    }
    return retVal;
}


/**
 * @brief   tells LTC daisy-chain to start measuring the voltage on GPIOS.
 *
 * This function sends an instruction to the daisy-chain via SPI to start the measurement.
 *
 * @param   adcMode     LTC ADCmeasurement mode (fast, normal or filtered)
 * @param   adcMeasCh   number of channels measured for GPIOS (one at a time, typically when multiplexers are used, or all five GPIOs)
 *
 * @return  retVal      E_OK if dummy byte was sent correctly by SPI, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_StartGPIOMeasurement(LTC_ADCMODE_e adcMode, LTC_ADCMEAS_CHAN_e  adcMeasCh) {

    STD_RETURN_TYPE_e retVal;

    if(adcMeasCh == LTC_ADCMEAS_ALLCHANNEL) {

        if(adcMode == LTC_ADCMODE_FAST_DCP0 || adcMode == LTC_ADCMODE_FAST_DCP1) {

            retVal = LTC_SendCmd(ltc_cmdADAX_fast_ALLGPIOS);
        }
        else if(adcMode == LTC_ADCMODE_FILTERED_DCP0 || adcMode == LTC_ADCMODE_FILTERED_DCP1) {

            retVal = LTC_SendCmd(ltc_cmdADAX_filtered_ALLGPIOS);
        } else {
            /*if(adcMode == LTC_ADCMODE_NORMAL_DCP0 || adcMode == LTC_ADCMODE_NORMAL_DCP1)*/
            retVal = LTC_SendCmd(ltc_cmdADAX_normal_ALLGPIOS);
        }
    } else if (adcMeasCh == LTC_ADCMEAS_SINGLECHANNEL) {
        // Single Channel
        if(adcMode == LTC_ADCMODE_FAST_DCP0 || adcMode == LTC_ADCMODE_FAST_DCP1) {

            retVal = LTC_SendCmd(ltc_cmdADAX_fast_GPIO1);
        }
        else if(adcMode == LTC_ADCMODE_FILTERED_DCP0 || adcMode == LTC_ADCMODE_FILTERED_DCP1) {

            retVal = LTC_SendCmd(ltc_cmdADAX_filtered_GPIO1);
        } else {
            /*if(adcMode == LTC_ADCMODE_NORMAL_DCP0 || adcMode == LTC_ADCMODE_NORMAL_DCP1)*/

            retVal = LTC_SendCmd(ltc_cmdADAX_normal_GPIO1);
        }
    } else {
        retVal = E_NOT_OK;
    }

    return retVal;
}


/**
 * @brief   checks if the data received from the daisy-chain is not corrupt.
 *
 * This function computes the PEC (CRC) from the data received by the daisy-chain.
 * It compares it with the PEC sent by the LTCs.
 * If there are errors, the array LTC_ErrorTable is updated to locate the LTCs in daisy-chain
 * that transmitted corrupt data.
 *
 * @param   *DataBufferSPI_RX_with_PEC   data obtained from the SPI transmission
 *
 * @return  retVal                       E_OK if PEC check is OK, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_RX_PECCheck(uint8_t *DataBufferSPI_RX_with_PEC) {

    uint16_t i = 0;
    uint16_t j = 0;
    STD_RETURN_TYPE_e retVal=E_OK;
    uint8_t PEC_TX[2];
    uint16_t PEC_result = 0;
    uint8_t PEC_Check[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

    // check all PECs and put data without command and PEC in DataBufferSPI_RX (easier to use)
    for(i=0;i<LTC_N_LTC;i++) {

        PEC_Check[0] = DataBufferSPI_RX_with_PEC[4+i*8];
        PEC_Check[1] = DataBufferSPI_RX_with_PEC[5+i*8];
        PEC_Check[2] = DataBufferSPI_RX_with_PEC[6+i*8];
        PEC_Check[3] = DataBufferSPI_RX_with_PEC[7+i*8];
        PEC_Check[4] = DataBufferSPI_RX_with_PEC[8+i*8];
        PEC_Check[5] = DataBufferSPI_RX_with_PEC[9+i*8];

        PEC_result=LTC_pec15_calc(6, PEC_Check);
        PEC_TX[0]=(uint8_t)((PEC_result>>8)&0xff);
        PEC_TX[1]=(uint8_t)(PEC_result&0xff);

        // if calculated PEC not equal to received PEC
        if ( (PEC_TX[0]!=DataBufferSPI_RX_with_PEC[10+i*8]) || (PEC_TX[1]!=DataBufferSPI_RX_with_PEC[11+i*8]) ){

            // update error table of the corresponding LTC
            j=i%LTC_NUMBER_OF_LTC_PER_MODULE;
            LTC_ErrorTable[i/LTC_NUMBER_OF_LTC_PER_MODULE].LTC[j]=1;

            retVal=E_NOT_OK;

        }
        else {
            retVal = E_OK;
        }
    }

    return (retVal);
}


/**
 * @brief   send command to the LTC daisy-chain and receives data from the LTC daisy-chain.
 *
 * This is the core function to receive data from the LTC6804 daisy-chain.
 * A 2 byte command is sent with the corresponding PEC. Example: read configuration register (RDCFG).
 * Only command has to be set, the function calculates the PEC automatically.
 * The data send is:
 * 2 bytes (COMMAND) 2 bytes (PEC)
 * The data received is:
 * 6 bytes (LTC1) 2 bytes (PEC) + 6 bytes (LTC2) 2 bytes (PEC) + 6 bytes (LTC3) 2 bytes (PEC) + ... + 6 bytes (LTC{LTC_N_LTC}) 2 bytes (PEC)
 *
 * The function does not check the PECs. This has to be done elsewhere.
 *
 * @param   *Command                    command sent to the daisy-chain
 * @param   *DataBufferSPI_RX_with_PEC  data to sent to the daisy-chain, i.e. data to be sent + PEC
 *
 * @return  statusSPI                   E_OK if SPI transmission is OK, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_RX(uint8_t *Command, uint8_t *DataBufferSPI_RX_with_PEC) {

    STD_RETURN_TYPE_e statusSPI = E_OK;
    uint16_t i = 0;

    // DataBufferSPI_RX_with_PEC contains the data to receive.
    // The transmission function checks the PECs.
    // It constructs DataBufferSPI_RX, which contains the received data without PEC (easier to use).

    for(i=0;i<LTC_N_BYTES_FOR_DATA_TRANSMISSION;i++) {
        ltc_tmpTXPECbuffer[i]=0x00;
    }

    ltc_tmpTXPECbuffer[0] = Command[0];
    ltc_tmpTXPECbuffer[1] = Command[1];
    ltc_tmpTXPECbuffer[2] = Command[2];
    ltc_tmpTXPECbuffer[3] = Command[3];


    statusSPI = LTC_ReceiveData(ltc_tmpTXPECbuffer,DataBufferSPI_RX_with_PEC);

    if(statusSPI != E_OK) {

        return E_NOT_OK;
    }
    else
        return E_OK;

}




/**
 * @brief   sends command and data to the LTC daisy-chain.
 *
 * This is the core function to transmit data to the LTC6804 daisy-chain.
 * The data sent is:
 * COMMAND + 6 bytes (LTC1) + 6 bytes (LTC2) + 6 bytes (LTC3) + ... + 6 bytes (LTC{LTC_N_LTC})
 * A 2 byte command is sent with the corresponding PEC. Example: write configuration register (WRCFG).
 * THe command has to be set and then the function calculates the PEC automatically.
 * The function calculates the needed PEC to send the data to the daisy-chain. The sent data has the format:
 * 2 byte-COMMAND (2 bytes PEC) + 6 bytes (LTC1) (2 bytes PEC) + 6 bytes (LTC2) (2 bytes PEC) + 6 bytes (LTC3) (2 bytes PEC) + ... + 6 bytes (LTC{LTC_N_LTC}) (2 bytes PEC)
 *
 * The function returns 0. The only way to check if the transmission was successful is to read the results of the write operation.
 * (example: read configuration register after writing to it)
 *
 * @param   *Command                    command sent to the daisy-chain
 * @param   *DataBufferSPI_TX           data to be sent to the daisy-chain
 * @param   *DataBufferSPI_TX_with_PEC  data to sent to the daisy-chain, i.e. data to be sent + PEC (calculated by the function)
 *
 * @return                              E_OK if SPI transmission is OK, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_TX(uint8_t *Command, uint8_t *DataBufferSPI_TX, uint8_t *DataBufferSPI_TX_with_PEC) {

    uint16_t i = 0;
    STD_RETURN_TYPE_e statusSPI = E_NOT_OK;
    uint16_t PEC_result = 0;
    uint8_t PEC_Check[6] = {0x00,0x00,0x00,0x00,0x00,0x00};

    //DataBufferSPI_TX contains the data to send.
    //The transmission function calculates the needed PEC.
    //With it constructs DataBufferSPI_TX_with_PEC.
    //It corresponds to the data effectively received/sent.

    for(i=0;i<LTC_N_BYTES_FOR_DATA_TRANSMISSION;i++) {
        DataBufferSPI_TX_with_PEC[i]=0x00;
    //    DataBufferSPI_RX_with_PEC[i]=0x00;
    }


    DataBufferSPI_TX_with_PEC[0] = Command[0];
    DataBufferSPI_TX_with_PEC[1] = Command[1];
    DataBufferSPI_TX_with_PEC[2] = Command[2];
    DataBufferSPI_TX_with_PEC[3] = Command[3];

    // Calculate PEC of all data (1 PEC value for 6 bytes)
    for(i=0;i<LTC_N_LTC;i++) {

        PEC_Check[0]=DataBufferSPI_TX_with_PEC[4+i*8]=DataBufferSPI_TX[0+i*6];
        PEC_Check[1]=DataBufferSPI_TX_with_PEC[5+i*8]=DataBufferSPI_TX[1+i*6];
        PEC_Check[2]=DataBufferSPI_TX_with_PEC[6+i*8]=DataBufferSPI_TX[2+i*6];
        PEC_Check[3]=DataBufferSPI_TX_with_PEC[7+i*8]=DataBufferSPI_TX[3+i*6];
        PEC_Check[4]=DataBufferSPI_TX_with_PEC[8+i*8]=DataBufferSPI_TX[4+i*6];
        PEC_Check[5]=DataBufferSPI_TX_with_PEC[9+i*8]=DataBufferSPI_TX[5+i*6];

        PEC_result=LTC_pec15_calc(6, PEC_Check);
        DataBufferSPI_TX_with_PEC[10+i*8]=(uint8_t)((PEC_result>>8)&0xff);
        DataBufferSPI_TX_with_PEC[11+i*8]=(uint8_t)(PEC_result&0xff);

    }

    statusSPI = LTC_SendData(DataBufferSPI_TX_with_PEC);

    if(statusSPI != E_OK) {

        return E_NOT_OK;
    }
    else
        return E_OK;

}

/**
 * @brief   configures the data that will be sent to the LTC daisy-chain to configure multiplexer channels.
 *
 * This function does not sent the data to the multiplexer daisy-chain. This is done
 * by the function LTC_SetMuxChannel(), which calls LTC_SetMUXChCommand().
 *
 * @param   *DataBufferSPI_TX      data to be sent to the daisy-chain to configure the multiplexer channels
 * @param   mux                    multiplexer ID to be configured (0,1,2 or 3)
 * @param   channel                multiplexer channel to be configured (0 to 7)
 *
 * @return  void
 *
 */
static void LTC_SetMUXChCommand(uint8_t *DataBufferSPI_TX, uint8_t mux, uint8_t channel) {

    uint16_t i = 0;

    for(i=0;i<LTC_N_LTC;i++) {

        DataBufferSPI_TX[0+i*6]=0x69;        // 0x6 : LTC6804: ICOM START from Master
                                            // 9: LTC1380: D15..12 ()
        if (mux == 0) {
            DataBufferSPI_TX[1+i*6]=0x08;    // 0x0 : LTC1380: D11..8  (0,A1,A0,/W)
        }                                    // 8: LTC6804: Master NACK (FCOM)
        else if (mux == 1) {
            DataBufferSPI_TX[1+i*6]=0x28;
        }
        else if (mux == 2) {
            DataBufferSPI_TX[1+i*6]=0x48;
        }
        else if (mux == 3) {
            DataBufferSPI_TX[1+i*6]=0x68;
        }
        else {
            DataBufferSPI_TX[1+i*6]=0x08;
        }

        DataBufferSPI_TX[2+i*6]=0x00;        // 0x0 : LTC6804: ICOM Blank (Continue...)
                                            // 0: LTC1380: D7..4 (dummies)
        if (channel == 0) {
            DataBufferSPI_TX[3+i*6]=0x89;    // 0x8 : LTC1380: D3..D0 (EN,C2,C1,C0...enable output of channel)
        }                                    // 9: LTC6804: Master NACK+STOP (FCOM)
        else if (channel == 1) {
            DataBufferSPI_TX[3+i*6]=0x99;
        }
        else if (channel == 2) {
            DataBufferSPI_TX[3+i*6]=0xA9;
        }
        else if (channel == 3) {
            DataBufferSPI_TX[3+i*6]=0xB9;
        }
        else if (channel == 4) {
            DataBufferSPI_TX[3+i*6]=0xC9;
        }
        else if (channel == 5) {
            DataBufferSPI_TX[3+i*6]=0xD9;
        }
        else if (channel == 6) {
            DataBufferSPI_TX[3+i*6]=0xE9;
        }
        else if (channel == 7) {
            DataBufferSPI_TX[3+i*6]=0xF9;
        }
        else if (channel == 0xFF) {
            DataBufferSPI_TX[3+i*6]=0x09;
        }
        else {
            DataBufferSPI_TX[3+i*6]=0x09;
        }

        DataBufferSPI_TX[4+i*6]=0x10;        // 0x1 : ICOM-STOP
                                            // 0: dummy (Dn)
        DataBufferSPI_TX[5+i*6]=0x09;        // 0x0 : dummy (Dn)
                                            // 9: MASTER NACK + STOP (FCOM)
    }
}



/**
 * @brief   sends data to the LTC daisy-chain to configure multiplexer channels.
 *
 * This function calls the function LTC_SetMUXChCommand() to set the data.
 *
 * @param        *DataBufferSPI_TX              data to be sent to the daisy-chain to configure the multiplexer channels
 * @param        *DataBufferSPI_TX_with_PEC     data to be sent to the daisy-chain to configure the multiplexer channels, with PEC (calculated by the function)
 * @param        mux                    multiplexer ID to be configured (0,1,2 or 3)
 * @param        channel                multiplexer channel to be configured (0 to 7)
 *
 * @return       E_OK if SPI transmission is OK, E_NOT_OK otherwise
 */
static uint8_t LTC_SetMuxChannel(uint8_t *DataBufferSPI_TX, uint8_t *DataBufferSPI_TX_with_PEC, uint8_t mux, uint8_t channel ) {

    STD_RETURN_TYPE_e statusSPI = E_NOT_OK;

    //send WRCOMM to send I2C message to choose channel
    LTC_SetMUXChCommand(DataBufferSPI_TX,mux,channel);
    statusSPI = LTC_TX((uint8_t*)ltc_cmdWRCOMM, DataBufferSPI_TX, DataBufferSPI_TX_with_PEC);

    if(statusSPI != E_OK) {

        return E_NOT_OK;
    }
    else
        return E_OK;

}


/**
 * @brief   sends 72 clock pulses to the LTC daisy-chain.
 *
 * This function is used for the communication with the multiplexers via I2C on the GPIOs.
 * It send the command STCOMM to the LTC daisy-chain.
 *
 * @param   *DataBufferSPI_TX           data to be sent to the daisy-chain, set to 0xFF
 * @param   *DataBufferSPI_TX_with_PEC  data to be sent to the daisy-chain  with PEC (calculated by the function)
 *
 * @return  statusSPI                   E_OK if clock pulses were sent correctly by SPI, E_NOT_OK otherwise
 *
 */
static STD_RETURN_TYPE_e LTC_I2CClock(uint8_t *DataBufferSPI_TX, uint8_t *DataBufferSPI_TX_with_PEC) {

    uint16_t i = 0;

    STD_RETURN_TYPE_e statusSPI = E_NOT_OK;

    for(i=0;i<4+9;i++) {
        DataBufferSPI_TX_with_PEC[i]=0xFF;
    }

    DataBufferSPI_TX_with_PEC[0] = ltc_cmdSTCOMM[0];
    DataBufferSPI_TX_with_PEC[1] = ltc_cmdSTCOMM[1];
    DataBufferSPI_TX_with_PEC[2] = ltc_cmdSTCOMM[2];
    DataBufferSPI_TX_with_PEC[3] = ltc_cmdSTCOMM[3];

    statusSPI = LTC_SendI2CCmd(DataBufferSPI_TX_with_PEC);

    return statusSPI;


}

LTC_RETURN_TYPE_e LTC_Ctrl(LTC_TASK_TYPE_e LTC_Todo)
{
    LTC_STATEMACH_e ltcstate = LTC_STATEMACH_UNDEFINED;
    LTC_RETURN_TYPE_e retVal = LTC_REQUEST_PENDING;

    ltc_task_1ms_cnt++;

    if(LTC_GetStateRequest() == LTC_STATE_NO_REQUEST)
    {
        ltcstate = LTC_GetState();

        if(LTC_Todo == LTC_HAS_TO_MEASURE)
        {
            if(ltcstate == LTC_STATEMACH_UNINITIALIZED)
            {
                /* set initialization request*/
                retVal = LTC_SetStateRequest(LTC_STATE_INIT_REQUEST, LTC_ADCMODE_UNDEFINED, LTC_ADCMEAS_UNDEFINED, LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE);
                ltc_taskcycle=1;
            }
            else if(ltcstate == LTC_STATEMACH_IDLE)
            {
                /* set state requests for starting measurement cycles if LTC is in IDLE state*/

                ++ltc_taskcycle;

                switch(ltc_taskcycle)
                {

                case 2:
                    retVal = LTC_SetStateRequest(LTC_STATE_VOLTAGEMEASUREMENT_REQUEST,LTC_VOLTAGE_MEASUREMENT_MODE,LTC_ADCMEAS_ALLCHANNEL, LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE);
                    break;

                case 3:
                    retVal = LTC_SetStateRequest(LTC_STATE_READVOLTAGE_REQUEST,LTC_VOLTAGE_MEASUREMENT_MODE,LTC_ADCMEAS_ALLCHANNEL, LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE);
                    break;


                case 4:
                    LTC_SaveVoltages();
                    retVal = LTC_SetStateRequest(LTC_STATE_MUXMEASUREMENT_REQUEST,LTC_GPIO_MEASUREMENT_MODE,LTC_ADCMEAS_SINGLECHANNEL, LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE);
                    break;

                case 5:
                    if (ltc_muxcycle_finished==E_OK){
                        LTC_SaveTemperatures_SaveBalancingFeedback();
                        retVal = LTC_SetStateRequest(LTC_STATE_BALANCECONTROL_REQUEST,LTC_ADCMODE_NORMAL_DCP0,LTC_ADCMEAS_ALLCHANNEL, LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE);
                    }
                    break;

                case 6:
                    retVal = LTC_SetStateRequest(LTC_STATE_ALLGPIOMEASUREMENT_REQUEST,LTC_ADCMODE_NORMAL_DCP0,LTC_ADCMEAS_ALLCHANNEL, LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE);
                    break;

                case 7:
                    LTC_SaveAllGPIOs();
                    ltc_taskcycle=1;            // Restart measurement cycle
                    break;


                default:
                    ltc_taskcycle=1;
                    break;
                }
            }
        }

        else if(LTC_Todo == LTC_HAS_TO_REINIT)
        {
            if(ltcstate != LTC_STATEMACH_INITIALIZATION || ltcstate != LTC_STATEMACH_INITIALIZED)
            {
                ltc_taskcycle=1;
                retVal = LTC_SetStateRequest(LTC_STATE_REINIT_REQUEST,LTC_ADCMODE_UNDEFINED, LTC_ADCMEAS_UNDEFINED, LTC_NUMBER_OF_MUX_MEASUREMENTS_PER_CYCLE);
            }
        }

        else
        {
            retVal = LTC_ILLEGAL_TASK_TYPE;
        }
    }
    return retVal;
}



/**
 * @brief   gets the frequency of the SPI clock.
 *
 * This function reads the configuration from the SPI handle directly.
 *
 * @return    frequency of the SPI clock
 */
static uint32_t LTC_GetSPIClock(void) {

    uint32_t SPI_Clock = 0;

    if (LTC_SPI_INSTANCE == SPI2 || LTC_SPI_INSTANCE == SPI3) {
        // SPI2 and SPI3 are connected to APB1 (PCLK1)
        // The prescaler setup bits LTC_SPI_PRESCALER corresponds to the bits 5:3 in the SPI_CR1 register
        // Reference manual p.909
        // The shift by 3 puts the bits 5:3 to the first position
        // Division are made by powers of 2 which corresponds to shifting to the right
        // Then 0 corresponds to divide by 2, 1 corresponds to divide by 4... so 1 has to be added to the value of the configuration bits

        SPI_Clock = HAL_RCC_GetPCLK1Freq()>>( (LTC_SPI_PRESCALER>>3)+1 );
    }

    if (LTC_SPI_INSTANCE == SPI1 || LTC_SPI_INSTANCE == SPI4 || LTC_SPI_INSTANCE == SPI5 || LTC_SPI_INSTANCE == SPI6) {
        // SPI1, SPI4, SPI5 and SPI6 are connected to APB2 (PCLK2)
        // The prescaler setup bits LTC_SPI_PRESCALER corresponds to the bits 5:3 in the SPI_CR1 register
        // Reference manual p.909
        // The shift by 3 puts the bits 5:3 to the first position
        // Division are made by powers of 2 which corresponds to shifting to the right
        // Then 0 corresponds to divide by 2, 1 corresponds to divide by 4... so 1 has to be added to the value of the configuration bits

        SPI_Clock = HAL_RCC_GetPCLK2Freq()>>( (LTC_SPI_PRESCALER>>3)+1 );
    }

    return SPI_Clock;
}




/**
 * @brief   sets the transfer time needed to receive/send data with the LTC daisy-chain.
 *
 * This function gets the clock frequency and uses the number of LTCs in the daisy-chain.
 *
 * @return  void
 *
 */
static void LTC_SetTransferTimes(void) {

    uint32_t transferTime_us = 0;
    uint32_t SPI_Clock = 0;

    SPI_Clock = LTC_GetSPIClock();

    // Transmission of a command and data
    // Multiplication by 1000*1000 to get us
    transferTime_us = ((LTC_N_BYTES_FOR_DATA_TRANSMISSION)*8*1000*1000)/(SPI_Clock);
    transferTime_us = transferTime_us + SPI_WAKEUP_WAIT_TIME;
    ltc_state.commandDataTransferTime = (transferTime_us/1000)+1;

    // Transmission of a command
    // Multiplication by 1000*1000 to get us
    transferTime_us = ((4)*8*1000*1000)/(SPI_Clock);
    transferTime_us = transferTime_us + SPI_WAKEUP_WAIT_TIME;
    ltc_state.commandTransferTime = (transferTime_us/1000)+1;

    // Transmission of a command + 9 clocks
    // Multiplication by 1000*1000 to get us
    transferTime_us = ((4+9)*8*1000*1000)/(SPI_Clock);
    transferTime_us = transferTime_us + SPI_WAKEUP_WAIT_TIME;
    ltc_state.gpioClocksTransferTime = (transferTime_us/1000)+1;
}



/**
 * @brief   checks the state requests that are made.
 *
 * This function checks the validity of the state requests.
 * The resuls of the checked is returned immediately.
 *
 * @param   statereq    state request to be checked
 *
 * @return              result of the state request that was made, taken from LTC_RETURN_TYPE_e
 */
static LTC_RETURN_TYPE_e LTC_CheckStateRequest(LTC_STATE_REQUEST_e statereq) {

    if (ltc_state.statereq == LTC_STATE_NO_REQUEST) {

        //init only allowed from the uninitialized state
        if (statereq == LTC_STATE_INIT_REQUEST) {

            if (ltc_state.state==LTC_STATEMACH_UNINITIALIZED) {

                return LTC_OK;
            } else {

                return LTC_ALREADY_INITIALIZED;
            }
        }
        if (LTC_STATE_REINIT_REQUEST) {

            //re-init allowed from error states, too
            if (ltc_state.state==LTC_STATEMACH_IDLE) {

                 return LTC_OK;
            }
            else if ( (ltc_state.state==LTC_STATEMACH_ERROR_SPIFAILED) ||
                      (ltc_state.state==LTC_STATEMACH_ERROR_PECFAILED) ||
                      (ltc_state.state==LTC_STATEMACH_ERROR_MUXFAILED)
                    ) {

                 return LTC_OK_FROM_ERROR;
            } else {

                 return LTC_BUSY_OK;
            }
        }
        //other state request than re-init not allowed if there is an error
        if( (statereq == LTC_STATE_VOLTAGEMEASUREMENT_REQUEST) ||
            (statereq == LTC_STATE_READVOLTAGE_REQUEST) ||
            (statereq == LTC_STATE_MUXMEASUREMENT_REQUEST) ||
            (statereq == LTC_STATE_BALANCECONTROL_REQUEST) ||
            (statereq == LTC_STATE_IDLE_REQUEST) ||
            (statereq == LTC_STATE_ALLGPIOMEASUREMENT_REQUEST)
          ) {

            if (ltc_state.state==LTC_STATEMACH_IDLE) {

                 return LTC_OK;
            }
            else if (ltc_state.state==LTC_STATEMACH_ERROR_SPIFAILED) {

                 return LTC_SPI_ERROR;
            }
            else if (ltc_state.state==LTC_STATEMACH_ERROR_PECFAILED) {

                 return LTC_PEC_ERROR;
            }
            else if (ltc_state.state==LTC_STATEMACH_ERROR_MUXFAILED) {

                 return LTC_MUX_ERROR;
            } else {

                 return LTC_BUSY_OK;
            }
        } else {

            return LTC_ILLEGAL_REQUEST;
        }
    } else {

        return LTC_REQUEST_PENDING;
    }
}
