/*
 * volume.c
 *
 *  Created on: Mar 16, 2023
 *      Author: Teo
 */


#include "stm32f0xx_hal.h"
#include "volume.h"

/* defines for the CS PIN */
#define VOLUME_CS_Pin 			GPIO_PIN_6
#define VOLUME_CS_GPIO_Port 	GPIOA
#define SPI_TIMEOUT 1000

extern SPI_HandleTypeDef hspi1;

/* SPI Chip Select */
static void SELECT(void)
{
  HAL_GPIO_WritePin(VOLUME_CS_GPIO_Port, VOLUME_CS_Pin, GPIO_PIN_RESET);
}

/* SPI Chip Deselect */
static void DESELECT(void)
{
  HAL_GPIO_WritePin(VOLUME_CS_GPIO_Port,VOLUME_CS_Pin, GPIO_PIN_SET);
}

void VOLUME_set(uint8_t channel, uint8_t volume)
{
	// first byte set channel if more than one LM1972 is connected
	// second byte set volume (0 - max, 127 - mute)
	SELECT();
	HAL_SPI_Transmit(&hspi1, &channel, 1, SPI_TIMEOUT);
	HAL_SPI_Transmit(&hspi1, &volume, 1, SPI_TIMEOUT);
	DESELECT();
}
