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
 * @file    cansignal_cfg.c
 * @author  foxBMS Team
 * @date    16.09.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  CANS
 *
 * @brief   Configuration of the messages and signal settings for the CAN driver
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
#include "cansignal_cfg.h"

#include "database.h"
#include "syscontrol.h"
#include "mcu.h"
#include "sox.h"

/*================== Function Prototypes ==================================*/

static uint32_t cans_getmux(uint32_t, void *);
static uint32_t cans_setmux(uint32_t, void *);
static uint32_t cans_getupdatemux(uint32_t sigIdx, void *value);
static uint32_t cans_getvolt(uint32_t, void *);
static uint32_t cans_setvolt(uint32_t, void *);
static uint32_t cans_gettempering(uint32_t, void *);
static uint32_t cans_settempering(uint32_t, void *);
static uint32_t cans_getcanerr(uint32_t, void *);
static uint32_t cans_setcanerr(uint32_t, void *);
static uint32_t cans_gettemp(uint32_t, void *);
static uint32_t cans_settemp(uint32_t, void *);
static uint32_t cans_getsocsoh(uint32_t, void *);
static uint32_t cans_setsocsoh(uint32_t, void *);
static uint32_t cans_getsof(uint32_t, void *);
static uint32_t cans_setsof(uint32_t, void *);
static uint32_t cans_getcurr(uint32_t, void *);
static uint32_t cans_setcurr(uint32_t, void *);
static uint32_t cans_getminmaxvolt(uint32_t, void *);
static uint32_t cans_setminmaxvolt(uint32_t, void *);
static uint32_t cans_gettriggercurrent(uint32_t sigIdx, void *value);
static uint32_t cans_getstaterequest(uint32_t, void *);
static uint32_t cans_setstaterequest(uint32_t, void *);
static uint32_t cans_getisoguard(uint32_t, void *);
static uint32_t cans_getdebug(uint32_t, void *);
static uint32_t cans_setdebug(uint32_t, void *);

/*================== Macros and Definitions ===============================*/

const uint32_t cans_signalToMuxMapping[] = {
        CAN0_SIG_BMS2_Mux,
        CAN0_SIG_BMS3_Mux,
        CAN0_SIG_BMS4_Mux,
        CAN0_SIG_BMS5_Mux,
        CAN0_SIG_BMS11_Mux,
        CAN0_SIG_BMS12_Mux,
        CAN0_SIG_BMS13_Mux };
static uint8_t cans_muxVal[sizeof(cans_signalToMuxMapping)/sizeof(cans_signalToMuxMapping[0])];

static DATA_BLOCK_CURRENT_s cans_current_tab;

/*================== Constant and Variable Definitions ====================*/

