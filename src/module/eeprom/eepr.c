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
 * @file    eepr.c
 * @author  foxBMS Team
 * @date    07.10.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  EEPR
 *
 * @brief Driver for the storage in the EEPROM memory.
 *
 * driver for external EEPROM device AT25128 (16kByte)
 * /HOLD and /WP are permanently kept at Hi-Level, thus HW-Write-Protection (WP) is disabled.
 * SW-Protection(Block Write Protection) could be done by Sending Command EEPR_CMD_WREN if disabled before by EEPR_CMD_WRDI.
 * At startup the device is unprotected. Actual state will be provided by EEPR_CMD_RDSR (Read Status Register).
 * A Block Write Protection can be controlled by Status Register Bit BP1 and BP0 in different address ranges within
 * the whole range: Byte Address 0x0000..0x3FFF
 * \n
 *  | Protected Address  |    BP1  |  BP0    |
 *  | :----              |:----    | :----   |
 *  | None               |    0    |    0    |
 *  | 0x3000..0x3FFFF    |    0    |    1    |
 *  | 0x2000..0x3FFFF    |    1    |    0    |
 *  | 0x0000..0x3FFFF    |    1    |    1    |
 */

/*================== Includes =============================================*/
#include "eepr.h"
#include "cmsis_os.h"
#include "diag.h"

/*================== Macros and Definitions ===============================*/



/*================== Constant and Variable Definitions ====================*/

uint8_t eepr_datastate = 0;
uint8_t eepr_cmd_read[128+3] = { EEPR_CMD_READ, 0, 0 };
uint8_t eepr_cmd_write[128+3] = { EEPR_CMD_WRITE, 0, 0 };
uint8_t eepr_cmdbuf[2];
uint8_t eepr_cmd_WREN[1] = { EEPR_CMD_WREN };



// EEPR_STATUSREGISTER_s epr_state;
EEPR_STATUS_s eepr_state = {
    .RSR.u8         = 0xFF,
    .state          = EEPR_UNINITIALIZED,
    .substate       = 0,
    .triggerentry   = 0,
    .repeat         = 2,
    .timer          = 0,
    .address        = 0,
    .nr_of_pages    = 1,   // will be re-set at initialization to sizeof(EEPR_CALIB_FRAME_s)+63/64
    .readtime       = 5,   // will be re-set at initialization to 5ms  * (sizeof(EEPR_CALIB_FRAME_s)+63/64)
    .writetime      = 50,  // will be re-set at initialization to 50ms * (sizeof(EEPR_CALIB_FRAME_s)+63/64)
    .page_nr        = 0,
    .write_len      = 64,
    .read_len       = 64,
};





/*================== Function Prototypes ==================================*/
static void EEPR_StateFailed(EEPR_STATE_e state);
static void EEPR_ReEnterStateInit(void);


/*================== Function Implementations =============================*/

// FIXME if config changes (for example eepr_writebuffer) the main driver files have to be changed as well. Maintainability?
/**
 * @brief   initialization of external Serial EEPROM device (AT25128N)
 *
 * @return  DIAG_EEPR_OK if initialization OK, otherwise DIAG_EEPR_ERROR
*/
EEPR_RETURNTYPE_e EEPR_Init(void)
{

    EEPR_RETURNTYPE_e retVal = EEPR_ERROR;
    eepr_state.address = EEPR_DATA_STARTADDR;    // must be a multiple of 0x40 (64 Byte pages !)
    eepr_state.nr_of_pages = (sizeof(EEPR_CALIB_FRAME_s)+63)/64; // will be used for EEPROM write in blocks of 64 Byte pages
    eepr_state.readtime =  ((sizeof(EEPR_CALIB_FRAME_s)+63)/64) * 5;         // max.  5ms time for reading each page (64 Byte)
    eepr_state.writetime = ((sizeof(EEPR_CALIB_FRAME_s)+63)/64) * 50;        // max. 50ms time for writing each page (64 Byte)
    eepr_state.page_nr=0;

    eepr_cmdbuf[0]=(EEPR_CMD_RDSR);    // first byte is command, second one is dummy (for data receive)

    eepr_writebuffer = eepr_frame.Block.data;        // take actual data as write buffer initialization

    if(EEPR_SendData(eepr_cmdbuf, 2, 1)  ==  EEPR_OK)
    {   // send data, data length, receive data offset(leading dummy bytes)
        retVal = EEPR_OK;     // EEPROM Initialization: command sending process has been started
    }
    else {
        retVal = EEPR_ERROR;  // EEPROM Initialization failed due to communication error
    }

    return retVal;

}


