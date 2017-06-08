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
 * @file    database_cfg.h
 * @author  foxBMS Team
 * @date    18.08.2015 (date of creation)
 * @ingroup ENGINE_CONF
 * @prefix  DATA
 *
 * @brief   Database configuration header
 *
 * Provides interfaces to database configuration
 *
 */

#ifndef DATABASE_CFG_H_
#define DATABASE_CFG_H_

/*================== Includes =============================================*/
#include "batterysystem_cfg.h"

/*================== Macros and Definitions ===============================*/

/**
 * @brief maximum amount of data block
 *
 * this value is extendible but limitation is done due to RAM consumption and performance
 */
#define DATA_MAX_BLOCK_NR                11        /* max 11 Blocks actually supported*/

/**
 * @brief data block identification number
 */
typedef enum {
    DATA_BLOCK_1        = 0,
    DATA_BLOCK_2        = 1,
    DATA_BLOCK_3        = 2,
    DATA_BLOCK_4        = 3,
    DATA_BLOCK_5        = 4,
    DATA_BLOCK_6        = 5,
    DATA_BLOCK_7        = 6,
    DATA_BLOCK_8        = 7,
    DATA_BLOCK_9        = 8,
    DATA_BLOCK_10       = 9,
    DATA_BLOCK_11       = 10,
    DATA_BLOCK_MAX      = DATA_MAX_BLOCK_NR,
} DATA_BLOCK_ID_TYPE_e;


/**
 * @brief data block access types
 *
 * read or write access types
 */
typedef enum
{
    WRITE_ACCESS    = 0,    /*!< write access to data block   */
    READ_ACCESS     = 1,    /*!< read access to data block   */
}DATA_BLOCK_ACCESS_TYPE_e;

/**
 * @brief data block consistency types
 *
 * recommendation: use single buffer for small data (e.g.,one variable) and less concurrent read and write accesses
 */
typedef enum {
    // Init-Sequence
    SINGLE_BUFFERING    = 1,    /*!< single buffering   */
    DOUBLE_BUFFERING    = 2,    /*!< double buffering   */
 // TRIPLEBUFFERING     = 3,    /* actually not supported*/
}DATA_BLOCK_CONSISTENCY_TYPE_e;

/**
 * configuration struct of database channel (data block)
 */
typedef struct {
    void *blockptr;
    uint16_t datalength;
    DATA_BLOCK_CONSISTENCY_TYPE_e buffertype;
} DATA_BASE_HEADER_s;

/**
 * configuration struct of database device
 */
typedef struct {
    uint8_t nr_of_blockheader;
    DATA_BASE_HEADER_s *blockheaderptr;
} DATA_BASE_HEADER_DEV_s;


/*================== Macros and Definitions [USER CONFIGURATION] =============*/
// FIXME comments doxygen, is comment necessary?
/*Macros and Definitions for User Configuration*/
#define     DATA_BLOCK_ID_CELLVOLTAGE                   DATA_BLOCK_1
#define     DATA_BLOCK_ID_CELLTEMPERATURE               DATA_BLOCK_2
#define     DATA_BLOCK_ID_SOX                           DATA_BLOCK_3
#define     DATA_BLOCK_ID_BALANCING_CONTROL_VALUES      DATA_BLOCK_4
#define     DATA_BLOCK_ID_BALANCING_FEEDBACK_VALUES     DATA_BLOCK_5
#define     DATA_BLOCK_ID_CURRENT                       DATA_BLOCK_6
#define     DATA_BLOCK_ID_ADC                           DATA_BLOCK_7
#define     DATA_BLOCK_ID_STATEREQUEST                  DATA_BLOCK_8
#define     DATA_BLOCK_ID_CANERRORSIG                   DATA_BLOCK_9
#define     DATA_BLOCK_ID_MINMAX                        DATA_BLOCK_10
#define     DATA_BLOCK_ID_ISOGUARD                      DATA_BLOCK_11


/**
 * data block struct of cell voltage
 */
typedef struct {
    uint16_t voltage[BS_NR_OF_BAT_CELLS];       /*!< unit: mV                                   */
    uint32_t previous_timestamp;                /*!< timestamp of last database entry           */
    uint32_t timestamp;                         /*!< timestamp of database entry                */
    uint8_t state;                              /*!< for future use                             */
} DATA_BLOCK_CELLVOLTAGE_s;

/**
 * typedef of data block structure of cell temperatures
 */
typedef struct {
    int16_t temperature[BS_NR_OF_TEMP_SENSORS];             /*!< unit: degree Celsius                       */
    uint32_t previous_timestamp;                            /*!< timestamp of last database entry           */
    uint32_t timestamp;                                     /*!< timestamp of database entry                */
    uint8_t state;                                          /*!< for future use                             */
} DATA_BLOCK_CELLTEMPERATURE_s;

/**
 * data block struct of sox
 */
typedef struct {
    float soc_mean;                     /*!< 0.0 <= soc_mean <= 100.0           */
    float soc_min;                      /*!< 0.0 <= soc_min <= 100.0            */
    float soc_max;                      /*!< 0.0 <= soc_max <= 100.0            */
    uint32_t previous_timestamp;        /*!< timestamp of last database entry   */
    uint32_t timestamp;                 /*!< timestamp of database entry        */
    uint8_t state;                      /*!<                                    */
    float sof_continuous_charge;        /*!<                                    */
    float sof_continuous_discharge;     /*!<                                    */
    float sof_peak_charge;              /*!<                                    */
    float sof_peak_discharge;           /*!<                                    */
} DATA_BLOCK_SOX_s;


