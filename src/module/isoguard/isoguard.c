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
 * @file    isoguard.c
 * @author  foxBMS Team
 * @date    12.10.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  ISO
 *
 * @brief   Driver for the isolation monitoring.
 *
 * main file for insulation measurement
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
#include "isoguard.h"

#include "database.h"
#include "mcu.h"
#include "diag.h"
#include "ir155.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/**
 * Dutycycle - resistance table
 * for Bender IR155-3204 (Art. No. B91068139)
 *
 *  dc | resistance/kOhm  |  description           |  interval
 *  ---|------------------|------------------------|--------
 *  100| -63,16           |  not valid             | 0
 *  98 | -38,71           |  invalid               | 0
 *  95 | =0,00            |  invalid               | 0
 *  95 | >0,00            |  shortcut              | 1
 *  90 | 70,59            |  very low resistance   | 2
 *  80 | 240,00           |  low resistance        | 3
 *  70 | 461,54           |  low resistance        | 3
 *  60 | 763,64           |  lowmid resistance     | 4
 *  50 | 1200,00          |  highmid resistance    | 5
 *  40 | 1885,71          |  highmid resistance    | 5
 *  30 | 3120,00          |  high resistance       | 6
 *  20 | 6000,00          |  high resistance       | 6
 *  10 | 20400,00         |  very high resistance  | 7
 *  5  | oo               |  very high resistance  | 7
 *  3  |-55200,00         |  invalid               | 0
 *  0  |-22800,00         |  invalid               | 0
 *
 */

#ifdef ISO_ISOGUARD_ENABLE
uint16_t const static ir155_ResistanceInterval[7] = {
    0,      /*!< shortcut(>0) or invalid (=0)   */
    70,     /*!< very low resistance            */
    240,    /*!< low resistance                 */
    763,    /*!< low-mid resistance             */
    1200,   /*!< high-mid resistance            */
    3120,   /*!< high resistance                */
    20400   /*!< very high resistance           */
};
#endif

/*================== Function Prototypes ==================================*/
#ifdef ISO_ISOGUARD_ENABLE
static uint8_t ISO_GetResistanceIntervall(uint32_t resistance);
#endif
/*================== Function Implementations =============================*/

void ISO_Init(void) {

#ifdef ISO_ISOGUARD_ENABLE
    /* Initialize Software-Module */
    IR155_Init(ISO_CYCLE_TIME);

    /* Enable Hardware-Bender-Module */
    IR155_ENABLE_BENDER_HW();
#endif
}


void ISO_ReInit(void) {

#ifdef ISO_ISOGUARD_ENABLE
    /* Disable Hardware-Bender-Modul */
    IR155_DISABLE_BENDER_HW();

    /* DeInit Software-Modul */
    IR155_DeInit();

    /* DeInit Timer-Modul */
    HAL_TIM_IC_DeInit(&TIM_BENDER_IC_HANDLE);

    /* Wait 20us */
    MCU_Wait_us(20);

    /* Init and enable timer module and Isometer*/
    ISO_Init();
#endif

}


void ISO_MeasureInsulation(void) {

#ifdef ISO_ISOGUARD_ENABLE

    STD_RETURN_TYPE_e retVal = E_NOT_OK;
    uint32_t resistance = 0;
    IR155_STATE_e state = IR155_STATE_UNDEFINED;
    static DATA_BLOCK_ISOMETER_s ISO_measData = {    // database structure
            .valid = 1,
            .state = 1,
            .resistance = 0,
            .timestamp = 0,
            .previous_timestamp = 0,
    };

    retVal = IR155_MeasureResistance(&state, &resistance);

    /* Get resistance intervall */
    ISO_measData.resistance = (ISO_GetResistanceIntervall(resistance) & 0x07 );  // 3 Bits: 2^3 Interval areas

    if(state == IR155_MEAS_NOT_VALID) {
        /* Measurement result is not valid */
        ISO_measData.valid = 1;
    } else {
        ISO_measData.valid = 0;
    }

    if(retVal == E_OK && resistance > ISO_RESISTANCE_THRESHOLD &&
            (state == IR155_RESIST_MEAS_GOOD || state == IR155_RESIST_ESTIM_GOOD)) {
        ISO_measData.state = 0;     // Good resistance measured
    } else {
        ISO_measData.state = 1;     // Invalid resistance measured or error occured;
    }

    ISO_measData.previous_timestamp = ISO_measData.timestamp;
    ISO_measData.timestamp = MCU_GetTimeStamp();

    /* Store data in database */
    DATA_StoreDataBlock(&ISO_measData, DATA_BLOCK_ID_ISOGUARD);
#endif

    DIAG_SysMonNotify(DIAG_SYSMON_ISOGUARD_ID, 0);        // task is running, state = ok
}

#ifdef ISO_ISOGUARD_ENABLE
/**
 * Determines measured resistance interval.
 * Use of intervals because of measuring and signal inaccuracy
 * @param   resistance measured resistance in [kOhm]
 * @return  resistance interval 0 - 7
 */
static uint8_t ISO_GetResistanceIntervall(uint32_t resistance) {
    uint8_t interval = 0;

    if(resistance > ir155_ResistanceInterval[6]) {
        interval = 7;               /* very high resistance */
    }
    else if(resistance > ir155_ResistanceInterval[5]) {
        interval = 6;               /* high resistance */
    }
    else if(resistance > ir155_ResistanceInterval[4]) {
        interval = 5;               /* highmid resistance */
    }
    else if(resistance > ir155_ResistanceInterval[3]) {
        interval = 4;               /* lowmid resistance */
    }
    else if(resistance > ir155_ResistanceInterval[2]) {
        interval = 3;               /* low resistance */
    }
    else if(resistance > ir155_ResistanceInterval[1]) {
        interval = 2;               /* very low resistance */
    }
    else if(resistance > ir155_ResistanceInterval[0]) {
        interval = 1;               /* shortcut */
    }
    else {
        interval = 0;               /* invalid */
    }
    return interval;
}
#endif
