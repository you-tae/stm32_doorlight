/*
 * ultrasonic.c
 *
 *  Created on: Feb 6, 2025
 *      Author: npedu-203-11
 */

#include "ultrasonic.h"

TIM_HandleTypeDef *myHtim;

uint32_t valueIC1 = 0;
uint32_t valueIC2 = 0;
uint32_t difference = 0;
bool		 IsFirstCaptured = 0;
bool 		 doneCapture = false;
uint16_t distance = 0;

void initUltrasonic(TIM_HandleTypeDef *htim)
{
	myHtim = htim;
	HAL_TIM_IC_Start_IT(myHtim, TIM_CHANNEL_1);
}

// Input Capture Interrupt callback
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if(IsFirstCaptured == 0)
		{
			// echo의 상승 신호 발생 시각을 valueIC1 기록
			valueIC1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			IsFirstCaptured = 1;
			//echo 하강 신호에 대응되는 인터럽트 설정
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		else if(IsFirstCaptured == 1)
		{
			//echo의 하강 신호 발생 시각을 valueIC2에 기록
			valueIC2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			//valueIC1과 valueIC2의 시간차 계산
			if(valueIC1 < valueIC2)
			{
				difference = valueIC2 - valueIC1;
			}
			else if(valueIC1 > valueIC2)
			{
				difference = (65535 - valueIC1) + valueIC2;
			}
			// 거리 계산, 소리의 속도 0.034
			distance = difference * 0.034 / 2;
			IsFirstCaptured = 0;
			doneCapture = true;
			// 다음 측정을 위하여 초기화: echo의 상승 신호에 대응되는 인터럽트 설정
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(htim, TIM_IT_CC1);     // 인터럽트 끄기
		}
	}
}

uint16_t getDistance()
{
	//TRIG 핀에 10us의 펄스 출력
	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, 1);
	delay_us(10);
	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, 0);
	//인터럽트 활설화
	__HAL_TIM_ENABLE_IT(myHtim, TIM_IT_CC1);
	doneCapture = false;
	while(doneCapture == false);
	return distance;
}