const CANS_signal_s cans_CAN0_signals_tx[] = {
        { {CAN0_MSG_BMS1}, 0, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<    CAN_SIGNAL_general_error, // only 2 bits
        { {CAN0_MSG_BMS1}, 2, 6, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<    CAN_SIGNAL_current_state, // only 2 bits
        { {CAN0_MSG_BMS1}, 8, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<    CAN_SIGNAL_error_overtemp_bat, // only 2 bits
        { {CAN0_MSG_BMS1}, 10, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_overtemp_ic, // only 2 bits
        { {CAN0_MSG_BMS1}, 12, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_overtemp_charge, // only 2 bits
        { {CAN0_MSG_BMS1}, 14, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_overtemp_discharge, // only 2 bits
        { {CAN0_MSG_BMS1}, 16, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_overvoltage, // only 2 bits
        { {CAN0_MSG_BMS1}, 18, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_undervoltage, // only 2 bits
        { {CAN0_MSG_BMS1}, 20, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_relays, // only 2 bits
        { {CAN0_MSG_BMS1}, 22, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_selftest, // only 2 bits
        { {CAN0_MSG_BMS1}, 24, 6, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_states_relays,
        { {CAN0_MSG_BMS1}, 30, 2, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_error_cantiming,
        { {CAN0_MSG_BMS1}, 49, 1, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_balancing_active,
        { {CAN0_MSG_BMS1}, 52, 4, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   CAN_SIGNAL_status_msg_overflowing_counter,
        { {CAN0_MSG_BMS1}, 56, 8, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<   reserved
        { {CAN0_MSG_BMS2}, 56, 8, 0, 0, 1, 0, FALSE,FALSE, FALSE, {CAN0_SIGNAL_NONE},&cans_setcanerr, &cans_getcanerr }, //!<
        { {CAN0_MSG_BMS2},  0, 4, 15, 0, 1, 0, FALSE , FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux }, //!< Cell Voltages of Cells 0 1 2 of Module 0
        { {CAN0_MSG_BMS2},  4, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 0 of Module 0
        { {CAN0_MSG_BMS2}, 20, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 1 of Module 0
        { {CAN0_MSG_BMS2}, 36, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 2 of Module 0
        { {CAN0_MSG_BMS3}, 0, 4, 0, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux }, //!< Cell Voltages of Cells 3 4 5 of Module 0
        { {CAN0_MSG_BMS3}, 4, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 3 of Module 0
        { {CAN0_MSG_BMS3}, 20, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt}, //!< Voltage of Cell 4 of Module 0
        { {CAN0_MSG_BMS3}, 36, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt}, //!< Voltage of Cell 5 of Module 0
        { {CAN0_MSG_BMS4}, 0, 4, 0, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux }, //!< Cell Voltages of Cells 6 7 8 of Module 0
        { {CAN0_MSG_BMS4}, 4, 16,  5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 6 of Module 0
        { {CAN0_MSG_BMS4}, 20, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 7 of Module 0
        { {CAN0_MSG_BMS4}, 36, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 8 of Module 0
        { {CAN0_MSG_BMS5}, 0, 4, 0, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux }, //!< Cell Voltages of Cells 9 10 11 of Module 0
        { {CAN0_MSG_BMS5}, 4, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 9 of Module 0
        { {CAN0_MSG_BMS5}, 20, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 10 of Module 0
        { {CAN0_MSG_BMS5}, 36, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 11 of Module 0
        { {CAN0_MSG_BMS6}, 0, 1, 1, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Flag Signal if Cooling needed
        { {CAN0_MSG_BMS6}, 1, 1, 1, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Flag Signal if Heating needed
        { {CAN0_MSG_BMS6}, 2, 6, 31, -32, 1, -32, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Need for Tempering
        { {CAN0_MSG_BMS6}, 8, 8, 87.5, -40, 0.5, -40, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Average Temp
        { {CAN0_MSG_BMS6}, 16, 8, 87.5, -40, 0.5, -40, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Minimum Temp
        { {CAN0_MSG_BMS6}, 24, 8, 87.5, -40, 0.5, -40, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Maximum Temp
        { {CAN0_MSG_BMS6}, 32, 4, 15, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Module Number of Module with highest temperature
        { {CAN0_MSG_BMS6}, 36, 4, 15, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_settempering, &cans_gettempering }, //!< Module Number of Module with lowest temperature
        { {CAN0_MSG_BMS6}, 40, 24, 1, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!< reserved
        { {CAN0_MSG_BMS7}, 0, 8, 100, 0, 0.5, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsocsoh, &cans_getsocsoh }, //!< State of Charge averaged over all cells
        { {CAN0_MSG_BMS7}, 8, 8, 100, 0, 0.5, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsocsoh, &cans_getsocsoh }, //!< State of Charge minimum of all cells
        { {CAN0_MSG_BMS7}, 16, 8, 100, 0, 0.5, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsocsoh, &cans_getsocsoh }, //!< State of Charge maximum of all cells
        { {CAN0_MSG_BMS7}, 24, 8, 100, 0, 0.5, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsocsoh, &cans_getsocsoh }, //!< State of Health averaged over all cells
        { {CAN0_MSG_BMS7}, 32, 8, 100, 0, 0.5, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsocsoh, &cans_getsocsoh }, //!< State of Health minimum of all cells
        { {CAN0_MSG_BMS7}, 40, 8, 100, 0, 0.5, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsocsoh, &cans_getsocsoh }, //!< State of Health maximum of all cells
        { {CAN0_MSG_BMS7}, 48, 16, 1, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!< reserved
        { {CAN0_MSG_BMS8}, 0, 16,  65535, 0, 0.01, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsof, &cans_getsof },  //!< SOF charge continuous
        { {CAN0_MSG_BMS8}, 16, 16, 65535, 0, 0.01, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsof, &cans_getsof },  //!< SOF charge peak
        { {CAN0_MSG_BMS8}, 32, 16, 65535, 0, 0.01, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsof, &cans_getsof }, //!< SOF discharge continuous
        { {CAN0_MSG_BMS8}, 48, 16, 65535, 0, 0.01, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setsof, &cans_getsof }, //!< SOF discharge peak
        { {CAN0_MSG_BMS9}, 0, 1, 1, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, &cans_getisoguard }, //!< State of IsoGood Pin of Bender
        { {CAN0_MSG_BMS9}, 1, 7, 127, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, &cans_getisoguard }, //!< Isolation PWM value of Bender
        { {CAN0_MSG_BMS9}, 8, 4, 15, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!< Module number of balancing active indicator
        { {CAN0_MSG_BMS9}, 12, 12, 4095, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!< Balancing Active indication bitfield
        { {CAN0_MSG_BMS9}, 40, 24, 16777215, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!<
        { {CAN0_MSG_BMS9}, 24, 8, 1, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!< tbd
        { {CAN0_MSG_BMS9}, 32, 8, 1, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!<
        { {CAN0_MSG_BMS11}, 0, 4, 12, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux }, //!< Cell Temperatures of Modules
        { {CAN0_MSG_BMS11}, 60, 4, 15, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!< reserved
        { {CAN0_MSG_BMS11}, 4, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 0 of Module 0
        { {CAN0_MSG_BMS11}, 11, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 1 of Module 0
        { {CAN0_MSG_BMS11}, 18, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 2 of Module 0
        { {CAN0_MSG_BMS11}, 25, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 3 of Module 0
        { {CAN0_MSG_BMS11}, 32, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 4 of Module 0
        { {CAN0_MSG_BMS11}, 39, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 5 of Module 0
        { {CAN0_MSG_BMS11}, 46, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 6 of Module 0
        { {CAN0_MSG_BMS11}, 53, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 7 of Module 0
        { {CAN0_MSG_BMS12}, 56, 8, 0, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!<
        { {CAN0_MSG_BMS12}, 0, 4, 0, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux }, //!< Cell Voltages of Cells 12 13 14 of Module 0
        { {CAN0_MSG_BMS12}, 4, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 13 of Module 0
        { {CAN0_MSG_BMS12}, 20, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 14 of Module 0
        { {CAN0_MSG_BMS12}, 36, 16, 5000, 0, 1, 0, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_setvolt, &cans_getvolt }, //!< Voltage of Cell 15 of Module 0
        { {CAN0_MSG_BMS13}, 0, 4, 0, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setmux, &cans_getupdatemux }, //!<
        { {CAN0_MSG_BMS13}, 60, 4, 15, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, NULL_PTR }, //!< reserved
        { {CAN0_MSG_BMS13}, 4, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 8 of Module 0
        { {CAN0_MSG_BMS13}, 11, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 9 of Module 0
        { {CAN0_MSG_BMS13}, 18, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 10 of Module 0
        { {CAN0_MSG_BMS13}, 25, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 11 of Module 0
        { {CAN0_MSG_BMS13}, 32, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 12 of Module 0
        { {CAN0_MSG_BMS13}, 39, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 13 of Module 0
        { {CAN0_MSG_BMS13}, 46, 7, 87, -40, 1, -40, FALSE, FALSE, 0, {CAN0_SIGNAL_NONE}, &cans_settemp, &cans_gettemp }, //!< Temp 14 of Module 0
        { {CAN0_MSG_BMS14}, 0, 16, 65535, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setminmaxvolt, &cans_getminmaxvolt }, //!< Average Voltage of Cells
        { {CAN0_MSG_BMS14}, 16, 16, 65535, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setminmaxvolt, &cans_getminmaxvolt }, //!< Minimum Voltage of Cells
        { {CAN0_MSG_BMS14}, 32, 16, 65535, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setminmaxvolt, &cans_getminmaxvolt }, //!< Maximum Voltage of Cells
        { {CAN0_MSG_BMS14}, 48, 4, 15, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setminmaxvolt, &cans_getminmaxvolt }, //!< ModuleNr with Minimum Voltage
        { {CAN0_MSG_BMS14}, 52, 4, 15, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, &cans_setminmaxvolt, &cans_getminmaxvolt } //!< ModuleNr with Maximum Voltage
#ifdef CAN_ISABELLENHUETTE_TRIGGERED
        ,{ {CAN0_MSG_BMS_CurrentTrigger}, 0, 32, 0, 0, 1, 0, FALSE, FALSE, FALSE, {CAN0_SIGNAL_NONE}, NULL_PTR, &cans_gettriggercurrent } //!< ModuleNr with Maximum Voltage
#endif
};


const CANS_signal_s cans_CAN1_signals_tx[] = {
};


const CANS_signal_s cans_CAN0_signals_rx[] = {
        { {CAN0_MSG_BMS10},   0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_BMS10},   8,  8,    8, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, &cans_setstaterequest,  &cans_getstaterequest },
        { {CAN0_MSG_ISENS0},  0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS0},  8,  8,  255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS0}, 16, 32, 0xFFFFFFFF, -3000, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, &cans_setcurr,          &cans_getcurr },
        { {CAN0_MSG_ISENS1},  0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS1},  8,  8,    255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS1}, 16, 32,  0xFFFFFFFF, 0, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, &cans_setcurr,          &cans_getcurr },
        { {CAN0_MSG_ISENS2},  0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS2},  8,  8,    255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS2}, 16, 32,  0xFFFFFFFF, 0, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, &cans_setcurr,          &cans_getcurr },
        { {CAN0_MSG_ISENS3},  0,  8,    32, 0, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS3},  8,  8,    255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN0_MSG_ISENS3}, 16, 32,  0xFFFFFFFF, 0, 0.001,    0,     FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, &cans_setcurr,       &cans_getcurr },
        { {CAN0_MSG_DEBUG},   0, 64,  100, 0, 0.001,    0,     FALSE,  FALSE,  FALSE,  {CAN0_SIGNAL_NONE}, &cans_setdebug,      &cans_getdebug }
};

const CANS_signal_s cans_CAN1_signals_rx[] = {
        { {CAN1_MSG_BMS10},   0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_BMS10},   8,  8,    8, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, &cans_setstaterequest,  &cans_getstaterequest },
        { {CAN1_MSG_ISENS0},  0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS0},  8,  8,    255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS0}, 16, 32,  0xFFFFFFFF, 0, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, &cans_setcurr,          &cans_getcurr },
        { {CAN1_MSG_ISENS1},  0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS1},  8,  8,    255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS1}, 16, 32,  0xFFFFFFFF, 0, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, &cans_setcurr,          &cans_getcurr },
        { {CAN1_MSG_ISENS2},  0,  8,    32, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS2},  8,  8,    255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS2}, 16, 32,  0xFFFFFFFF, 0, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, &cans_setcurr,          &cans_getcurr },
        { {CAN1_MSG_ISENS3},  0,  8,    32, 0, 0.001,    0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS3},  8,  8,    255, 0, 1,        0,  FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, NULL_PTR,               NULL_PTR },
        { {CAN1_MSG_ISENS3}, 16, 32,  0xFFFFFFFF, 0, 0.001,    0,     FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, &cans_setcurr,       &cans_getcurr },
        { {CAN1_MSG_DEBUG},   0, 64,  100, 0, 0.001,    0,     FALSE,  FALSE,  FALSE,  {CAN1_SIGNAL_NONE}, &cans_setdebug,      &cans_getdebug }
};


const uint16_t cans_CAN0_signals_tx_length = sizeof(cans_CAN0_signals_tx)/sizeof(cans_CAN0_signals_tx[0]);
const uint16_t cans_CAN1_signals_tx_length = sizeof(cans_CAN1_signals_tx)/sizeof(cans_CAN1_signals_tx[0]);

const uint16_t cans_CAN0_signals_rx_length = sizeof(cans_CAN0_signals_rx)/sizeof(cans_CAN0_signals_rx[0]);
const uint16_t cans_CAN1_signals_rx_length = sizeof(cans_CAN1_signals_rx)/sizeof(cans_CAN1_signals_rx[0]);

/*================== Function Implementations =============================*/

uint32_t cans_setmux(uint32_t sigIdx, void *value) {
    uint32_t i, locMuxIdx =0xFFFFFFFF;
    for (i = 0; i< sizeof(cans_signalToMuxMapping)/sizeof(cans_signalToMuxMapping[0]); i++){
        if (sigIdx == cans_signalToMuxMapping[i]) locMuxIdx=i;
    }
    if(value != NULL_PTR) {
        cans_muxVal[locMuxIdx] = *(uint8_t*) value;
    }
    return 0;
}
uint32_t cans_getmux(uint32_t sigIdx, void *value) {
    uint32_t i, locMuxIdx =0xFFFFFFFF;
    for (i = 0; i< sizeof(cans_signalToMuxMapping)/sizeof(cans_signalToMuxMapping[0]); i++){
        if (sigIdx == cans_signalToMuxMapping[i]) locMuxIdx=i;
    }
    if (locMuxIdx != 0xFFFFFFFF){
        if(value != NULL_PTR) {
            *(uint32_t *)value = (uint32_t)cans_muxVal[locMuxIdx];
        }
    }
    return 0;
}
uint32_t cans_getupdatemux(uint32_t sigIdx, void *value) {
    uint32_t i, locMuxIdx =0xFFFFFFFF;
    for (i = 0; i< sizeof(cans_signalToMuxMapping)/sizeof(cans_signalToMuxMapping[0]); i++){
        if (sigIdx == cans_signalToMuxMapping[i]) locMuxIdx=i;
    }
    if (locMuxIdx != 0xFFFFFFFF){
        if(value != NULL_PTR) {
            cans_muxVal[locMuxIdx]++;
            if (cans_muxVal[locMuxIdx] >=BS_NR_OF_MODULES){
                cans_muxVal[locMuxIdx] = 0;
            }
            *(uint32_t *)value = (uint32_t)cans_muxVal[locMuxIdx];
        }
    }
    return 0;
}
uint32_t cans_setvolt(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}
uint32_t cans_getvolt(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_CELLVOLTAGE_s volt_tab;
    uint32_t cellIdx, muxIdx, muxVal=0;
    DATA_GetTable(&volt_tab, DATA_BLOCK_ID_CELLVOLTAGE);
    switch (sigIdx){
        case CAN0_SIG_BMS2__Mod0_Cell0_Value:
        case CAN0_SIG_BMS2__Mod0_Cell1_Value:
        case CAN0_SIG_BMS2__Mod0_Cell2_Value:
            cellIdx = sigIdx - CAN0_SIG_BMS2__Mod0_Cell0_Value;
            muxIdx = CAN0_SIG_BMS2_Mux;
            break;
        case CAN0_SIG_BMS3__Mod0_Cell3_Value:
        case CAN0_SIG_BMS3__Mod0_Cell4_Value:
        case CAN0_SIG_BMS3__Mod0_Cell5_Value:
            cellIdx = sigIdx - CAN0_SIG_BMS3__Mod0_Cell3_Value + 3;
            muxIdx = CAN0_SIG_BMS3_Mux;
            break;
        case CAN0_SIG_BMS4__Mod0_Cell6_Value:
        case CAN0_SIG_BMS4__Mod0_Cell7_Value:
        case CAN0_SIG_BMS4__Mod0_Cell8_Value:
            cellIdx = sigIdx - CAN0_SIG_BMS4__Mod0_Cell6_Value + 6;
            muxIdx = CAN0_SIG_BMS4_Mux;
            break;
        case CAN0_SIG_BMS5__Mod0_Cell9_Value:
        case CAN0_SIG_BMS5__Mod0_Cell10_Value:
        case CAN0_SIG_BMS5__Mod0_Cell11_Value:
            cellIdx = sigIdx - CAN0_SIG_BMS5__Mod0_Cell9_Value + 9;
            muxIdx = CAN0_SIG_BMS5_Mux;
            break;
#if NR_OF_BAT_CELLS_PER_MODULE > 12
        case CAN0_SIG_BMS12__Mod0_Cell12_Value:
        case CAN0_SIG_BMS12__Mod0_Cell13_Value:
        case CAN0_SIG_BMS12__Mod0_Cell14_Value:
            cellIdx = sigIdx - CAN0_SIG_BMS12__Mod0_Cell12_Value + 12;
            muxIdx = CANS_SIG_BMS12_Mux;
            break;
#else
        case CAN0_SIG_BMS12__Mod0_Cell12_Value:
        case CAN0_SIG_BMS12__Mod0_Cell13_Value:
        case CAN0_SIG_BMS12__Mod0_Cell14_Value:
            *(uint32_t *)value = 0;
            return 0;
#endif
    }
    cans_getmux(muxIdx,&muxVal);
    cellIdx = muxVal * BS_NR_OF_BAT_CELLS_PER_MODULE + cellIdx;
    if(value != NULL_PTR) {
        *(uint32_t *)value = volt_tab.voltage[cellIdx];
    }
    return 0;
}

uint32_t cans_settemp(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}
uint32_t cans_gettemp(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_CELLTEMPERATURE_s temp_tab;
    uint32_t cellIdx, muxIdx, muxVal=0;
    DATA_GetTable(&temp_tab, DATA_BLOCK_ID_CELLTEMPERATURE);
    switch (sigIdx){
        case CAN0_SIG_BMS11_Temp_Mod0_Temp0:
        case CAN0_SIG_BMS11_Temp_Mod0_Temp1:
        case CAN0_SIG_BMS11_Temp_Mod0_Temp2:
        case CAN0_SIG_BMS11_Temp_Mod0_Temp3:
        case CAN0_SIG_BMS11_Temp_Mod0_Temp4:
        case CAN0_SIG_BMS11_Temp_Mod0_Temp5:
        case CAN0_SIG_BMS11_Temp_Mod0_Temp6:
        case CAN0_SIG_BMS11_Temp_Mod0_Temp7:
            cellIdx = sigIdx - CAN0_SIG_BMS11_Temp_Mod0_Temp0;
            muxIdx = CAN0_SIG_BMS11_Mux;
            break;
        case CAN0_SIG_BMS13_Temp_Mod0_Temp8:
        case CAN0_SIG_BMS13_Temp_Mod0_Temp9:
        case CAN0_SIG_BMS13_Temp_Mod0_Temp10:
        case CAN0_SIG_BMS13_Temp_Mod0_Temp11:
        case CAN0_SIG_BMS13_Temp_Mod0_Temp12:
        case CAN0_SIG_BMS13_Temp_Mod0_Temp13:
        case CAN0_SIG_BMS13_Temp_Mod0_Temp14:
            cellIdx = sigIdx - CAN0_SIG_BMS13_Temp_Mod0_Temp8 + 8;
            muxIdx = CAN0_SIG_BMS13_Mux;
            break;

    }
    cans_getmux(muxIdx,&muxVal);
    cellIdx = muxVal * BS_NR_OF_TEMP_SENSORS_PER_MODULE + cellIdx;
    if(value != NULL_PTR) {
        //temperature: -40 degrees offset
        *(uint32_t *)value = temp_tab.temperature[cellIdx] + 40;
    }
    return 0;
}
uint32_t cans_settempering(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}
uint32_t cans_gettempering(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_MINMAX_s minmax_tab;
    DATA_GetTable(&minmax_tab, DATA_BLOCK_ID_MINMAX);
    if(value != NULL_PTR) {
        switch(sigIdx){
            case CAN0_SIG_BMS6_Average_Temp:
                *(uint32_t *)value = (minmax_tab.temperature_mean+40)*2;
                break;
            case CAN0_SIG_BMS6_Minimum_Temp:
                *(uint32_t *)value = (minmax_tab.temperature_min+40)*2;
                break;
            case CAN0_SIG_BMS6_Maximum_Temp:
                *(uint32_t *)value = (minmax_tab.temperature_max+40)*2;
                break;
            case CAN0_SIG_BMS6_MaxTemp_Module_Number:
                *(uint32_t *)value = minmax_tab.temperature_module_number_max;
                break;
            case CAN0_SIG_BMS6_MinTemp_Module_Number:
                *(uint32_t *)value = minmax_tab.temperature_module_number_min;
                break;
            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}
uint32_t cans_setcanerr(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}
uint32_t cans_getcanerr(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_CANERRORSIG_s canerr_tab;
    static DATA_BLOCK_BALANCING_CONTROL_s balancing_tab;
    DATA_GetTable(&canerr_tab, DATA_BLOCK_ID_CANERRORSIG);
    DATA_GetTable(&balancing_tab, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
    if(value != NULL_PTR) {
        switch(sigIdx){
            case CAN0_SIG_BMS1_states_relays:
                *(uint32_t *)value = canerr_tab.contactor_state_feedback;
                break;
            case CAN0_SIG_BMS1_balancing_active:
                *(uint32_t *)value = balancing_tab.enable_balancing;
                break;
            case CAN0_SIG_BMS1_error_cantiming:
                *(uint32_t *)value = canerr_tab.error_cantiming;
                break;
            case CAN0_SIG_BMS1_error_overvoltage:
                *(uint32_t *)value = canerr_tab.error_highvolt;
                break;
            case CAN0_SIG_BMS1_error_undervoltage:
                *(uint32_t *)value = canerr_tab.error_lowvolt;
                break;
            case CAN0_SIG_BMS1_error_overcurrent_charge:
                *(uint32_t *)value = canerr_tab.error_overcurrent_charge;
                break;
            case CAN0_SIG_BMS1_error_overcurrent_discharge:
                *(uint32_t *)value = canerr_tab.error_overcurrent_discharge;
                break;
            case CAN0_SIG_BMS1_error_hightemp_bat:
                *(uint32_t *)value = canerr_tab.error_hightemp;
                break;
            case CAN0_SIG_BMS1_error_lowtemp_bat:
                *(uint32_t *)value = canerr_tab.error_lowtemp;
                break;
            case CAN0_SIG_BMS1_error_contactor:
                *(uint32_t *)value = canerr_tab.contactor_error;
                break;
            case CAN0_SIG_BMS1_general_error:
                //If interlock open: general error
                if (canerr_tab.interlock_open == 1) {
                    *(uint32_t *)value = 1;
                }
                else if (canerr_tab.error_cantiming == 2) {
                    *(uint32_t *)value = 1;
                }
                else {
                    *(uint32_t *)value = canerr_tab.contactor_error | \
                                        canerr_tab.error_lowtemp    | \
                                        canerr_tab.error_lowvolt    | \
                                        canerr_tab.error_hightemp    | \
                                        canerr_tab.error_highvolt    | \
                                        canerr_tab.error_overcurrent_charge    | \
                                        canerr_tab.error_overcurrent_discharge;
                }
                break;
            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}
uint32_t cans_setsocsoh(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}
uint32_t cans_getsocsoh(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_SOX_s sox_tab;
    DATA_GetTable(&sox_tab, DATA_BLOCK_ID_SOX);
    if(value != NULL_PTR) {
        switch(sigIdx){
            case CAN0_SIG_BMS7_SOC_Average:
                //CAN signal resolution 0,5%, GetSoc Unit 0.01% --> factor 50
                //*(uint32_t *)value = (sox_tab.soc_mean*100.0)/50.0;
                *(uint32_t *)value = sox_tab.soc_mean*2.0;
                break;
            case CAN0_SIG_BMS7_SOC_Minimum:
                //CAN signal resolution 0,5%, GetSoc Unit 0.01% --> factor 50
                //*(uint32_t *)value = (sox_tab.soc_min*100.0)/50.0;
                *(uint32_t *)value = sox_tab.soc_min*2.0;
                break;
            case CAN0_SIG_BMS7_SOC_Maximum:
                //CAN signal resolution 0,5%, GetSoc Unit 0.01% --> factor 50
                //*(uint32_t *)value = (sox_tab.soc_max*100.0)/50.0;
                *(uint32_t *)value = sox_tab.soc_max*2.0;
                break;
            default:
                *(uint32_t *)value = 50.0;
                break;
        }
    }
    return 0;
}
uint32_t cans_setsof(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}
uint32_t cans_getsof(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_SOX_s sox_tab;
    DATA_GetTable(&sox_tab, DATA_BLOCK_ID_SOX);
    if(value != NULL_PTR) {
        // values transmitted in resolution of 10mA (16bit means 0A-655.35A)
        switch(sigIdx){
            case CAN0_SIG_BMS8_Current_Charge_Max:
                *(uint32_t *)value = sox_tab.sof_continuous_charge*100.0;
                break;
            case CAN0_SIG_BMS8_Current_Charge_Max_Peak:
            *(uint32_t *)value = sox_tab.sof_peak_charge*100.0;
                break;
            case CAN0_SIG_BMS8_Current_Discharge_Max:
            *(uint32_t *)value = sox_tab.sof_continuous_discharge*100.0;
                break;
            case CAN0_SIG_BMS8_Current_Discharge_Max_Peak:
            *(uint32_t *)value = sox_tab.sof_peak_discharge*100.0;
                break;
        }
    }
    return 0;
}
uint32_t cans_setminmaxvolt(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}
uint32_t cans_getminmaxvolt(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_MINMAX_s minmax_tab;
    DATA_GetTable(&minmax_tab, DATA_BLOCK_ID_MINMAX);
    if(value != NULL_PTR) {
        switch(sigIdx){
            case CAN0_SIG_BMS14_VoltageAverage:
                *(uint32_t *)value = minmax_tab.voltage_mean;
                break;
            case CAN0_SIG_BMS14_VoltageMinimum:
                *(uint32_t *)value = minmax_tab.voltage_min;
                break;
            case CAN0_SIG_BMS14_VoltageMaximum:
                *(uint32_t *)value = minmax_tab.voltage_max;
                break;
            case CAN0_SIG_BMS14_ModuleNrMinVolt:
                *(uint32_t *)value = minmax_tab.voltage_module_number_min;
                break;
            case CAN0_SIG_BMS14_ModuleNrMaxVolt:
                *(uint32_t *)value = minmax_tab.voltage_module_number_max;
                break;
            default:
                *(uint32_t *)value = 0;
                break;
        }
    }
    return 0;
}
uint32_t cans_gettriggercurrent(uint32_t sigIdx, void *value){
    *(uint32_t *)value = 0x00FFFF31;
    return 0;
}

uint32_t cans_setcurr(uint32_t sigIdx, void *value) {
    int32_t currentValue;
    int32_t voltageValue[3];
    uint32_t idx =0;
    uint8_t dummy[4] = {0,0,0,0};
    dummy[0] = *(uint32_t *)value & 0x000000FF;
    dummy[1] = (*(uint32_t *)value & 0x0000FF00) >> 8;
    dummy[2] = (*(uint32_t *)value & 0x00FF0000) >> 16;
    dummy[3] = (*(uint32_t *)value & 0xFF000000) >> 24;

    if(value != NULL_PTR) {
        switch(sigIdx){
            case CAN0_SIG_ISENS0_I_Measurement:
            // case CAN1_SIG_ISENS0_I_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct
                currentValue = (int32_t)(dummy[3] | dummy[2] << 8
                        | dummy[1] << 16 | dummy[0] << 24);
                cans_current_tab.previous_timestamp = cans_current_tab.timestamp;
                cans_current_tab.timestamp = MCU_GetTimeStamp();
                cans_current_tab.current=(float)(currentValue);
                cans_current_tab.state_current++;
                DATA_StoreDataBlock(&cans_current_tab,DATA_BLOCK_ID_CURRENT);
                break;
            case CAN0_SIG_ISENS1_U1_Measurement:
            // case CAN1_SIG_ISENS1_U1_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct
                idx =0;
                voltageValue[idx] = (int32_t)(dummy[3] | dummy[2] << 8
                        | dummy[1] << 16 | dummy[0] << 24);
                cans_current_tab.voltage[idx]=(float)(voltageValue[idx])*cans_CAN0_signals_rx[sigIdx].factor;
                cans_current_tab.state_voltage++;
                DATA_StoreDataBlock(&cans_current_tab,DATA_BLOCK_ID_CURRENT);
                break;
            case CAN0_SIG_ISENS2_U2_Measurement:
            // case CAN1_SIG_ISENS2_U2_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct
                idx = 1;
                voltageValue[idx] = (int32_t)(dummy[3] | dummy[2] << 8
                        | dummy[1] << 16 | dummy[0] << 24);
                cans_current_tab.voltage[idx]=(float)(voltageValue[idx])*cans_CAN0_signals_rx[sigIdx].factor;
                cans_current_tab.state_voltage++;
                DATA_StoreDataBlock(&cans_current_tab,DATA_BLOCK_ID_CURRENT);
                break;
            case CAN0_SIG_ISENS3_U3_Measurement:
            // case CAN1_SIG_ISENS3_U3_Measurement:  uncommented because identical position in CAN0 and CAN1 rx signal struct
                idx = 2;
                voltageValue[idx] = (int32_t)(dummy[3] | dummy[2] << 8
                        | dummy[1] << 16 | dummy[0] << 24);
                cans_current_tab.voltage[idx]=(float)(voltageValue[idx])*cans_CAN0_signals_rx[sigIdx].factor;
                cans_current_tab.state_voltage++;
                DATA_StoreDataBlock(&cans_current_tab,DATA_BLOCK_ID_CURRENT);
                break;
        }
    }
    return 0;
}
uint32_t cans_getcurr(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}

uint32_t cans_setstaterequest(uint32_t sigIdx, void *value) {
    DATA_BLOCK_STATEREQUEST_s staterequest_tab;
    uint8_t staterequest;

    DATA_GetTable(&staterequest_tab,DATA_BLOCK_ID_STATEREQUEST);

    if(value != NULL_PTR) {
        if (sigIdx == CAN0_SIG_BMS10_Request || sigIdx == CAN1_SIG_BMS10_Request ) {
            staterequest = *(uint8_t *)value;
            staterequest_tab.previous_state_request = staterequest_tab.state_request;
            staterequest_tab.state_request = staterequest;
            if ((staterequest_tab.state_request != staterequest_tab.previous_state_request)|| \
                    (MCU_GetTimeStamp()- staterequest_tab.timestamp) > 3000){
                staterequest_tab.state_request_pending = staterequest;
            }
            staterequest_tab.previous_timestamp = staterequest_tab.timestamp;
            staterequest_tab.timestamp = MCU_GetTimeStamp();
            staterequest_tab.state++;
            DATA_StoreDataBlock(&staterequest_tab,DATA_BLOCK_ID_STATEREQUEST);
            SYSCRTL_VCUPresent(SYSCTRL_VCUPRESENCE_OK);
        }
    }
    return 0;
}
uint32_t cans_getstaterequest(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}


uint32_t cans_getisoguard(uint32_t sigIdx, void *value) {
    static DATA_BLOCK_ISOMETER_s isoguard_tab;
    DATA_GetTable(&isoguard_tab, DATA_BLOCK_ID_ISOGUARD);
    if(value != NULL_PTR) {
        if (sigIdx == CAN0_SIG_BMS9_Isolation_Good) {
            *(uint8_t *)value = isoguard_tab.state;
        } else if (sigIdx == CAN0_SIG_BMS9_Isolation_Value) {
            *(uint8_t *)value = isoguard_tab.resistance;
        }
    }
    return 0;
}


uint32_t cans_setdebug(uint32_t sigIdx, void *value) {

    uint8_t data[8] = {0,0,0,0,0,0,0,0};
    static DATA_BLOCK_BALANCING_CONTROL_s balancing_tab;


    data[0] = *(uint64_t *)value & 0x00000000000000FF;
    data[1] = (*(uint64_t *)value & 0x000000000000FF00) >> 8;
    data[2] = (*(uint64_t *)value & 0x0000000000FF0000) >> 16;
    data[3] = (*(uint64_t *)value & 0x00000000FF000000) >> 24;
    data[4] = (*(uint64_t *)value & 0x000000FF00000000) >> 32;
    data[5] = (*(uint64_t *)value & 0x0000FF0000000000) >> 40;
    data[6] = (*(uint64_t *)value & 0x00FF000000000000) >> 48;
    data[7] = (*(uint64_t *)value & 0xFF00000000000000) >> 56;


    if(value != NULL_PTR) {
        switch(data[0]){
            case 11: // Set Soc directly with value. Unit in CAN message: 0.01 percent --> range 0...10000 means 0%Soc...100%Soc
                SOC_SetValue( ((float)((data[1])<<8 | (data[2])))/100.0 ); //divide by 100 to get SOC between 0 and 100
                break;
            case 14: // debug Message for Balancing on pack level
                // dataptr[1];          balancing request
                // dataptr[2]/1000.0;   balancing threshold, mV -> V
                //LTC_BalancingEnablePack(dataptr[1],(((float)(dataptr[2]))/1000.0));
                if (data[1] == 1) {
                    DATA_GetTable(&balancing_tab, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
                    balancing_tab.enable_balancing=1;
                    balancing_tab.threshold = data[2]; //get threshold in mV
                    /*for(int i = 0; i<12; i++)   // get cells to be balanced
                    {
                        balancing_tab.value[i] = (data[3]>>i)&0x01;
                    }*/
                    DATA_StoreDataBlock(&balancing_tab, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
                } else {
                    DATA_GetTable(&balancing_tab, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
                    balancing_tab.enable_balancing=0;
                    balancing_tab.threshold = data[2]; //get threshold in mV
                    DATA_StoreDataBlock(&balancing_tab, DATA_BLOCK_ID_BALANCING_CONTROL_VALUES);
                }
                break;
            default:
                break;
        }

    }
    return 0;
}
uint32_t cans_getdebug(uint32_t sigIdx, void *value) {
    static uint32_t access_dummy;
    if(value != NULL_PTR) {
        *(uint32_t *)value = access_dummy;
    }
    access_dummy++;
    return 0;
}

