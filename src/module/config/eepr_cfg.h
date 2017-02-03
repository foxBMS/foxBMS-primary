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
 * @file    eepr_cfg.h
 * @author  foxBMS Team
 * @date    23.02.2016
 * @ingroup DRIVERS_CONF,EXT_PER
 * @prefix  EEPR
 *
 * @brief Header for the configuration for the driver for the storage in the EEPROM memory.
 *
 * Header of the configuration file for EEPRom module.
 *
 */
#ifndef EEPR_CFG_H_
#define EEPR_CFG_H_
/*================== Includes =============================================*/
#include "general.h"
#include "bkpsram_cfg.h"

#include "sox.h"
#include "contactor.h"

/*================== Macros and Definitions ===============================*/
                                                                   // 60 for all entries in calib but DIAG_CONTACTOR, + 3 to round up to %4 bytes
// FIXME is it better to use sizeof operator? (sizeof(calib)+3)/4 or ((60+(2byte + (6byte * NR_OF_CONTACTORS)))+3)/4
#define EEPR_CALIB_EEPR_DATA_BLOCK_U32LENGTH    20        // 20*4 Byte

#define EEPR_TXBUF_LENGTH            (sizeof(EEPR_CALIB_FRAME_s) + 16)	/* data + command + tolerance*/

#define EEPR_ReceiveData(bufferptr,length)   SPI_ReceiveData(bufferptr,length)
#define EEPR_SendData(bufferptr,length,receiveoffset)   SPI_SendData(bufferptr,length,receiveoffset)

/**
 * symbolic names for the EEPROM return types
 */
typedef enum {
    EEPR_OK      = 0,  /*!< error not occured or occured but threshold not reached    */
    EEPR_BUSY    = 1,  /*!< EEPROM driver busy */
    EEPR_ERROR   = 2,  /*!< error occurred */
} EEPR_RETURNTYPE_e;


// FIXME give reason for alignment and use of dummy bytes.
/**
 * struct to save combined calibration data
 * state of charge voltages,
 * dummy,
 * maximum cell voltage,
 * minimum cell voltage,
 * maximum temperature,
 * minimum temperature,
 * dummy,
 * dummy,
 * maximum current,
 * minimum current,
 * dummy,
 * dummy,
 * number of opening/closing events of the contactors,
 * dummy,
 * ...
 */
typedef union {
    uint32_t    index[EEPR_CALIB_EEPR_DATA_BLOCK_U32LENGTH];    // 4*20 = 80Byte
    struct {
        SOX_SOC_s       soc;                /* 12byte: 3 * float(32bit) */
        uint32_t        dummy_0;

        uint16_t        u_cell_max;
        uint16_t        u_cell_min;
        uint16_t        u_temperature_max;
        uint16_t        u_temperature_min;
        uint32_t        dummy_1_2;
        uint32_t        dummy_1_3;

        float        current_max;
        float        current_min;
        uint32_t        dummy_2_2;
        uint32_t        dummy_2_3;

        DIAG_CONTACTOR_s  contactors_count; // 20byte: 2byte + (6byte * NR_OF_CONTACTORS)
        uint32_t        dummy_4_1;
        uint32_t        dummy_4_2;
        uint32_t        dummy_4_3;         //   80Byte
    }calib;
} struct_CALIB_DATA_s;


/**
 * struct to save eeprom data
 *  calibration data,
 *  checksum of calibration data
 */
typedef union {
    struct {
        struct_CALIB_DATA_s data;
        uint16_t chksum;            // checksum of eeprom data
    } Block;                        // 80 + 2 Byte checksum -> 82Byte
    unsigned int U[(EEPR_CALIB_EEPR_DATA_BLOCK_U32LENGTH +1)];    // 84Byte
} EEPR_CALIB_FRAME_s;




/*================== Constant and Variable Definitions ====================*/
/**
 * data receive interface to SPI Unit
 */
extern EEPR_RETURNTYPE_e SPI_ReceiveData(uint8_t* buffer, uint16_t length);

/**
 * data send interface to SPI Unit
 */
extern EEPR_RETURNTYPE_e SPI_SendData(uint8_t* data, uint16_t length, uint16_t receiveoffset);
/**
 * calibration data in eeprom
 */
extern EEPR_CALIB_FRAME_s eepr_frame;

/**
 * default calibration data
 */
extern const EEPR_CALIB_FRAME_s defaultcalibdata;

/**
 * write buffer for calibration data in eeprom
 */
extern struct_CALIB_DATA_s eepr_writebuffer;

/**
 * write buffer checksum
 */
extern uint16_t eepr_writebufferChksum;

/*================== Function Prototypes ==================================*/

// FIXME bad confusing use of EEPR in EEPR module and BKPSRAM module. maybe move this checksum to utils or similar and use define here?

/**
 * @fn      U16 EEPR_CalcChecksum(U8 *dataptr, U16 byte_len)
 * @brief   calculates the 16-bit sum-of-bytes checksum over byte_len of bytes by starting at *dataptr
 *
 * @param   dataptr   start address for calculation
 * @param   byte_len  number of bytes for calculation
 * @return  16 Bit Checksum
*/
extern uint16_t EEPR_CalcChecksum(uint8_t *dataptr, uint16_t byte_len);

/**
 * @brief  checks if the bkpsram_ch_1 checksum saved in backup SRAM
 *         equals the data saved in eeprom
 *
 * @return 0 if checksum is OK, otherwise 0xFF
*/
extern uint8_t  EEPR_BkpSramCheckChksum(void);

/**
 * @brief  writes the default calibration data into bkpsram_ch_1
 *         in the backup SRAM
 *
 * @return none
*/
extern void EEPR_BkpSramDefaultDataRecovery(void);

/**
 * @brief  writes the data stored/saved in EEPROM into the backup SRAM
 *
 * @return none
*/
extern void EEPR_BkpSramDataRecovery(void);

/**
 * @brief  Updates the EEPROM data with the data stored in the backup SRAM
 *
 * @return none
*/
extern void EEPR_UpdateEepromData(void);

/**
 * @brief  Sets the soc data saved in the backup SRAM
 *
 * @param  ptr pointer where the soc data is stored
 * @return none
*/
extern void EEPR_Set_nvsoc(SOX_SOC_s* ptr);

/**
 * @brief  Gets the soc data saved in the backup SRAM
 *
 * @return state of charge
*/
extern SOX_SOC_s EEPR_Get_nvsoc(void);

/*================== Function Implementations =============================*/


#endif /* EEPR_CFG_H_ */
