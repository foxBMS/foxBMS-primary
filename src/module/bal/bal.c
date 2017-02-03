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
 * @file    bal.c
 * @author  foxBMS Team
 * @date    26.02.2016 (date of creation)
 * @ingroup DRIVERS
 * @prefix  BAL
 *
 * @brief   Driver for the Balancing module.
 */

/*================== Includes =============================================*/
#include "general.h"
#include "bal.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/
static DATA_BLOCK_BALANCING_CONTROL_s bal_balancing;
static DATA_BLOCK_CELLVOLTAGE_s bal_cellvoltage;
static DATA_BLOCK_MINMAX_s bal_minmax;

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

/*================== Public functions =====================================*/
void BAL_Init(void) {
    DATA_GetTable(&bal_balancing, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
    bal_balancing.enable_balancing = 0;
    DATA_StoreDataBlock(&bal_balancing, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
}

void BAL_Ctrl(void) {
    uint32_t i = 0;
    uint16_t min = 0;

    DATA_GetTable(&bal_balancing, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
    DATA_GetTable(&bal_cellvoltage, DATA_BLOCK_ID_CELLVOLTAGE);
    DATA_GetTable(&bal_minmax, DATA_BLOCK_ID_MINMAX);

    min = bal_minmax.voltage_min;

    if (bal_balancing.enable_balancing == 1) {
        for (i=0;i<NR_OF_BAT_CELLS;i++) {
            if ( bal_cellvoltage.voltage[i] > (min+(uint16_t)(bal_balancing.threshold)) ) {
                bal_balancing.value[i] = 1;
            } else {
                bal_balancing.value[i] = 0;
            }
        }
    }
    else {
        for (i=0;i<NR_OF_BAT_CELLS;i++) {
            bal_balancing.value[i] = 0;
            }
    }

    DATA_StoreDataBlock(&bal_balancing, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);

}

/*================== Static functions =====================================*/
