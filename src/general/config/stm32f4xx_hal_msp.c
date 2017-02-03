/**
 * @file stm32f4xx_hal_msp.c
 * @author  STMicroelectronics
 * @date    2015
 * @ingroup GENERAL_CONF
 * @brief HAL MSP Module, definition of initialization/de-initialization functions called when hardware units are activated/deactivated
 *
 */

/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/*================== Includes =============================================*/
#include "stm32f4xx_hal.h"
#include "dma.h"

/*================== Macros and Definitions ===============================*/

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/



static uint8_t CAN_CLK1_enabled = 0;


/**
  * @{
  */

/**
  * Initializes the Global MSP and is called from HAL_Init() 
  */
void HAL_MspInit(void)
{

}

/**
  * @brief  DeInitializes the Global MSP and is called from HAL_DeInit()
  */
void HAL_MspDeInit(void)
{

}






/**
 *  Inizalizes the CAN modules, called by HAL_CAN_Init()
 *
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    if(hcan->Instance  ==  CAN1)
    {
        CAN_CLK1_enabled++;
        if(CAN_CLK1_enabled  ==  1)
        {
            // Enable CAN1 clock
            __HAL_RCC_CAN1_CLK_ENABLE();
        }
    }
    if(hcan->Instance  ==   CAN2)
    {
        CAN_CLK1_enabled++;
        if(CAN_CLK1_enabled  ==  1)
        {
            // Enable CAN1 clock. CAN1 clock needs to run if CAN2 clock is enabled
            __HAL_RCC_CAN1_CLK_ENABLE();
        }
        // Enable CAN2 clock
        __HAL_RCC_CAN2_CLK_ENABLE();
    }

}

/**
 * Deinitalizes the CAN modules
 *
 */
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
    if(hcan->Instance  ==  CAN1)
    {
        if(CAN_CLK1_enabled > 0)
        {
            CAN_CLK1_enabled--;
        }

        if(CAN_CLK1_enabled  ==  0)
        {
            // Enable CAN1 clock
            __HAL_RCC_CAN1_CLK_DISABLE();
        }
    }

    if(hcan->Instance  ==   CAN2)
    {
        // Disable CAN2 clock
        __HAL_RCC_CAN2_CLK_DISABLE();

        if(CAN_CLK1_enabled > 0)
        {
            CAN_CLK1_enabled--;
        }
        if(CAN_CLK1_enabled  ==  0)
        {
            // Enable CAN1 clock. CAN1 clock needs to run if CAN2 clock is enabled
            __HAL_RCC_CAN1_CLK_DISABLE();
        }
    }
}







/**
 * inits SPI 1-3
 * configures SPI-Pins and SPI-Clocks
 * called by HAL_SPI_Init()
 *
 * @param hspi: pointer to SPI hardware handle
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

  // GPIO_InitTypeDef GPIO_InitStruct;

  /* USER CODE BEGIN SPI1_MspInit 0 */


  /* USER CODE END SPI1_MspInit 0 */



  // GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance == SPI6)
  {
  // SPI4 peripheral clock enable
  __HAL_RCC_SPI6_CLK_ENABLE();

  }

}

/**
 * de-inits SPI 1-3
 * called by HAL-Functions
 *
 * @param hspi: pointer to SPI hardware handle
 *
 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

  if(hspi->Instance == SPI4)
  {
  /* USER CODE BEGIN SPI4_MspDeInit 0 */

  /* USER CODE END SPI4_MspDeInit 0 */
    /* Peripheral clock disable */
    // __SPI1_CLK_DISABLE();
    __SPI4_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }

  if(hspi->Instance == SPI6)
  {

    __SPI6_CLK_DISABLE();

  }


}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __USART3_CLK_ENABLE();

//    /**USART2 GPIO Configuration
//    PA2     ------> USART2_TX
//    PA3     ------> USART2_RX
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
//    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
  if(huart->Instance==USART2)
  {
      /* Peripheral clock enable */
      __USART2_CLK_ENABLE();  }

}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