/*  data structure declaration of DATA_BLOCK_BALANCING_CONTROL */
typedef struct
{
    uint16_t value[BS_NR_OF_BAT_CELLS];    /*!< */
    uint32_t previous_timestamp;        /*!< timestamp of last database entry           */
    uint32_t timestamp;                 /*!< timestamp of database entry                */
    uint8_t enable_balancing;           /*!< Switch for enabling/disabling balancing    */
    uint8_t threshold;                  /*!< balancing threshold in mV                  */
    uint8_t state;                      /*!< for future use                             */
} DATA_BLOCK_BALANCING_CONTROL_s;

/**
 * data block struct of cell balancing feedback
 */

typedef struct {
    uint16_t value[BS_NR_OF_BAT_CELLS];    /*!< unit: mV (opto-coupler output)     */
    uint32_t previous_timestamp;        /*!< timestamp of last database entry   */
    uint32_t timestamp;                 /*!< timestamp of database entry        */
    uint8_t state;                      /*!< for future use                     */
} DATA_BLOCK_BALANCING_FEEDBACK_s;


/**
 * typedef of data block structure of current measurement
 */
typedef struct {
    float current;                                      /*!< unit: to be defined                */
    float voltage[BS_NR_OF_VOLTAGES_FROM_CURRENT_SENSOR];  /*!<                                    */
    uint32_t previous_timestamp;                        /*!< timestamp of last database entry   */
    uint32_t timestamp;                                 /*!< timestamp of database entry        */
    uint8_t state_current;
    uint8_t state_voltage;
} DATA_BLOCK_CURRENT_s;


/**
 * data block struct of ADC
 */
typedef struct {
    float vbat; //unit: to be defined
    uint32_t vbat_previous_timestamp;           /*!< timestamp of last database entry of vbat           */
    uint32_t vbat_timestamp;                    /*!< timestamp of database entry of vbat                */
    float temperature;                          /*!<                                                    */
    uint32_t temperature_previous_timestamp;    /*!< timestamp of last database entry of temperature    */
    uint32_t temperature_timestamp;             /*!< timestamp of database entry of temperature         */
    uint8_t state_vbat;                         /*!<                                                    */
    uint8_t state_temperature;                  /*!<                                                    */
} DATA_BLOCK_ADC_s;


/**
 * data block struct of can state request
 */

typedef struct {
    uint8_t state_request;
    uint8_t previous_state_request;
    uint8_t state_request_pending;
    uint32_t timestamp;             /*!< timestamp of database entry        */
    uint32_t previous_timestamp;    /*!< timestamp of last database entry   */
    uint8_t state;
} DATA_BLOCK_STATEREQUEST_s;

/**
 * data block struct of can error signal
 */
typedef struct
{
    uint8_t error_cantiming;
    uint8_t error_highvolt;
    uint8_t error_hightemp;
    uint8_t error_lowvolt;
    uint8_t error_lowtemp;
    uint8_t error_overcurrent_charge;
    uint8_t error_overcurrent_discharge;

    uint8_t contactor_state_control;
    uint8_t contactor_state_feedback;
    uint8_t contactor_error;
    uint8_t interlock_open;

    uint16_t over_under_values;

    uint32_t timestamp;                /*!< timestamp of database entry        */
    uint32_t previous_timestamp;       /*!< timestamp of last database entry   */
    uint8_t state;
} DATA_BLOCK_CANERRORSIG_s;

/**
 * data block struct of LTC minimum and maximum values
 */
typedef struct {
    uint32_t voltage_mean;
    uint16_t voltage_min;
    uint16_t voltage_module_number_min;
    uint16_t voltage_cell_number_min;
    uint16_t previous_voltage_min;
    uint16_t voltage_max;
    uint16_t voltage_module_number_max;
    uint16_t voltage_cell_number_max;
    uint16_t previous_voltage_max;
    int32_t temperature_mean;
    int16_t temperature_min;
    uint16_t temperature_module_number_min;
    uint16_t temperature_sensor_number_min;
    int16_t temperature_max;
    uint16_t temperature_module_number_max;
    uint16_t temperature_sensor_number_max;
    uint8_t state;
    uint32_t timestamp;             /*!< timestamp of database entry                                        */
    uint32_t previous_timestamp;    /*!< timestamp of last database entry                                   */
} DATA_BLOCK_MINMAX_s;


/**
 * data block struct of isometer measurement
 */
typedef struct {
    uint8_t valid;                  /*!< 0 -> valid, 1 -> resistance unreliable                             */
    uint8_t state;                  /*!< 0 -> resistance/measurement OK , 1 -> resistance too low or error  */
    uint8_t resistance;             /*!< in intervals (max 3bit)                                            */
    uint32_t timestamp;             /*!< timestamp of database entry                                        */
    uint32_t previous_timestamp;    /*!< timestamp of last database entry                                   */
} DATA_BLOCK_ISOMETER_s;

/*================== Constant and Variable Definitions ====================*/

/**
 * @brief device configuration of database
 *
 * all attributes of device configuration are listed here (pointer to channel list, number of channels)
 */
extern DATA_BASE_HEADER_DEV_s data_base_dev;

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

#endif /* DATABASE_CFG_H_ */
