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
#include "os.h"
#include "eepr.h"
/*================== Macros and Definitions ===============================*/

/**
 * EEPR_VERSIONNUMBERMAYOR
 *
 * A change of major number usually indicates a change of memory layout in the eeprom
 * Any change of data alignment could lead to data loss if not handled by a dedicated function
 *
 * */
#define EEPR_VERSIONNUMBERMAYOR             0

#define EEPR_VERSIONNUMBERMINOR             1

#define EEPR_HEADERPATTERN                  0xAA551234

/*================== Constant and Variable Definitions ====================*/


/**
 * Version number of EEPR Software
 */
EEPR_HEADER_s eepr_header = {
        .versionnumbermajor = EEPR_VERSIONNUMBERMAYOR,
        .versionnumberminor = EEPR_VERSIONNUMBERMINOR,
        .eepr_headerpattern = EEPR_HEADERPATTERN,
        .dummy2 = 0,
        .dummy3 = 0,
        .chksum = 0
};

EEPR_HEADER_s eepr_header_buffer;

const EEPR_CALIB_STATISTICS_s defaultcalibstatistics = {
        .u_cell_max = 0,
        .u_cell_min = 0,
        .u_temperature_max = 0,
        .u_temperature_min = 0,
        .current_max = 0,
        .current_min = 0,
};


EEPR_CH_CFG_s eepr_ch_cfg[EEPR_CHANNEL_MAX_NR] = {
/* Hardware Protected Channels (last quarter of address area)  */
        /*  EEPROM CHANNELS: HW/SW-SYSTEM */
        {0x3000, sizeof(EEPR_HEADER_s),           EEPR_CH_HEADER,          0x3000 + sizeof(EEPR_HEADER_s) - 4,           EEPR_SW_WRITE_UNPROTECTED, (uint8_t*)&eepr_header_buffer},
//      {0x3020, ...},
        {0x3F00, 0x80,                            EEPR_BOARD_INFO,         0x3F00 + 0x80 - 4,                             EEPR_SW_WRITE_PROTECTED,   (NULL_PTR)},
        {0x3F80, 0x80,                            EEPR_HARDWARE_INFO,      0x3F80 + 0x80 - 4,                             EEPR_SW_WRITE_PROTECTED,   (NULL_PTR)},
/* Channels placed in Hardware Protection disabled address area  */
        /*  EEPROM CHANNELS: APPLICATION */
//      {0x0000, 00x54,    reserved to maintain previous formatted data,   0x0080 + sizeof(BKPSRAM_CH_NVSOC_s) - 4,      EEPR_SW_WRITE_UNPROTECTED, (uint8_t*)&bkpsram_nvsoc },
        {0x0080, sizeof(BKPSRAM_CH_OP_HOURS_s),   EEPR_CH_OPERATING_HOURS, 0x0080 + sizeof(BKPSRAM_CH_OP_HOURS_s) - 4,   EEPR_SW_WRITE_UNPROTECTED, (uint8_t*)&bkpsram_operating_hours},
        {0x0090, sizeof(BKPSRAM_CH_NVSOC_s),      EEPR_CH_NVSOC,           0x0090 + sizeof(BKPSRAM_CH_NVSOC_s) - 4,      EEPR_SW_WRITE_UNPROTECTED, (uint8_t*)&bkpsram_nvsoc },
        {0x00B0, sizeof(BKPSRAM_CH_CONT_COUNT_s), EEPR_CH_CONTACTOR,       0x00B0 + sizeof(BKPSRAM_CH_CONT_COUNT_s) - 4, EEPR_SW_WRITE_UNPROTECTED, (uint8_t*)&bkpsram_contactors_count},
        {0x00F0, sizeof(EEPR_CALIB_STATISTICS_s), EEPR_CH_STATISTICS,      0x00F0 + sizeof(EEPR_CALIB_STATISTICS_s) - 4, EEPR_SW_WRITE_UNPROTECTED, (NULL_PTR)},
        // FREE EEPRROMS CHANNELS (for future use)
        {0x0110, 0x70,                            EEPR_CH_USER_DATA,       0x0110 + 0x70 - 4,                            EEPR_SW_WRITE_UNPROTECTED, (NULL_PTR)},
//      {0x0180, ...},
};

