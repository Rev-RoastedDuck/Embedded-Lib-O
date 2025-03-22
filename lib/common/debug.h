/*
 * log.h
 *	
 *  Created on: 2024_11_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */
 
/******************************************************************************/
/*----------------------------------INCLUDE-----------------------------------*/
/******************************************************************************/
#include "stdio.h"

/******************************************************************************/
/*------------------------------------MARCO-----------------------------------*/
/******************************************************************************/
#define DEBUG_PRINT(pass,fmt, ...) \
	do { \
			if(!pass){ \
				printf("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
			}\
	} while (0)

#define debug_uart_printf_1(fmt, ...) 				
#define debug_uart_printf_2(pass, fmt, ...) 	DEBUG_PRINT(pass, fmt, ##__VA_ARGS__)

#define debug_ebable	(1)
#define debug_disable	(0)