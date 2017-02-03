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
 * @file global.h
 * @author  foxBMS Team
 * @date 25.02.2015 (date of creation)
 *
 * @ingroup GENERAL_CONF
 *
 * @brief Configuration of the battery system (e.g., number of battery modules, battery cells, temperature sensors)
 *
 * This files contains basic macros of the battery system in order to derive needed inputs
 * in other parts of the software. These macros are all depended on the hardware.
 *
 * The only exception is the a macro which allows to store variables in the backup SRAM.
 *
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

/*================== Includes =============================================*/

/*================== Macros and Definitions ===============================*/
/*fox
 * number of modules in battery pack
 * @var number of battery modules
 * @type int
 * @default 2
 * @validator 0<x
 * @level user
 * @group GENERAL
 */
#define NR_OF_MODULES                           1

/*fox
 * number of battery cells per battery module (parallel cells are counted as one)
 * @var number of battery cells per battery module
 * @type int
 * @default 12
 * @validator 0<x<=12
 * @level user
 * @group GENERAL
 */
#define NR_OF_BAT_CELLS_PER_MODULE              12

/*fox
 * number of temperature sensors per battery module
 * @var number of temperature sensors per battery module
 * @type int
 * @default 6
 * @validator 0<=x<=16
 * @level user
 * @group GENERAL
 */
#define NR_OF_TEMP_SENSORS_PER_MODULE           6


#define NR_OF_BAT_CELLS                         (NR_OF_MODULES*NR_OF_BAT_CELLS_PER_MODULE)
#define NR_OF_TEMP_SENSORS                      (NR_OF_MODULES*NR_OF_TEMP_SENSORS_PER_MODULE)

/*fox
 * number of pack voltage inputs measured by current sensors (like IVT-MOD)
 * @var number of voltages measured by the current sensor
 * @type int
 * @default 3
 * @validator 0<=x<=3
 * @level devel
 * @group GENERAL
 */
#define NR_OF_VOLTAGES_FROM_CURRENT_SENSOR      3

/*fox
 * number of contactors
 * On the foxBMS Basic Board 6 contactors are supported. On the foxBMS
 * Basic Extension board one can use 3 more contactors.
 * @var number of contactors
 * @type int
 * @default 3
 * @validator 0<x<=6
 * @level user
 * @group GENERAL
 */
#define NR_OF_CONTACTORS                        3


/**
 * A variable defined as ``(type) BKP_SRAM (name)`` will be stored in the
 * ram which is backuped by the battery. Therefore as long as the power
 * supply is not disconnected, the variable value will be stored during
 * e.g. restarts.
 */
#define BKP_SRAM    __attribute__((section (".BKP_RAMSection")))

/*================== Constant and Variable Definitions ====================*/


/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/


#endif