/* In case of compile errors in the following dummy-declarations,
 *  - check configuration in eepr_ch_cfg[]
 *  - modify the following dummy-declarations according to data lenght of each channel
 *  - modify (increment) the eeprom version versionnumbermajor
 *  - backup and recover the data in eeprom
 *
 * Note: A modification of channel address, data length or data structure will lead to data loss if data transfer
 * is not handled (e.g. manually or automatically in EEPR_FormatCheck() )
 *
 * */
extern uint8_t compiler_throw_an_error_1[(sizeof(EEPR_HEADER_s) == 0x20)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!
extern uint8_t compiler_throw_an_error_2[(0x80 == 0x80)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!
extern uint8_t compiler_throw_an_error_3[(0x80 == 0x80)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!
extern uint8_t compiler_throw_an_error_4[(sizeof(BKPSRAM_CH_OP_HOURS_s) == 0x10)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!
extern uint8_t compiler_throw_an_error_5[(sizeof(BKPSRAM_CH_NVSOC_s) == 0x20)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!
extern uint8_t compiler_throw_an_error_6[(sizeof(BKPSRAM_CH_CONT_COUNT_s) == 0x40)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!
extern uint8_t compiler_throw_an_error_7[(sizeof(EEPR_CALIB_STATISTICS_s) == 0x20)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!
extern uint8_t compiler_throw_an_error_8[(0x70 == 0x70)?1:-1]; // EEPROM FORMAT ERROR! Change of data size. Please note comment above!!!



// write buffer for calibration data in eeprom
uint8_t eepr_WR_RD_buffer[EEPR_CH_MAXLENGTH];


// write buffer checksum
uint16_t eepr_WR_RD_Chksum=0xFFFF;

uint8_t eepr_spi_rxbuf[EEPR_TXBUF_LENGTH];
uint16_t eepr_spi_rxoffset;

uint16_t timeout_cnt = 0;

/** Dirty Flag indicates a modification of data in BKPSRAM for the corresponding channel. Thus Eeprom and BKPSRAM data are not consistent */
uint8_t MEM_BKP_SRAM eepr_ch_dirtyflag[EEPR_CHANNEL_MAX_NR];

/** Buffer used for eeprom reads data when double-buffering with  BKPSRAM is not used */
uint8_t MEM_BKP_SRAM eepr_bkpsram_buffer[EEPR_CH_MAXLENGTH];

uint8_t cnt;    //counter variable
/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/
void EEPR_delete_bkpsrambuffer(EEPR_CHANNEL_ID_TYPE_e eepr_channel){
    uint16_t counter;
    for(counter = 0; counter < eepr_ch_cfg[eepr_channel].length; counter++){
        eepr_bkpsram_buffer[counter] = 0xFF;
    }
}

void EEPR_SetChDirtyFlag(EEPR_CHANNEL_ID_TYPE_e eepr_channel){
    eepr_ch_dirtyflag[eepr_channel] = 0;
}

void EEPR_RemoveChDirtyFlag(EEPR_CHANNEL_ID_TYPE_e eepr_channel){
    eepr_ch_dirtyflag[eepr_channel] = 1;
}

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

uint16_t EEPR_CalcChecksum(uint8_t*dataptr, uint16_t byte_len)
{
    uint16_t u16_chksum=0;

    for(;byte_len > 0;byte_len--)
        u16_chksum += *dataptr++;

    return (u16_chksum);
}




STD_RETURN_TYPE_e EEPR_BkpSramCheckChksum(EEPR_CHANNEL_ID_TYPE_e eepr_channel, uint8_t *ptr)
{
    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    uint8_t*dataptr = eepr_ch_cfg[eepr_channel].bkpsramptr;
    uint16_t byte_len = eepr_ch_cfg[eepr_channel].length;

    if(dataptr == NULL_PTR)
    {
        if(ptr != NULL_PTR){
            dataptr = ptr;
        }else{
            retVal = E_NOT_OK;
        }
    }
    else if(EEPR_CalcChecksum(dataptr, byte_len - 4) == *((uint32_t*)(dataptr + byte_len - 4)))
    {
        retVal = E_OK;  // checksum ok
    }

    return (retVal);
}

void EEPR_SetDefaultValue(EEPR_CHANNEL_ID_TYPE_e eepr_channel)
{
    switch(eepr_channel)
    {
        case EEPR_CH_NVSOC:
            bkpsram_nvsoc.data = default_nvsoc.data;
            bkpsram_nvsoc.checksum = EEPR_CalcChecksum((uint8_t*)(&bkpsram_nvsoc),sizeof(bkpsram_nvsoc)-4);
            EEPR_SetChDirtyFlag(EEPR_CH_NVSOC);
            break;

        case EEPR_CH_CONTACTOR:
            bkpsram_contactors_count.data = default_contactors_count.data;
            bkpsram_contactors_count.checksum = EEPR_CalcChecksum((uint8_t*)(&bkpsram_contactors_count),sizeof(bkpsram_contactors_count)-4);
            EEPR_SetChDirtyFlag(EEPR_CH_CONTACTOR);
            break;

        case EEPR_CH_OPERATING_HOURS:
            bkpsram_operating_hours.data = default_operating_hours.data;
            bkpsram_operating_hours.checksum = EEPR_CalcChecksum((uint8_t*)(&bkpsram_operating_hours),sizeof(bkpsram_operating_hours)-4);
            EEPR_SetChDirtyFlag(EEPR_CH_OPERATING_HOURS);
            break;

        default:
            break;
    }
}

/**
 * @brief  copys the data of one channel from eeprom into the bkpsrams
 *
 * @param  eepr_channel the channel that will be copied
 * @return 0 if everything worked correctly, error bit if something failed
*/
EEPR_ERRORTYPES_e EEPR_ReadChannelData(EEPR_CHANNEL_ID_TYPE_e eepr_channel){
    timeout_cnt = 0;
    EEPR_ERRORTYPES_e retval = EEPR_NO_ERROR;
    uint8_t*dataptr = eepr_ch_cfg[eepr_channel].bkpsramptr;
    uint16_t maxtime = EEPR_GetChReadTime(eepr_channel);

    //@FIXME check dataptr== NULL_PTR
    if(EEPR_SetStateRequest(EEPR_READMEMORY,eepr_channel, dataptr) == EEPR_OK){
        EEPR_Trigger();
        while (EEPR_GetState() != EEPR_IDLE){
            if (timeout_cnt > maxtime) {
                retval = EEPR_ERR_RD;     // possible reasons: SPI busy, eeprom hardware in busy state or defect
                break;
            }
            timeout_cnt++;
            EEPR_Trigger();
            osDelay(1);             // wait 1ms for next call of eeprom trigger
        }
    }else
        retval = EEPR_ERR_RD;
    if(retval == EEPR_NO_ERROR){
        EEPR_RemoveChDirtyFlag(eepr_channel);
    }
    return retval;
}

/**
 * @brief  copys the data of one channel from bkpsram into the eeprom
 *
 * @param  eepr_channel the channel that will be copied
 * @return 0 if everything worked correctly, error bit if something failed
*/
EEPR_ERRORTYPES_e EEPR_BackupChannelData2Eepr(EEPR_CHANNEL_ID_TYPE_e eepr_channel){
    uint8_t*dataptr = eepr_ch_cfg[eepr_channel].bkpsramptr;
    EEPR_ERRORTYPES_e retval = EEPR_NO_ERROR;
    timeout_cnt = 0;
    uint16_t maxtime = EEPR_GetChWriteTime(eepr_channel);

    //@FIXME check dataptr== NULL_PTR
    if(EEPR_SetStateRequest(EEPR_WRITEMEMORY, eepr_channel, dataptr) == EEPR_OK){
        EEPR_Trigger();
        while (EEPR_GetState() != EEPR_IDLE)
        {
            if (timeout_cnt > maxtime) {
                retval |= EEPR_ERR_WR;    // possible reasons: SPI busy, eeprom hardware in busy state or defect
                break;
            }
            timeout_cnt++;
            EEPR_Trigger();
            osDelay(1);             // wait 1ms for next call of eeprom trigger
        }
    }else
        retval = EEPR_ERR_WR;
    if(retval == EEPR_NO_ERROR){
        EEPR_RemoveChDirtyFlag(eepr_channel);
    }
   return retval;
}

/**
 * @brief  handles the data exchange between eeprom and bkpsram
 *          checks if the bkpsram data are valid.
 *          If so, and a dirty flag is set, the function saves the data into eeprom
 *          If not, the function recovers the bkpsram data backed up from the eeprom
 *
 * @param  ptr pointer where the channel data are stored
 * @return none
*/
EEPR_ERRORTYPES_e EEPR_RefreshChannelData(EEPR_CHANNEL_ID_TYPE_e eepr_channel){
    EEPR_ERRORTYPES_e retval = 0;
    uint8_t*dataptr = eepr_ch_cfg[eepr_channel].bkpsramptr;
    uint16_t byte_len = eepr_ch_cfg[eepr_channel].length;

    //if no bkpsramptr is defined in channel, use a temporary buffer
    if(dataptr==NULL_PTR)
    {
        dataptr = eepr_bkpsram_buffer;
    }
    /* verify the checksum of the Backup-SRAM instance */
    if(EEPR_CalcChecksum(dataptr, byte_len - 4) == *((uint32_t*)(dataptr + byte_len - 4)))
    {   // checksum ok, if dirty flag is set actualize eeprom data (copy channel data bkpsram -> eeprom )
        if(eepr_ch_dirtyflag[eepr_channel] == 0){
            retval = EEPR_BackupChannelData2Eepr(eepr_channel);
        }
    }
    else
    {   // checksum corrupt, recover bkpsram by eeprom data (copy channel data eeprom -> bkpsram)
        retval = EEPR_ReadChannelData(eepr_channel);
    }

    if(dataptr == eepr_bkpsram_buffer){ //clear temporary buffer for next usage (invalidate checksum)
        EEPR_delete_bkpsrambuffer(eepr_channel);
    }

    return retval;
}

uint8_t EEPR_FormatCheck(void){
    uint8_t retval=1;
    //EEPR_ERRORTYPES_e errtype;

    if(eepr_header_buffer.eepr_headerpattern == EEPR_HEADERPATTERN)
    {
        if(eepr_header_buffer.versionnumbermajor == EEPR_VERSIONNUMBERMAYOR)
            retval = 0;
        else
            retval = 1; // FIXME: handle data transfer (update) in case of new version with different data format
    }
    else
    {
        retval = 1;
    }
    return retval;
}

EEPR_ERRORTYPES_e EEPR_InitChannelData(void)
{
    EEPR_ERRORTYPES_e retval = EEPR_NO_ERROR;
    EEPR_ERRORTYPES_e errtype = EEPR_NO_ERROR;

    /* Read eeprom header check data format */
    errtype = EEPR_ReadChannelData(EEPR_CH_HEADER);
    if(errtype == EEPR_NO_ERROR)
    {
        EEPR_FormatCheck();
    }
    else
    {
        // @FIXME_ read a second time?
        // very first time of eeprom initialization
        eepr_header.chksum = EEPR_CalcChecksum((uint8_t*)&eepr_header, sizeof(eepr_header)-4);
        eepr_header_buffer = eepr_header;
        errtype = EEPR_BackupChannelData2Eepr(EEPR_CH_HEADER);
        if(errtype != EEPR_NO_ERROR){
            errtype = EEPR_BackupChannelData2Eepr(EEPR_CH_HEADER);
        }
        return errtype;
    }

    if(RTC_NVMRAM_DATAVALID_VARIABLE == 0)
    {  // NVM data corrupt, so set all the dirty flags and take backup (if valid) or default values
        for(cnt = 0; cnt < EEPR_CHANNEL_MAX_NR; cnt++){
            EEPR_SetChDirtyFlag(cnt);
        }
        errtype |= EEPR_ReadChannelData(EEPR_CH_NVSOC);
        retval |= errtype;
        if(errtype != EEPR_NO_ERROR){
            errtype = EEPR_NO_ERROR;
            EEPR_SetDefaultValue(EEPR_CH_NVSOC);
        }
        errtype |= EEPR_ReadChannelData(EEPR_CH_CONTACTOR);
        retval |= errtype;
        if(errtype != EEPR_NO_ERROR){
            errtype = EEPR_NO_ERROR;
            EEPR_SetDefaultValue(EEPR_CH_CONTACTOR);
        }
        errtype |= EEPR_ReadChannelData(EEPR_CH_OPERATING_HOURS);
        retval |= errtype;
        if(errtype != EEPR_NO_ERROR){
            errtype = EEPR_NO_ERROR;
            EEPR_SetDefaultValue(EEPR_CH_OPERATING_HOURS);
        }
        RTC_NVMRAM_DATAVALID_VARIABLE = 1;      // validate NVNRAM data
    }
    else
    {
        //@FIXME do set dirty flags for not double buffered channel (not in bkpsram) unless the ram is not cleared (warm reset)
        errtype |= EEPR_RefreshChannelData(EEPR_CH_NVSOC);
        retval |= errtype;
        if(errtype == EEPR_ERR_RD){ // read error (no eeprom data available) and NVM data corrupt, so take default values
            errtype = EEPR_NO_ERROR;
            EEPR_SetDefaultValue(EEPR_CH_NVSOC);
        }

        errtype |= EEPR_RefreshChannelData(EEPR_CH_CONTACTOR);
        retval |= errtype;
        if(errtype == EEPR_ERR_RD){ // read error (no eeprom data available) and NVM data corrupt, so take default values
            errtype = EEPR_NO_ERROR;
            EEPR_SetDefaultValue(EEPR_CH_CONTACTOR);
        }

        errtype |= EEPR_RefreshChannelData(EEPR_CH_OPERATING_HOURS);
        retval |= errtype;
        if(errtype == EEPR_ERR_RD){ // read error (no eeprom data available) and NVM data corrupt, so take default values
            errtype = EEPR_NO_ERROR;
            EEPR_SetDefaultValue(EEPR_CH_OPERATING_HOURS);
        }
    }
    return retval;
}

EEPR_ERRORTYPES_e EEPR_Init(void){
    timeout_cnt = 0;
    EEPR_ERRORTYPES_e retval;
    retval = EEPR_NO_ERROR;
    uint16_t maxtime = EEPR_WRITETIME_PER_PAGE;
    if(EEPR_SetStateRequest(EEPR_IDLE, 0, 0) == EEPR_OK){
        while (EEPR_GetState() != EEPR_IDLE)
        {
            if (timeout_cnt > maxtime) {   // timeout: 50ms (normal needed time: 5ms)
               // eeprom not usable, no eeprom data available
                retval=EEPR_INIT_ERROR;     // possible reasons: SPI busy, eeprom hardware in busy state or defect
                break;
            }

        timeout_cnt++;
        EEPR_Trigger();
        osDelay(1);             // wait 1ms for next call of eeprom trigger
        }
    }

    if(retval == 0)
    {
        //initialization successful
        retval = EEPR_InitChannelData();      // do update channel data
    }
    return retval;
}


EEPR_RETURNTYPE_e EEPR_GetChannelData(EEPR_CHANNEL_ID_TYPE_e eepr_channel, uint8_t *dest_ptr){
    EEPR_RETURNTYPE_e ret_val = EEPR_OK;
    uint8_t *dataptr;
    dataptr = dest_ptr;

    if(dataptr == NULL_PTR){
        if(dest_ptr != NULL_PTR){
            dataptr = eepr_ch_cfg[eepr_channel].bkpsramptr;
        }else
            ret_val = EEPR_ERROR;
    }
    if(ret_val == EEPR_OK)
        ret_val = EEPR_SetStateRequest(EEPR_READMEMORY,eepr_channel, dataptr);
    
    return ret_val;
}


EEPR_RETURNTYPE_e EEPR_SetChannelData(EEPR_CHANNEL_ID_TYPE_e eepr_channel, uint8_t *src_ptr){
    EEPR_RETURNTYPE_e ret_val;
    uint8_t *dataptr;
    dataptr=src_ptr;
    uint16_t byte_len = eepr_ch_cfg[eepr_channel].length;

    if(dataptr == NULL_PTR){
        dataptr = eepr_ch_cfg[eepr_channel].bkpsramptr;
    }
    //checksumtest
    if(EEPR_CalcChecksum(dataptr, byte_len - 4) == *((uint32_t*)(dataptr + byte_len - 4))){
        ret_val = EEPR_SetStateRequest(EEPR_WRITEMEMORY,eepr_channel, dataptr);
    }else{
        ret_val = EEPR_ERROR;
    }
    if(ret_val == EEPR_OK){
        EEPR_RemoveChDirtyFlag(eepr_channel);
    }
    return ret_val;
}


