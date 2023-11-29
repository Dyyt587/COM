/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    bdma2.c
  * @brief   This file provides code for the configuration
  *          of all the requested memory to memory DMA transfers.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "bdma2.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure DMA                                                              */
/*----------------------------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Enable DMA controller clock
  */
void MX_BDMA2_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_BDMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* BDMA2_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BDMA2_Channel2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(BDMA2_Channel2_IRQn);
  /* BDMA2_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BDMA2_Channel3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(BDMA2_Channel3_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

