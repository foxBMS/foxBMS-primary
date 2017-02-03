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
 * @file    sox_cfg.h
 * @author  foxBMS Team
 * @date    18.08.2015 (date of creation)
 * @ingroup APPLICATION_CONF
 * @prefix  SOX
 *
 * @brief   Configuration header for SOX
 *
 */

#ifndef SOX_CFG_H_
#define SOX_CFG_H_

/*================== Includes =============================================*/
#include "general.h"
#include "database.h"

/*================== Macros and Definitions ===============================*/
/*fox
 * when initializing SOC from SOC-Voltage lookup table, current must be below
 * this value in order to ensure low disturbance and equilibrium in chemical
 * reactions (relaxation effects)
 * @var         maximum current allowed for SOC initialization
 * @type        int
 * @unit        mA
 * @validator   [1,200]
 * @default     100
 * @level       user
 * @group       SOX
 */
#define SOX_SOC_INIT_CURRENT_LIMIT      100

/*fox
 * when initializing SOC from SOC-Voltage lookup table, the difference between
 * two consecutive voltage measurements of the cell with minimum voltage must be below
 * this value in order to ensure low disturbance and equilibrium in chemical
 * reactions (relaxation effects)
 * @var         maximum variation allowed for minimum voltage to enable SOC initialization
 * @type        int
 * @unit        mV
 * @validator   [1,20]
 * @default     10
 * @level       user
 * @group       SOX
 */
#define SOX_DELTA_MIN_LIMIT             10

/*fox
 * when initializing SOC from SOC-Voltage lookup table, the difference between
 * two consecutive voltage measurements of the cell with maximum voltage must be below
 * this value in order to ensure low disturbance and equilibrium in chemical
 * reactions (relaxation effects)
 * @var         maximum variation allowed for maximum voltage to enable SOC initialization
 * @type        int
 * @unit        mV
 * @validator   [1,20]
 * @default     10
 * @level       user
 * @group       SOX
 */
#define SOX_DELTA_MAX_LIMIT             10

/*fox
 * the cell capacity used for SOC calculation, in this case Ah counting
 * Specified once according to data sheet of cell usually.
 * @var         cell capacity in SOC formula
 * @type        float
 * @unit        mAh
 * @level       user
 * @validator   x>1
 * @default     10000.0
 * @group       SOX
 */
#define SOX_CELL_CAPACITY               10000.0

/*fox
 * the maximum current in charge direction that the battery pack can sustain.
 * Normally set once for the specific battery cell from datasheet
 * @var         maximum current continuous charge
 * @type        float
 * @unit        A
 * @group       SOX
 * @validator   [1,240]
 * @default     10.0
 * @level   user
 */
#define SOX_CURRENT_MAX_CONTINUOUS_CHARGE 10.00

/*fox
 * the maximum current in discharge direction that the battery pack can deliver.
 * Normally set once for the specific battery cell from datasheet
 *
 * @var         maximum current continuous discharge
 * @type        float
 * @unit        A
 * @group       SOX
 * @validator   [1,240]
 * @default     10.0
 * @level       user
 */
#define SOX_CURRENT_MAX_CONTINUOUS_DISCHARGE 10.00

/*fox
 * the current that the battery pack should be able to discharge when in LIMPHOME mode,
 * i.e., something noncritical went wrong but it should be able to drive home.
 * The value is system engineer's choice.
 *
 * @var         discharge current in limp home emergency mode
 * @type        float
 * @unit        A
 * @group       SOX
 * @valid       [1,40]
 * @default     3.00
 * @level       user
 */
#define SOX_CURRENT_LIMP_HOME 3.00

/*fox
 * the cold temperature where the derating of maximum discharge current starts,
 * i.e., below this temperature battery pack should not deliver full discharge current
 * @var         low temperature discharge derating start
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     5.0
 * @level       user
 */
#define SOX_TEMP_LOW_CUTOFF_DISCHARGE 5.0

/*fox
 * the cold temperature where the derating of maximum discharge current is fully applied,
 * i.e., below this temperature battery pack should not deliver any current in discharge direction
 * @var         low temperature discharge derating full
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     -5.0
 * @level       user
 */
#define SOX_TEMP_LOW_LIMIT_DISCHARGE -5.0

/*fox
 * the cold temperature where the derating of maximum charge current starts,
 * i.e., below this temperature battery pack should not deliver full charge current
 * @var         low temperature charge derating start
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     10.0
 * @level       user
 */
#define SOX_TEMP_LOW_CUTOFF_CHARGE 10.0

/*fox
 * the cold temperature where the derating of maximum charge current is fully applied,
 * i.e., below this temperature battery pack should not deliver any current in charge direction
 * @var         low temperature charge derating full
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     0.0
 * @level       user
 */
#define SOX_TEMP_LOW_LIMIT_CHARGE 0.0

