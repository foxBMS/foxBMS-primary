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
 * @file    eepr_cfg.c
 * @author  foxBMS Team
 * @date    23.02.2016 (date of creation)
 * @ingroup DRIVERS_CONF,EXT_PER
 * @prefix  EEPR
 *
 * @brief   Configuration for the driver for the storage in the EEPROM memory
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
#include "eepr_cfg.h"
#include "mcu.h"
#include "spi.h"
#include <string.h>
/*================== Macros and Definitions ===============================*/


/*================== Constant and Variable Definitions ====================*/

// FIXME actually you define this variable here (in config), but you use it in eepr.c, so if one will change its name, the "static part" of the driver has to be changed
// calibration data in eeprom
EEPR_CALIB_FRAME_s eepr_frame;

// default Calibration Data
const EEPR_CALIB_FRAME_s defaultcalibdata = {
    .Block.data.calib.soc.max            = 50.0,
    .Block.data.calib.soc.min            = 50.0,
    .Block.data.calib.soc.mean           = 50.0,
    .Block.data.calib.contactors_count.cont_switch_closed = {0,0,0},
    .Block.data.calib.contactors_count.cont_switch_opened = {0,0,0},
    .Block.data.calib.contactors_count.cont_switch_opened = {0,0,0},
    .Block.data.calib.u_cell_max         = 0,
    .Block.data.calib.u_cell_min         = 0,
    .Block.data.calib.u_temperature_max  = 0,
    .Block.data.calib.u_temperature_min  = 0,
    .Block.data.calib.current_max        = 0,
    .Block.data.calib.current_max        = 0,
    .Block.data.calib.operating_hours = { 0,0,0,0,0,0,0 },
};

// write buffer for calibration data in eeprom
struct_CALIB_DATA_s eepr_writebuffer;

// write buffer checksum
uint16_t eepr_writebufferChksum=0xFFFF;

uint8_t eepr_spi_rxbuf[EEPR_TXBUF_LENGTH];
uint16_t eepr_spi_rxoffset;

/*================== Function Prototypes ==================================*/
static uint16_t EEPR_BkpSramCalculateChksum(void);


/*================== Function Implementations =============================*/

/**
 * @brief   Receives data from the EEPROM via SPI
 *
 * @param   *buffer pointer where the data should be stored to
 * @param   length  length of the buffer
 *
 * @return  EEPR_OK if OK, otherwise EEPR_ERROR
 */
EEPR_RETURNTYPE_e SPI_ReceiveData(uint8_t* buffer, uint16_t length)
{
    EEPR_RETURNTYPE_e retVal = EEPR_ERROR;

    memcpy(buffer, eepr_spi_rxbuf + eepr_spi_rxoffset, length);
    if (spi_devices[1].State  ==  HAL_SPI_STATE_READY)
    {
        retVal = EEPR_OK;
    }
    else
    {
        retVal = EEPR_ERROR;
    }

    return retVal;
}

/**
 * @brief   Stores the data in the EEPROM via SPI
 *
 * @param  *data            pointer where the data is stored
 * @param   length          length of data
 * @param   receiveoffset   receive offset
 *
 * @return  EEPR_OK if okay, otherwise EEPR_ERROR
 */
EEPR_RETURNTYPE_e SPI_SendData(uint8_t* data, uint16_t length, uint16_t receiveoffset)
{

    EEPR_RETURNTYPE_e retVal = EEPR_ERROR;
    HAL_StatusTypeDef eepr_spi_halstate = HAL_ERROR;

    IO_WritePin(IO_PIN_MCU_0_DATA_STORAGE_EEPROM_SPI_NSS, IO_PIN_RESET); // FIXME use chip select/unselect functions fromSPI module!

    eepr_spi_halstate = HAL_SPI_TransmitReceive_IT(&spi_devices[1], data, eepr_spi_rxbuf, length);
    eepr_spi_rxoffset = receiveoffset;

    if(eepr_spi_halstate == HAL_OK)
        retVal = EEPR_OK;
    else
        retVal = EEPR_ERROR;

    return retVal;
}

uint16_t EEPR_CalcChecksum(uint8_t*dataptr, uint16_t byte_len) {
    uint16_t u16_chksum=0;

    for(;byte_len > 0;byte_len--)
        u16_chksum += *dataptr++;

    return (u16_chksum);
}


/**
 * @brief  calculates the 16-bit sum-of-bytes checksum over eepr_bkpsram_ch_1
 *         placed in backup SRAM
 *
 * @return 16 Bit Checksum
*/
static uint16_t EEPR_BkpSramCalculateChksum(void) {
    uint16_t retVal;
    retVal = EEPR_CalcChecksum((uint8_t*)(&bkpsram_ch_1),sizeof(bkpsram_ch_1)-4);
    return (retVal);
}


uint8_t EEPR_BkpSramCheckChksum(void) {
    uint8_t retVal = 0;

    if(bkpsram_ch_1.checksum !=  EEPR_BkpSramCalculateChksum())
        retVal = 0xFF;

    return (retVal);
}


void EEPR_BkpSramDefaultDataRecovery(void) {

    bkpsram_ch_1.nvsoc = defaultcalibdata.Block.data.calib.soc;

    bkpsram_ch_1.contactors_count = defaultcalibdata.Block.data.calib.contactors_count;

    bkpsram_ch_1.operating_hours = defaultcalibdata.Block.data.calib.operating_hours;

    bkpsram_ch_1.checksum = EEPR_CalcChecksum((uint8_t*)(&bkpsram_ch_1),sizeof(bkpsram_ch_1)-4);
}


void EEPR_BkpSramDataRecovery(void) {

    bkpsram_ch_1.nvsoc = eepr_frame.Block.data.calib.soc;

    bkpsram_ch_1.contactors_count = eepr_frame.Block.data.calib.contactors_count;

    bkpsram_ch_1.operating_hours = eepr_frame.Block.data.calib.operating_hours;

    bkpsram_ch_1.checksum = EEPR_CalcChecksum((uint8_t*)(&bkpsram_ch_1),sizeof(bkpsram_ch_1)-4);
}


void EEPR_UpdateEepromData(void) {

    eepr_writebuffer.calib.soc = bkpsram_ch_1.nvsoc;

    eepr_writebuffer.calib.contactors_count = bkpsram_ch_1.contactors_count;

    eepr_writebuffer.calib.operating_hours = bkpsram_ch_1.operating_hours;

    eepr_writebufferChksum = EEPR_CalcChecksum((uint8_t*)(&eepr_writebuffer), sizeof(struct_CALIB_DATA_s));
}


void EEPR_Set_nvsoc(SOX_SOC_s* ptr) {
    uint32_t interrupt_status = 0;

    /* Disable interrupts */
    interrupt_status=MCU_DisableINT();

    /* update bkpsram values */
    bkpsram_ch_1.nvsoc = *ptr;

    /* calculate checksum*/
    bkpsram_ch_1.checksum = EEPR_CalcChecksum((uint8_t*)(&bkpsram_ch_1),sizeof(bkpsram_ch_1)-4);

    /* Enable interrupts */
    MCU_RestoreINT(interrupt_status);
}


SOX_SOC_s EEPR_Get_nvsoc(void) {
    return (bkpsram_ch_1.nvsoc);
}

