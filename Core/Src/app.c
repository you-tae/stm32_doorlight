/*
 * app.c
 *
 *  Created on: Feb 4, 2025
 *      Author: npedu-203-11
 */

#include "app.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim11;
int timeoutDoor;

void delay_us(uint16_t time)
{
	htim11.Instance->CNT = 0;
	while(htim11.Instance->CNT < time);
}

void SystickCallback()
{
	if(timeoutDoor > 0) timeoutDoor--;
}

void app()
{
	//uart init
	initUart(&huart2);
	// 초음파 초기화
	initUltrasonic(&htim3);
	//timer init
	HAL_TIM_Base_Start(&htim11);     // only counting
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);   // 초기에는 LED 끄기

	while(1)
	{
		//초음파 값 읽기
		uint16_t distance = getDistance();

		// 거리에 따라 LED켜기
		if(distance <= 5)
		{
			timeoutDoor = 5000;
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
		}

		if(timeoutDoor == 0)  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
	}
}