/*fox
 * the hot temperature where the derating of maximum discharge current starts,
 * i.e., above this temperature battery pack should not deliver full discharge current
 * @var         high temperature discharge derating start
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     45.0
 * @level       user
 */
#define SOX_TEMP_HIGH_CUTOFF_DISCHARGE 45.0

/*fox
 * the hot temperature where the derating of maximum discharge current is fully applied,
 * i.e., above this temperature battery pack should not deliver any current in discharge direction
 * @var         high temperature discharge derating full
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     55.0
 * @level       user
 */
#define SOX_TEMP_HIGH_LIMIT_DISCHARGE 55.0

/*fox
 * the hot temperature where the derating of maximum charge current starts,
 * i.e., above this temperature battery pack should not deliver full charge current
 * @var         high temperature charge derating start
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     30.0
 * @level       user
 */
#define SOX_TEMP_HIGH_CUTOFF_CHARGE 30.0

/*fox
 * the hot temperature where the derating of maximum charge current is fully applied,
 * i.e., above this temperature battery pack should not deliver any current in charge direction
 * @var         high temperature charge derating full
 * @type        float
 * @valid       -40.0<x<80.0
 * @unit        °C
 * @group       SOX
 * @default     37.0
 * @level       user
 */
#define SOX_TEMP_HIGH_LIMIT_CHARGE 37.0

/*fox
 * above this SOC value battery pack should not be exposed to full current in charge direction
 * @var         high SOC derating starts
 * @type        int
 * @valid       0<=x<=10000
 * @unit        0.01%
 * @group       SOX
 * @default     8500
 * @level       user
 */
#define SOX_SOC_CUTOFF_CHARGE 8500

/*fox
 * above this SOC value battery pack should not be exposed to any current in charge direction
 * @var         high SOC derating full extent
 * @type        int
 * @valid       0<=x<=10000
 * @unit        0.01%
 * @group      SOX
 * @default    9500
 * @level       user
 */
#define SOX_SOC_LIMIT_CHARGE 9500

/*fox
 * below this SOC value battery pack should not deliver full current in discharge direction
 * @var         low SOC derating starts
 * @type        int
 * @valid       0<=x<=10000
 * @unit        0.01%
 * @group       SOX
 * @default     1500
 * @level       user
 */
#define SOX_SOC_CUTOFF_DISCHARGE 1500

/*fox
 * below this SOC value battery pack should not deliver any current in discharge direction
 * @var         low SOC derating full extent
 * @type        int
 * @valid       0<=x<=10000
 * @unit        0.01%
 * @group       SOX
 * @default     500
 * @level       user
 */
#define SOX_SOC_LIMIT_DISCHARGE 500

/*fox
 * above this voltage value battery pack should not be exposed to full current in charge direction
 * @var         high voltage derating starts
 * @type        int
 * @unit        mV
 * @group       SOX
 * @valid       [1000,5000]
 * @default     3300
 * @level       user
 */
#define SOX_VOLT_CUTOFF_CHARGE 3300

/*fox
 * above this voltage value battery pack should not be exposed to any current in charge direction
 * @var         high voltage derating full extent
 * @type        int
 * @unit        mV
 * @group       SOX
 * @valid       [1000,5000]
 * @default     3550
 * @level       user
 */
#define SOX_VOLT_LIMIT_CHARGE 3550

/*fox
 * below this voltage value battery pack should not deliver full current in discharge direction
 * @var         low voltage derating starts
 * @type        int
 * @unit        mV
 * @group       SOX
 * @valid       [1000,5000]
 * @default     2700
 * @level       user
 */
#define SOX_VOLT_CUTOFF_DISCHARGE 2700

/*fox
 * below this voltage value battery pack should not deliver any current in discharge direction
 * @var         low voltage derating full extent
 * @type        int
 * @unit        mV
 * @group       SOX
 * @valid       [1000,5000]
 * @default     2300
 * @level       user
 */
#define SOX_VOLT_LIMIT_DISCHARGE 2300

/*================== Constant and Variable Definitions ====================*/

/**
 * structure for configuration of SoF Calculation
 */
typedef struct {
    float I_DischaMax_Cont;
    float I_ChargeMax_Cont;
    float I_Limphome;

    float Cutoff_TLow_Discha;
    float Limit_TLow_Discha;

    float Cutoff_TLow_Charge;
    float Limit_TLow_Charge;

    float Cutoff_THigh_Discha;
    float Limit_THigh_Discha;

    float Cutoff_THigh_Charge;
    float Limit_THigh_Charge;

    float Cutoff_Soc_Charge ;
    float Limit_Soc_Charge;

    float Cutoff_Soc_Discha ;
    float Limit_Soc_Discha;

    float Cutoff_Voltage_Charge;
    float Limit_Voltage_Charge;

    float Cutoff_Voltage_Discha;
    float Limit_Voltage_Discha;
} SOX_SOF_CONFIG_s;

extern SOX_SOF_CONFIG_s sox_sof_config;

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/

#endif   // SOX_CFG_H_