uint16_t EEPR_GetReadTime(void)
{
    return (eepr_state.readtime);
}

uint16_t EEPR_GetWriteTime(void)
{
    return (eepr_state.writetime);
}

void EEPR_DataCtrl(void)
{

    if(!eepr_datastate) {
        if(EEPR_GetState()  ==  EEPR_IDLE)
        {
            EEPR_StateRequest(EEPR_READMEMORY);
            eepr_datastate=1;        // read data once after startup
        }
    }
    else if(eepr_datastate == 1)
    {
        if(EEPR_GetState()  ==  EEPR_IDLE)
        {    // data check ok
            eepr_datastate=2;
        }
        else if(EEPR_GetState()  ==  EEPR_READFAILED)
        {
            eepr_datastate=2;
        }
    }
}


uint8_t EEPR_ReadMemory(uint16_t EeprAddr, uint16_t byte_len)
{

    eepr_cmd_read[1]=(uint8_t)(EeprAddr>>8);    // Highbyte of Address
    eepr_cmd_read[2]=(uint8_t)(EeprAddr);       // Lowbyte of Address

    if(byte_len > EEPR_TXBUF_LENGTH-3)
        return 0xFE;

    if(EEPR_SendData(&eepr_cmd_read[0], byte_len+3, 3)  ==  0)
    {
    // Send Read Command has been started
        return 0;
    }
    else
        return 0xFF;

}


// FIXME variable declaration in the middle of nowhere, why global and not in function EEPR_WriteMemory
// FIXME why +4 when only used CMD and EeprAddr (1+2=4?)
uint8_t tmpbuf[(sizeof (EEPR_CALIB_FRAME_s)) + 4];
/**
 * @fn     U8 EEPR_WriteMemory(U16 EeprAddr,U8 *srcptr ,U16 byte_len)
 * @brief  writes "byte_len" of bytes from EEPROM memory starting at "address" and reading data from "srcptr"
 *
 * @param  EeprAddr  starting address of EEPROM where to write data
 * @param  srcptr    pointer where the data is stored
 * @param  byte_len  length of bytes that are to be written
 * @return 0 if writing successful, otherwise 0xFF
*/
uint8_t EEPR_WriteMemory(uint16_t EeprAddr, uint8_t *srcptr, uint16_t byte_len)
{

    uint8_t cnt = 0;

    if(srcptr == NULL)
        return 0xFF;

    if(!byte_len)
        return 0xFF;
    else if(byte_len > EEPR_TXBUF_LENGTH-3)
        return 0xFE;

    tmpbuf[0]=EEPR_CMD_WRITE;
    tmpbuf[1]=(uint8_t)(EeprAddr>>8);    // Highbyte of Address
    tmpbuf[2]=(uint8_t)(EeprAddr);       // Lowbyte of Address

    for(cnt=0;cnt<byte_len;cnt++)
    {
        tmpbuf[3+cnt]=*srcptr++;
    }

    if(EEPR_SendData(&tmpbuf[0], byte_len+3, 3)  ==  0)
    {
        // Send Write Command has been started
        return 0;
    }
    else
        return 0xFF;
}

EEPR_STATE_e EEPR_GetState(void)
{
    return (eepr_state.state);
}


