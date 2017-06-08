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
 * @file    io_eval_board_cfg.h
 * @author  foxBMS Team
 * @date    26.08.2015 (date of creation)
 * @ingroup DRIVERS_CONF
 * @prefix  none
 *
 * @brief   Configuration for the I/O ports for the evaluation board
 *
 */

#ifndef IO_CFG_H_EVAL_BOARD_H_
#define IO_CFG_H_EVAL_BOARD_H_


/*================== Includes =============================================*/

/*================== Macros and Definitions ===============================*/

#define PIN_MCU_0_BMS_INTERFACE_SPI_NSS                     IO_PE_3 // free io-pin on the discovery board
#define PIN_MCU_0_BMS_INTERFACE_SPI_MOSI                    IO_PE_6 // free io-pin on the discovery board
#define PIN_MCU_0_BMS_INTERFACE_SPI_MISO                    IO_PE_5 // free io-pin on the discovery board
#define PIN_MCU_0_BMS_INTERFACE_SPI_SCK                     IO_PE_2 // free io-pin on the discovery board

#define PIN_CAN1_TX                                         IO_PB_9 // free io-pin on the discovery board
#define PIN_CAN1_RX                                         IO_PB_8 // free io-pin on the discovery board
#define PIN_CAN0_TX                                         IO_PB_6 // free io-pin on the discovery board
#define PIN_CAN0_RX                                         IO_PB_5 // free io-pin on the discovery board

#define TEST_LED                                            IO_PC_8      // package pin:117
#define TEST_INPUT                                          IO_PG_9      // package pin:152
#define TEST_LED3                                           IO_PG_13     // package pin:156
#define TEST_LED4                                           IO_PG_14     // package pin:157
#define TEST_OUTPUT                                         IO_PB_4      // package pin:162

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

#endif /* IO_CFG_H_EVAL_BOARD_H_ */
