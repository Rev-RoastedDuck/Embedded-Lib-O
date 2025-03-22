/*
 * oled.h
 *	
 *  Created on: 2024_11_01
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef DEVICES_OLED_H_
#define DEVICES_OLED_H_

/******************************************************************************/
/*----------------------------------INCLUDE-----------------------------------*/
/******************************************************************************/
#include "i2c.h"
#include "string.h"
#include "stdio.h"
#include "iic_device.h"
#include "stm32g4xx_hal.h"

/******************************************************************************/
/*-----------------------------------CONFIG-----------------------------------*/
/******************************************************************************/
#define USE_SOFT_IIC	1
#if !USE_SOFT_IIC
#define	OLED_HARD_IIC_HANDLE			(&hi2c2)
#endif

/******************************************************************************/
/*----------------------------------STRUCT------------------------------------*/
/******************************************************************************/
typedef struct _RRD_DEVICE_OLED {
	IIC_DEVICE_RRD* __iic_device;

	void (*del)				(void **self);
	void (*init)			(struct _RRD_DEVICE_OLED *self);
	void (*clear)			(struct _RRD_DEVICE_OLED *self);
	void (*show_char)		(struct _RRD_DEVICE_OLED *self, uint8_t line, uint8_t column, char ch);
	void (*show_string)		(struct _RRD_DEVICE_OLED *self, uint8_t line, uint8_t column, char *string);
	void (*show_num)		(struct _RRD_DEVICE_OLED *self, uint8_t line, uint8_t column, uint32_t number, uint8_t length);
	void (*show_signed_num)	(struct _RRD_DEVICE_OLED *self, uint8_t line, uint8_t column, int32_t number, uint8_t length);
	void (*show_hex_num)	(struct _RRD_DEVICE_OLED *self, uint8_t line, uint8_t column, uint32_t number, uint8_t length);
	void (*show_float)		(struct _RRD_DEVICE_OLED *self, uint8_t line, uint8_t column, float number, uint8_t precision);
	void (*show_bin_num)	(struct _RRD_DEVICE_OLED *self, uint8_t line, uint8_t column, uint32_t number, uint8_t length);
} RRD_DEVICE_OLED;


/******************************************************************************/
/*---------------------------------INTERFACE----------------------------------*/
/******************************************************************************/
RRD_DEVICE_OLED *oled_device_new(void *iic_driver,bool use_soft_iic,uint8_t slave_address,uint16_t timeout);

/******************************************************************************/
/*-------------------------------------DEBUG----------------------------------*/
/******************************************************************************/
#define OPEN_OLED_TEST		0
#if OPEN_OLED_TEST
	void oled_test_start(void);
#endif

#endif /* \} ifndef DEVICES_OLED_H_ */
