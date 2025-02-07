/*
 * uart.c
 *
 *  Created on: Feb 4, 2025
 *      Author: npedu-203-11
 */
#include "uart.h"

//for circular buffer
#define rxBufferMax 100	          // 버퍼 크기
uint8_t rxBuffer[rxBufferMax];    // 버퍼
uint16_t rxBufferWriteIndex;      // 버퍼 쓰기 인덱스
uint16_t rxBufferReadIndex;       // 버퍼 읽기 인덱스
char		 rxchar;                  // 수신문자 임시보관

UART_HandleTypeDef* myHuart;

int _write(int file, char *p, int len)   //printf
{
	HAL_UART_Transmit(myHuart, (uint8_t*)p, len, 10);
	return len;
}

// 수신 인터럽트 콜백
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	rxBuffer[rxBufferWriteIndex++] = rxchar;     // 수신 데이터 버퍼에 저장
	rxBufferWriteIndex %= rxBufferMax;           // 인덱스가 최댓값 도달 시 0으로 리셋
	HAL_UART_Receive_IT(myHuart, (uint8_t*)&rxchar, 1);    // 수신 인터럽트 준비 --> 하나 받은 후 다시 준비를 시켜주어야 함.
}

void initUart(UART_HandleTypeDef *inHuart)
{
	myHuart = inHuart;
	// 수신 인터럽트 준비
	HAL_UART_Receive_IT(myHuart, (uint8_t*)&rxchar, 1);
}

char getUart()
{
	char result;
	//HAL_UART_Receive(myHuart, (uint8_t*)&result, 1, 1);   // polling 방식
	// 인터럽트 방식
	if(rxBufferWriteIndex == rxBufferReadIndex) return 0;
	result = rxBuffer[rxBufferReadIndex++];               // 버퍼에서 값을 하나씩 꺼내옴
	rxBufferReadIndex %= rxBufferMax;
	return result;
}

char *getMessage()
{
	static char message[100];
	static uint8_t messageCount = 0;
	char ch = getUart();
	if(ch != 0)
	{
		if(ch == '\n' || ch == '\r')
		{
			memset(message, 0, 100);
			messageCount = 0;
		}
		else if(ch >= 0x20)
		{
			message[messageCount++] = ch;
		}
	}
	return message;
}
