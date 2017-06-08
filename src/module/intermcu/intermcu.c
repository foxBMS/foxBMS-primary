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
 * @file    intermcu.h
 * @author  foxBMS Team
 * @date    04.03.2016 (date of creation)
 * @ingroup DRIVERS
 * @prefix  IMC
 *
 * @brief   Driver for the inter-MCU communication.
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
#include "intermcu.h"

#include "spi.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions======================*/
uint8_t readArray[] = {0};

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

/**
 * @brief   Sends one byte via the internal SPI bus connecting both the main MCU and the Safety MCU
 *
 * @param   message: the byte to be sent
 *
 * @return  void
 */
void IMC_sendByte(uint8_t message) {
    uint8_t transmitArray[] = {message};

    SPI_SetCS(2);
    HAL_SPI_Transmit(SPI_HANDLE_IMC, transmitArray, 1, 1000);
    SPI_UnsetCS(2);
}

/**
 * @brief   Enables a listener interrupt on the internal SPI bus connecting both the main MCU and the Safety MCU
 *
 * @return  void
 */
void IMC_enableInterrupt(void) {
 HAL_SPI_Receive_IT(SPI_HANDLE_IMC, readArray, 1);
}

/**
 * @brief   Reads one byte on the internal SPI bus connecting both the main MCU and the Safety MCU that has been received by interrupt
 *
 * @return  the received byte
 */
uint8_t IMC_readByte(void) {
    return readArray[0];
}
