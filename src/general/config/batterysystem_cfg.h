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
 * @file    batterysystem_cfg.h
 * @author  foxBMS Team
 * @date    20.02.2017 (date of creation)
 * @ingroup BATTER_SYSTEM_CONF
 * @prefix  BS
 *
 * @brief   Configuration of the battery system (e.g., number of battery modules, battery cells, temperature sensors)
 *
 * This files contains basic macros of the battery system in order to derive needed inputs
 * in other parts of the software. These macros are all depended on the hardware.
 *
 */

#ifndef BATTERYSYSTEM_CFG_H_
#define BATTERYSYSTEM_CFG_H_

/*fox
 * number of modules in battery pack
 * @var number of battery modules
 * @type int
 * @default 2
 * @validator 0<x
 * @level user
 * @group GENERAL
 */
#define BS_NR_OF_MODULES                           1

/*fox
 * number of battery cells per battery module (parallel cells are counted as one)
 * @var number of battery cells per battery module
 * @type int
 * @default 12
 * @validator 0<x<=12
 * @level user
 * @group GENERAL
 */
#define BS_NR_OF_BAT_CELLS_PER_MODULE               12


/*fox
 * Number of LTC-ICs per battery module
 * @var number of LTC-ICs per battery module
 * @type int
 * @default 1
 * @validator 1<=x
 * @level user
 * @group GENERAL
 */
#define BS_NR_OF_LTC_PER_MODULE                     1
/*fox
 * number of temperature sensors per battery module
 * @var number of temperature sensors per battery module
 * @type int
 * @default 6
 * @validator 0<=x<=16
 * @level user
 * @group GENERAL
 */
#define BS_NR_OF_TEMP_SENSORS_PER_MODULE            5


#define BS_NR_OF_BAT_CELLS                         (BS_NR_OF_MODULES * BS_NR_OF_BAT_CELLS_PER_MODULE)
#define BS_NR_OF_TEMP_SENSORS                      (BS_NR_OF_MODULES * BS_NR_OF_TEMP_SENSORS_PER_MODULE)

/*fox
 * number of pack voltage inputs measured by current sensors (like IVT-MOD)
 * @var number of voltages measured by the current sensor
 * @type int
 * @default 3
 * @validator 0<=x<=3
 * @level devel
 * @group GENERAL
 */
#define BS_NR_OF_VOLTAGES_FROM_CURRENT_SENSOR      3

/*fox
 * number of contactors
 * On the foxBMS Basic Board 6 contactors are supported. On the foxBMS
 * Basic Extension board one can use 3 more contactors.
 * @var number of contactors
 * @type int
 * @default 3
 * @validator 0<x<=9
 * @level user
 * @group GENERAL
 */
#define BS_NR_OF_CONTACTORS                        6

/*fox
 * separation of charge and discharge power line
 * @var separation of charge and discharge power line
 * @level advanced
 * @type select(2)
 * @default 1
 * @group GENERAL
 */
//#define BS_SEPARATE_POWERLINES 0
#define BS_SEPARATE_POWERLINES 1

#endif /* BATTERYSYSTEM_CFG_H_ */
