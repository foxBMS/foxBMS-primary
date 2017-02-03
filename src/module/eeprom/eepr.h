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
 * @file    eepr.h
 * @author  foxBMS Team
 * @date    27.11.2012
 * @ingroup DRIVERS
 * @prefix  EEPR
 *
 * @brief Headers for the driver for the storage in the EEPROM memory.
 *
 * Header file driver of external EEPROM device
 *
 */
#ifndef EEPR_H_
#define EEPR_H_

/*================== Includes =============================================*/
#include "eepr_cfg.h"

/*================== Macros and Definitions ===============================*/

/** Instruction Set for Serial EEPROM AT25128 (16k x 8 Bit): <br> - REN */
#define EEPR_CMD_WREN    0x06
/** Instruction Set for Serial EEPROM AT25128 (16k x 8 Bit): <br> - RDI */
#define EEPR_CMD_WRDI    0x04
/** Instruction Set for Serial EEPROM AT25128 (16k x 8 Bit): <br> - DSR */
#define EEPR_CMD_RDSR    0x05
/** Instruction Set for Serial EEPROM AT25128 (16k x 8 Bit): <br> - RSR */
#define EEPR_CMD_WRSR    0x01
/** Instruction Set for Serial EEPROM AT25128 (16k x 8 Bit): <br> - Read */
#define EEPR_CMD_READ    0x03
/** Instruction Set for Serial EEPROM AT25128 (16k x 8 Bit): <br> - Write */
#define EEPR_CMD_WRITE   0x02

/**
 * Control and Status Information of EEPROM Driver
 * must be a multiple of 0x40 (64 Byte pages!)
 */
#define EEPR_DATA_STARTADDR            0x00


/**
 * symbolic names for the EEPROM states
 */
typedef enum {
    EEPR_INITIALIZED            = 0,    /*!< Initializing-Sequence: EEPROM-driver initialized   */
    EEPR_INIT_INPROCESS         = 1,    /*!< Initializing-Sequence: EEPROM-driver initializing  */
    EEPR_IDLE                   = 2,    /*!< Idle-State                                         */
    EEPR_READMEMORY             = 3,    /*!< Reading-Sequence: Initiate reading                 */
    EEPR_READMEMORY_INPROCESS   = 4,    /*!< Reading-Sequence: Currently reading                */
    EEPR_CHECK_DATA             = 5,    /*!< Reading-Sequence: Data check                       */
    EEPR_WRITEMEMORY            = 6,    /*!< Writing-Sequence: Initiate writing                 */
    EEPR_WRITE_ENABLE_INPROCESS = 7,    /*!< Writing-Sequence: Enable writing                   */
    EEPR_WRITEMEMORY_ENABLED    = 8,    /*!< Writing-Sequence: Writing enabled                  */
    EEPR_WRITEMEMORY_INPROCESS  = 9,    /*!< Writing-Sequence: Currently writing                */
    EEPR_WRITEFAILED            = 0xFC, /*!< Error-State: Write failed                          */
    EEPR_READFAILED             = 0xFD, /*!< Error-State: Read failed                           */
    EEPR_INITFAILED             = 0xFE, /*!< Error-State: Initialization failed                 */
    EEPR_UNINITIALIZED          = 0xFF  /*!< EEPROM-driver not initialized                      */
} EEPR_STATE_e;


/**
 * Bitfield of EEPROM Status Register RSR
 */
typedef volatile union {
    struct {
        unsigned int nRDY           : 1;    /*!< RDY=0 indicates device is ready                */
        unsigned int WEN            : 1;    /*!< WEN=0 indicates device is not write enabled    */
        unsigned int BP0            : 1;    /*!< BP0 Block Protection Bit 0                     */
        unsigned int BP1            : 1;    /*!< BP1 Block Protection Bit 1                     */
        unsigned int                : 3;    /*!< dummy                                          */
        unsigned int WPEN           : 1;    /*!< WPEN                                           */

    } B;
    uint8_t u8;
} EEPR_STATUSREGISTER_s;

/**
 * Control and Status Information of EEPROM Driver
 */
typedef struct {
    EEPR_STATUSREGISTER_s RSR;  /*!< EEPROM status register                                             */
    EEPR_STATE_e state;         /*!< state of EEPROM Driver State Machine                               */
    uint8_t substate;           /*!< sub-state of state                                                 */
    uint8_t triggerentry;       /*!< re-entrance protection (function running flag)                     */
    uint8_t repeat;             /*!<                                                                    */
    uint32_t timer;             /*!< in counts of 1ms                                                   */
    uint16_t address;           /*!< actually accessed EEPROM address (absolute, 64 Byte page aligned)  */
    uint16_t nr_of_pages;       /*!< (sizeof(EEPR_STATUS_s)+63/64)                                      */
    uint16_t page_nr;           /*!< actually accessed EEPROM page                                      */
    uint16_t readtime;          /*!< time for read of all pages of struct_CALIB_FRAME                   */
    uint16_t writetime;         /*!< time for write of all pages of struct_CALIB_FRAME                  */
    uint8_t write_len;          /*!< write page in EEPROM  < 64Byte                                     */
    uint8_t read_len;           /*!< depends only on SPI buffer                                         */
} EEPR_STATUS_s;

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/

/**
 * @brief   cyclic trigger of EEPROM State Machine, trigger cycle: 1ms
 *          trigger timer in units of 1ms
 *
 * @return  void
*/
extern void EEPR_StateTrigger(void);

/**
 * @brief   manually trigger for state changes
 *
 * @param   state_req   next state to be entered
 *
 * @return  retVal      DIAG_EEPR_OK if request triggered, DIAG_EEPR_BUSY if EEPR_StateTrigger() is
 *                      actually running, otherwise DIAG_EEPR_ERROR
*/
extern EEPR_RETURNTYPE_e EEPR_StateRequest(EEPR_STATE_e state_req);

/**
 * @brief   returns actual state of EEPROM driver
 *
 * @return  state of EEPROM driver
*/
extern EEPR_STATE_e EEPR_GetState(void);

// FIXME function not called, check if needed?
/**
 * @brief   control of EEPROM Data management
 *
 * @return  void
*/
extern void EEPR_DataCtrl(void);

/**
 * @brief   reads "byte_len" of bytes from EEPROM memory starting at "address" and saving data in globla variable eepr_cmd_read[]
 *
 * @param   EeprAddr  starting address of EEPROM where to read data
 * @param   byte_len  length of bytes that are to be read
 * @return  0 if reading successful, otherwise 0xFF
*/
extern uint8_t EEPR_ReadMemory(uint16_t EeprAddr, uint16_t byte_len);

/**
 * @brief   max. time for reading of eeprom data (depending on data length)
 *
 * @return  readtime in ms
*/
extern uint16_t EEPR_GetReadTime(void);

/**
 * @brief   max. time for writing of eeprom data (depending on data length)
 *
 * @return  writetime in ms
*/
extern uint16_t EEPR_GetWriteTime(void);

/*================== Function Implementations =============================*/

#endif /* EEPR_H_ */