EEPR_RETURNTYPE_e EEPR_StateRequest(EEPR_STATE_e state_req)
{
    EEPR_RETURNTYPE_e retVal = EEPR_OK;

    taskENTER_CRITICAL();
    if(eepr_state.triggerentry)
    {
        retVal = EEPR_BUSY;   // return as "BUSY" because EEPR_StateTrigger() is actually running
    }

    if(retVal == EEPR_OK)
    {
        // when in EEPR_IDLE, EEPR_READFAILED or in EEPR_WRITEFAILED
        // only accept requests for: EEPR_WRITEMEMORY or EEPR_READMEMORY
        if (eepr_state.state  ==  EEPR_IDLE || eepr_state.state  ==  EEPR_READFAILED
                || eepr_state.state  ==  EEPR_WRITEFAILED)
        {

            if(state_req  ==  EEPR_WRITEMEMORY)
            {

                eepr_state.state = EEPR_WRITEMEMORY;
                (void)(DIAG_Handler(DIAG_CH_CALIB_EEPR_FAILURE,DIAG_EVENT_RESET,0, NULL));  // clear error event
            }
            else if(state_req  ==  EEPR_READMEMORY)
            {

                eepr_state.state = EEPR_READMEMORY;
                (void)(DIAG_Handler(DIAG_CH_CALIB_EEPR_FAILURE,DIAG_EVENT_RESET,1, NULL));  // clear error event
            }
            else
                retVal = EEPR_ERROR;
        }
        else if (eepr_state.state == EEPR_INITFAILED)
        {
            // when in EEPR_INITFAILED
            // only accept requests for: EEPR_IDLE

            if(state_req  ==  EEPR_IDLE)
            {
                eepr_state.state = EEPR_UNINITIALIZED;
                (void)(DIAG_Handler(DIAG_CH_CALIB_EEPR_FAILURE, DIAG_EVENT_RESET, 2, NULL)); // clear error event
            }
            else
                retVal = EEPR_ERROR;
        }
        else
            retVal = EEPR_ERROR;
    }

    taskEXIT_CRITICAL();

    return (retVal);
}


/**
 * @brief   transition function for error states
 *
 * @param   state error state
 *
 * @return  void
 */
static void EEPR_StateFailed(EEPR_STATE_e state)
{
    eepr_state.state    = state;
    eepr_state.repeat = 2;    // reset re-try-timer for next usage
}


/**
 * @brief   EEPROM state is reset to state after initialization
 *
 * @return  void
 */
static void EEPR_ReEnterStateInit(void)
{
    eepr_state.page_nr=0;
    eepr_state.address = EEPR_DATA_STARTADDR;
    eepr_state.repeat = 2;    // reset re-try-timer for next usage
}


