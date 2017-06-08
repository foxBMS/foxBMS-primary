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
 * @file    misc.c
 * @author  foxBMS Team
 * @date    23.12.2015 (date of creation)
 * @ingroup UTIL
 * @prefix  none
 *
 * @brief   Miscellaneous functions
 *
 *
 */

/*================== Includes =============================================*/
#include "general.h"
#include "misc.h"

/*================== Macros and Definitions ===============================*/

/*================== Constant and Variable Definitions ====================*/

/*================== Function Prototypes ==================================*/

/*================== Function Implementations =============================*/

/**
 * @brief U16ToDecascii is a helper function which converts U16 to ASCII.
 *
 * This function does some basic conversion stuff to be able to print out numbers/values as their corresponding ASCII.
 *
 * @return (type: uint8_t)
 */

uint8_t *U16ToDecascii(uint8_t *destptr, uint16_t *dataptr, uint8_t minDigits)
{
    uint16_t tmp = *dataptr;
    destptr += 4;

    *destptr = (tmp % 10) + 48;
    tmp = tmp/10;
    destptr--;

    *destptr = (tmp % 10) + 48;
    tmp = tmp/10;
    destptr--;

    *destptr = (tmp % 10) + 48;
    tmp = tmp/10;
    destptr--;

    *destptr = (tmp % 10) + 48;
    tmp = tmp/10;
    destptr--;

    *destptr = (tmp % 10) + 48;

    destptr += 5 - minDigits;

    return destptr;
}



/**
 * @brief U8ToDecascii is a helper function which converts U8 to ASCII.
 *
 * This function does some basic conversion stuff to be able to print out numbers/values as their corresponding ASCII.
 *
 * @return (type: uint8_t)
 */

uint8_t *U8ToDecascii(uint8_t *destptr, uint8_t *dataptr, uint8_t minDigits)
{
    uint8_t tmp = *dataptr;
    destptr += 2;

    *destptr = (tmp % 10) + 48;
    tmp = tmp/10;
    destptr--;

    *destptr = (tmp % 10) + 48;
    tmp = tmp/10;
    destptr--;

    *destptr = (tmp % 10) + 48;


    if (*dataptr <= 9)
    {
        destptr += 3 - minDigits;
    }
    else if (*dataptr >= 10 && *dataptr <= 99)
    {
        if (minDigits != 3)
        {
            destptr += 1;
        }
    }
    else
    {
        ;
    }

    return destptr;
}

/**
 * @brief hex2ascii is a helper function which converts hex input to ASCII.
 *
 * This function does some basic conversion stuff to be able to print out numbers/values as their corresponding ASCII.
 *
 * @return (type: uint8_t)
 */


uint8_t *hex2ascii(uint8_t *destptr, uint8_t *srcptr, uint8_t len)
{
    uint8_t lownibble,highnibble;
    uint8_t *destptr_old = destptr;
    srcptr = srcptr+len-1;      //set pointer to last address
    while(len--)
    {
        highnibble  =   (*srcptr>>4);       //high nibble
        lownibble   =   (*srcptr & 0xF);    //low nibble

        if(highnibble < 10)
            *destptr++ =  highnibble + '0';
        else
            *destptr++ =  (highnibble-10) + 'A';

        if(lownibble < 10)
            *destptr++ =  lownibble + 0x30; //0x30 == '0'
        else
            *destptr++ =  (lownibble-10) + 'A';
        srcptr--;
    }
    return destptr_old;
}

/**
 * @brief U32ToHexascii is a helper function which converts U32 to hex.
 *
 * This function is just a wrapper for hex2ascii.
 *
 * @return (type: uint8_t)
 */

uint8_t *U32ToHexascii(uint8_t *destptr, uint32_t *dataptr)
{
    return(hex2ascii(destptr,(uint8_t*)(dataptr), 4));
}

/**
 * @brief U16ToHexascii is a helper function which converts U16 to hex.
 *
 * This function is just a wrapper for hex2ascii.
 *
 * @return (type: uint8_t)
 */

uint8_t *U16ToHexascii(uint8_t *destptr, uint16_t *dataptr)
{
    return(hex2ascii(destptr,(uint8_t*)(dataptr), 2));
}


/**
 * @brief U8ToHexascii is a helper function which converts U16 to hex.
 *
 * This function is just a wrapper for hex2ascii.
 *
 * @return (type: uint8_t)
 */

uint8_t *U8ToHexascii(uint8_t *destptr, uint8_t *dataptr)
{
    return(hex2ascii(destptr,(uint8_t*)(dataptr), 1));
}


/**
 * @brief I32ToDecascii is a helper function which converts I32 to ASCII.
 *
 * This function does some basic conversion stuff to be able to print out numbers/values as their corresponding ASCII.
 *
 * Required buffer: at least uint8_t[12]
 *
 * @return (type: uint8_t)
 */
uint8_t *I32ToDecascii(uint8_t *destptr, int32_t *dataptr) {

    int32_t tmp = *dataptr;
    uint32_t tmp1;
    uint8_t negativ = 0;

    destptr += 10;

    /* determine if sign is necessary */
    if(*dataptr < 0) {
        negativ = 1;
        tmp1 = (uint32_t)-tmp;
    } else {
        tmp1 = tmp;
    }

    /* transform to ascii */
    for(uint8_t i = 0; i < 9; i++) {

        *destptr = (tmp1 % 10) + 48;
        tmp1 = tmp1/10;
        destptr--;
    }

    *destptr = (tmp1 % 10) + 48;

    /* Set destptr according to number length */
    if (*dataptr <= 9 && *dataptr >= -9)
    {
        destptr += 9 - negativ;
    }
    else if ((*dataptr >= 10 && *dataptr <= 99) || (*dataptr <= -10 && *dataptr >= -99))
    {
        destptr += 8 - negativ;
    }
    else if((*dataptr >= 100 && *dataptr <= 999) || (*dataptr <= -100 && *dataptr >= -999))
    {
        destptr += 7 - negativ;
    }
    else if((*dataptr >= 1000 && *dataptr <= 9999) || (*dataptr <= -1000 && *dataptr >= -9999))
    {
        destptr += 6 - negativ;
    }
    else if((*dataptr >= 10000 && *dataptr <= 99999) || (*dataptr <= -10000 && *dataptr >= -99999))
    {
        destptr += 5 - negativ;
    }
    else if((*dataptr >= 100000 && *dataptr <= 999999) || (*dataptr <= -100000 && *dataptr >= -999999))
    {
        destptr += 4 - negativ;
    }
    else if((*dataptr >= 1000000 && *dataptr <= 9999999) || (*dataptr <= -1000000 && *dataptr >= -9999999))
    {
        destptr += 3 - negativ;
    }
    else if((*dataptr >= 10000000 && *dataptr <= 99999999) || (*dataptr <= -10000000 && *dataptr >= -99999999))
    {
        destptr += 2 - negativ;
    }
    else if((*dataptr >= 100000000 && *dataptr <= 999999999) || (*dataptr <= -100000000 && *dataptr >= -999999999))
    {
        destptr += 1 - negativ;
    }
    else
    {
        destptr -= negativ;
    }

    if(negativ == 1)
        *destptr = 45;

    return destptr;
}
