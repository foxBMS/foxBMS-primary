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
 * @file    contactor_cfg.c
 * @author  foxBMS Team
 * @date    23.09.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  CONT
 *
 * @brief   Configuration for the driver for the contactors
 *
 * The interlock pins (control and feedback) are mapped together as a
 * switch and and the same is done for the contactors. Furthermore for the
 * contactors the electrical contact of the feedbacks is set.
 * The interlock and all contactors are defined to be switched off at the
 * startup.
 *
 * The length of all configuration arrays are calculated to be later
 * checked in the contactor driver initialization against their length
 * definition in the foxygen macros in global.h
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
#include "contactor_cfg.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/


CONT_CFG_s cont_interlock_cfg = {
        CONT_INTERLOCK_CONTROL,         CONT_INTERLOCK_FEEDBACK,        CONT_FEEDBACK_TYPE_DONT_CARE
};
CONT_STATES_s cont_interlock_state = {
        {FALSE, 0},     {CONT_SWITCH_OFF, 0},   {CONT_SWITCH_OFF, 0}
};

CONT_CFG_s cont_contactors_cfg[BS_NR_OF_CONTACTORS] = {
        {CONT_PLUS_MAIN_CONTROL,                CONT_PLUS_MAIN_FEEDBACK,                CONT_FEEDBACK_NORMALLY_OPEN},
        {CONT_PLUS_PRECHARGE_CONTROL,           CONT_PLUS_PRECHARGE_FEEDBACK,           CONT_FEEDBACK_NORMALLY_OPEN},
        {CONT_MINUS_MAIN_CONTROL,               CONT_MINUS_MAIN_FEEDBACK,               CONT_FEEDBACK_NORMALLY_OPEN},
#if BS_SEPARATE_POWERLINES == 1
        {CONT_CHARGE_PLUS_MAIN_CONTROL,         CONT_CHARGE_PLUS_MAIN_FEEDBACK,         CONT_FEEDBACK_NORMALLY_OPEN},
        {CONT_CHARGE_PLUS_PRECHARGE_CONTROL,    CONT_CHARGE_PLUS_PRECHARGE_FEEDBACK,    CONT_FEEDBACK_NORMALLY_OPEN},
        {CONT_CHARGE_MINUS_MAIN_CONTROL,        CONT_CHARGE_MINUS_MAIN_FEEDBACK,        CONT_FEEDBACK_NORMALLY_OPEN}
#endif // BS_SEPARATE_POWERLINES == 1
};

CONT_STATES_s cont_contactor_states[BS_NR_OF_CONTACTORS] = {
        {{FALSE, 0},    {CONT_SWITCH_OFF, 0},   {CONT_SWITCH_OFF, 0}},
        {{FALSE, 0},    {CONT_SWITCH_OFF, 0},   {CONT_SWITCH_OFF, 0}},
        {{FALSE, 0},    {CONT_SWITCH_OFF, 0},   {CONT_SWITCH_OFF, 0}},
#if BS_SEPARATE_POWERLINES == 1
        {{FALSE, 0},    {CONT_SWITCH_OFF, 0},   {CONT_SWITCH_OFF, 0}},
        {{FALSE, 0},    {CONT_SWITCH_OFF, 0},   {CONT_SWITCH_OFF, 0}},
        {{FALSE, 0},    {CONT_SWITCH_OFF, 0},   {CONT_SWITCH_OFF, 0}}
#endif // BS_SEPARATE_POWERLINES == 1
};

const uint8_t cont_contactors_cfg_length = sizeof(cont_contactors_cfg)/sizeof(cont_contactors_cfg[0]);
const uint8_t cont_contactors_states_length = sizeof(cont_contactor_states)/sizeof(cont_contactor_states[0]);
/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/