void EEPR_StateTrigger(void)
{

    EEPR_RETURNTYPE_e ret_val;
    EEPR_STATUSREGISTER_s tmp_eepr_stat;

    // Check re-entrance of function

    taskENTER_CRITICAL();
    if(!eepr_state.triggerentry)
    {

        eepr_state.triggerentry++;
        ret_val = EEPR_OK;
    }
    else
        ret_val = EEPR_ERROR;    // multiple calls of function
    taskEXIT_CRITICAL();

    if (ret_val == EEPR_ERROR)
        return;

    if(eepr_state.timer)
    {

        --eepr_state.timer;
        eepr_state.triggerentry--;
        return;    // handle state machine only when timer has elapsed
    }

    switch(eepr_state.state)
    {

        case EEPR_UNINITIALIZED:

            ret_val=EEPR_Init();        // initialize EEPROM
            if(ret_val == EEPR_OK)
            {

                eepr_state.timer  = 5;
                eepr_state.state  = EEPR_INIT_INPROCESS;
                eepr_state.repeat = 2;    // reset repeat counter for next usage
            }
            else
            {
                if(--eepr_state.repeat)
                {

                    eepr_state.timer  = 5;    // retry in 5ms
                } else
                {
                    eepr_state.state  = EEPR_INITFAILED;
                    eepr_state.repeat = 2;    // reset repeat counter for next usage
                }
            }
            break;

        case EEPR_INIT_INPROCESS:

            // wait for response
            ret_val = EEPR_ReceiveData((uint8_t*)(&tmp_eepr_stat),1);

            if(ret_val != EEPR_OK)
            {
            // no data available
                if(--eepr_state.repeat)
                {

                    eepr_state.timer  = 2;    // retry Reading in 1ms
                } else
                {
                    EEPR_StateFailed(EEPR_INITFAILED);
                }
                break;
            }
            else
            {
                eepr_state.repeat = 2;    // reset repeat counter for next usage
            }

            // response received
            if(tmp_eepr_stat.B.nRDY)
            {
            // /RDY-Flag is set -> EEPROM is busy (write cycle progress)
                if(--eepr_state.repeat)
                {
                    eepr_state.timer    = 10;    // restart initialization
                    eepr_state.state    = EEPR_UNINITIALIZED;
                } else
                {
                    EEPR_StateFailed(EEPR_INITFAILED);
                }

            }
            else
            {
                eepr_state.RSR = tmp_eepr_stat;    // overtake status information
                // initialization succeeded
                eepr_state.timer    = 0;
                eepr_state.state    = EEPR_IDLE;
            }
            break;

        case EEPR_READMEMORY:

            eepr_state.read_len = sizeof(EEPR_CALIB_FRAME_s) - eepr_state.page_nr * 64;    // rest size to be send
            if(eepr_state.read_len > 64)
                eepr_state.read_len=64;

            ret_val = EEPR_ReadMemory(eepr_state.address,eepr_state.read_len);
            if(ret_val != EEPR_OK)
            {
                if(--eepr_state.repeat)
                {
                    eepr_state.timer    = 2;    // retry Start of EEPROM Reading
                } else
                {
                    EEPR_StateFailed(EEPR_READFAILED);
                }
            }
            else
            {
                eepr_state.timer    = 2;
                eepr_state.state    = EEPR_READMEMORY_INPROCESS;
            }
            break;

        case EEPR_READMEMORY_INPROCESS:

            ret_val = EEPR_ReceiveData((uint8_t*)((uint32_t)(&eepr_frame)+eepr_state.page_nr*64),eepr_state.read_len);

            if(ret_val  ==  EEPR_OK)
            {
                if(++eepr_state.page_nr < eepr_state.nr_of_pages)
                {
                    eepr_state.address += 64;
                    eepr_state.state    = EEPR_READMEMORY;    // read next page
                }
                else
                {
                    eepr_state.state    = EEPR_CHECK_DATA;
                }
            }
            break;

        case EEPR_CHECK_DATA:
            // Verify Checksum
            if(eepr_frame.Block.chksum != EEPR_CalcChecksum((uint8_t*)(&eepr_frame.Block.data.index[0]), sizeof(struct_CALIB_DATA_s)))
            {   // data corrupt
                if(--eepr_state.repeat)
                {
                    eepr_state.state = EEPR_IDLE;   // retry reading of EEPROM
                    EEPR_ReEnterStateInit();        // FIXME repeat counter will endlessly be reset, mind actual eepr_state.page_nr
                }
                else
                {
                    EEPR_StateFailed(EEPR_READFAILED);
                    eepr_frame.Block.data = defaultcalibdata.Block.data;    // take default data
                    /********************************************/
                    /* KEEP CALBRATION DATA at last valid NVRAM data (CALIB_eNVMDATA)
                     * or at default CALIB_defaultDATA*/
                    /********************************************/
                }
            }
            else
            {
                eepr_state.state    = EEPR_IDLE;
                EEPR_ReEnterStateInit();
            }

            break;

        case EEPR_IDLE:

            // wait for State Change Request
            EEPR_ReEnterStateInit();
            break;

        case EEPR_WRITEMEMORY:

            // do first Write-Enable (WEN)
            if(EEPR_SendData(&eepr_cmd_WREN[0], 1 ,0)  ==  EEPR_OK)
            {   // send data, data length, receive data offset(leading dummy bytes)
            // Sending Command has been started
                if(--eepr_state.repeat) {

                   eepr_state.timer    = 10;
                   eepr_state.state    = EEPR_WRITE_ENABLE_INPROCESS;
                   eepr_state.substate = 0;
                   eepr_state.repeat=2;
                } else {

                    EEPR_StateFailed(EEPR_WRITEFAILED);
                }
            }
            else
            {
                eepr_state.timer    = 1;
            }
            break;

        case EEPR_WRITE_ENABLE_INPROCESS:

            if(eepr_state.substate  ==  0)
            {
                ret_val = EEPR_ReceiveData((uint8_t*)(&tmp_eepr_stat),1);

                if(ret_val != EEPR_OK)
                    break;

                /*Write-Enable (WEN) has been send !*/
                /*Now, verify status if WREN was accepted*/
                eepr_cmdbuf[0]=(EEPR_CMD_RDSR);    // first byte is command, second one is dummy (for data receive)
                if(EEPR_SendData(eepr_cmdbuf, 2, 1)  ==  0)
                {   // send data, data length, receive data offset(leading dummy bytes)
                    if(--eepr_state.repeat)
                    {
                        eepr_state.timer    = 1;
                        eepr_state.substate = 1 ;
                        eepr_state.repeat=2;
                        // stay in state EEPR_WRITE_ENABLE_INPROCESS
                    }
                    else
                    {
                       EEPR_StateFailed(EEPR_WRITEFAILED);
                    }
                }
                else
                {
                    eepr_state.timer    = 1;    // retry Sending
                }
            }
            else if(eepr_state.substate  ==  1)
            {
            // check Status Register of EEPROM (RSR)
                ret_val = EEPR_ReceiveData((uint8_t*)(&tmp_eepr_stat),1);
                if(ret_val != EEPR_OK) {

                    if(--eepr_state.repeat)
                    {
                    // no data available
                        eepr_state.timer    = 1;    // retry Receiving
                    }
                    else
                    {
                        EEPR_StateFailed(EEPR_WRITEFAILED);
                    }
                    break;
                }

                // response received
                if(!tmp_eepr_stat.B.nRDY)
                {
                // /RDY-Flag is cleared -> EEPROM is not busy (write cycle not in process)
                    eepr_state.RSR = tmp_eepr_stat;    // overtake status information
                    // initialization succeeded

                    if(eepr_state.RSR.B.WEN)
                    {
                    /* Write-Enable (WEN) has been accepted !*/
                        eepr_state.state = EEPR_WRITEMEMORY_ENABLED;
                        break;
                    }
                }
                // Write Not Enabled OR EEPROM is busy
                // retry Write Enabling
                if(--eepr_state.repeat)
                {
                    eepr_state.state    = EEPR_WRITEMEMORY;    // retry reading of EEPROM
                    eepr_state.timer    = 5;
                }
                else
                {
                   EEPR_StateFailed(EEPR_WRITEFAILED);
                }
            }
            break;

        case EEPR_WRITEMEMORY_ENABLED:

            eepr_state.write_len=sizeof(EEPR_CALIB_FRAME_s) - eepr_state.page_nr * 64;    // rest size to be send
            if(eepr_state.write_len > 64)
                eepr_state.write_len=64;

            if(eepr_state.page_nr == 0)
            {
                eepr_frame.Block.data = eepr_writebuffer;
                // check Checksum?
                eepr_frame.Block.chksum = eepr_writebufferChksum;
            }

            ret_val = EEPR_WriteMemory(eepr_state.address,(uint8_t*)((uint32_t)(&eepr_frame)+eepr_state.page_nr*64),eepr_state.write_len);

            if(ret_val != EEPR_OK)
            {
                if(--eepr_state.repeat)
                {
                    eepr_state.timer    = 2;    // retry Start Command of EEPROM Write
                } else
                {
                    EEPR_StateFailed(EEPR_WRITEFAILED);
                }
            }
            else
            {
                // FIXME  max time: 10ms/64-byte page ?
                eepr_state.timer    = 10;    // max time: 10ms/64-byte page
                eepr_state.state    = EEPR_WRITEMEMORY_INPROCESS;
            }
            break;

        case EEPR_WRITEMEMORY_INPROCESS:

            ret_val = EEPR_ReceiveData((uint8_t*)(&eepr_frame),eepr_state.write_len);
            if(ret_val  ==  EEPR_OK)
            {
                if(--eepr_state.repeat)
                {
                    if(++eepr_state.page_nr < eepr_state.nr_of_pages)
                    {
                        eepr_state.address += 64;
                        eepr_state.state    = EEPR_WRITEMEMORY;    // write next page
                        eepr_state.repeat=2;
                    }
                    else
                    {
                        /*now, do verify*/
                        EEPR_ReEnterStateInit();
                        eepr_state.state    = EEPR_READMEMORY;
                    }
                }
                else
                {
                    EEPR_StateFailed(EEPR_WRITEFAILED);
                }
            }
            // note: automatically write disable state at the completion of a write cycle !

            break;


        case EEPR_INITFAILED:

            (void)(DIAG_Handler(DIAG_CH_CALIB_EEPR_FAILURE,DIAG_EVENT_NOK,2, NULL));
            EEPR_ReEnterStateInit();
            break;

        case EEPR_READFAILED:

            (void)(DIAG_Handler(DIAG_CH_CALIB_EEPR_FAILURE,DIAG_EVENT_NOK,1, NULL));
            EEPR_ReEnterStateInit();
            break;

        case EEPR_WRITEFAILED:

            (void)(DIAG_Handler(DIAG_CH_CALIB_EEPR_FAILURE,DIAG_EVENT_NOK,0, NULL));
            EEPR_ReEnterStateInit();
            break;

        default:
            break;

    }

    eepr_state.triggerentry--;
}

