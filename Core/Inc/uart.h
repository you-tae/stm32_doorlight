/*
 * uart.h
 *
 *  Created on: Feb 4, 2025
 *      Author: npedu-203-11
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "main.h"

void initUart(UART_HandleTypeDef *inUart);
char getUart();
char *getMessage();

#endif /* INC_UART_H_ */
