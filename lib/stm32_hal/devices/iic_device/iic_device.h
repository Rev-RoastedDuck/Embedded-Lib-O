/*
 * iic_device.h
 *	
 *  Created on: 2024_11_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef DEVICES_IIC_DEVICE_H_
#define DEVICES_IIC_DEVICE_H_

/******************************************************************************/
/*----------------------------------INCLUDE-----------------------------------*/
/******************************************************************************/
#include "stdlib.h"
#include "stdbool.h"
#include "soft_i2c.h"
#include "stm32g4xx_hal.h"
#include "soft_i2c_interface.h"
#include "iic_device_interface.h"


/******************************************************************************/
/*----------------------------------STRUCT------------------------------------*/
/******************************************************************************/
typedef struct __IIC_DEVICE_RRD{
	IIC_DEVICE_INTERFACE_RRD *interface;

	RRD_DRIVER_IIC 		*__soft_iic_driver;
	I2C_HandleTypeDef 	*__hard_iic_driver;

	uint16_t 	timeout;
	uint8_t 	slave_address;
	
	void 		(*del)					(void **self);
}IIC_DEVICE_RRD;

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
IIC_DEVICE_RRD *iic_device_new(void *iic_driver,bool use_soft_iic,uint8_t slave_address,uint16_t timeout);
#endif







