/*
 * iic_device_interface.h
 *	
 *  Created on: 2024_11_12
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */

#ifndef DEVICES_IIC_DEVICE_INTERFACE_H_
#define DEVICES_IIC_DEVICE_INTERFACE_H_

/******************************************************************************/
/*----------------------------------INCLUDE-----------------------------------*/
/******************************************************************************/
#include "stdint.h"

typedef uint8_t (*iic_device_check_device_fn_t)(void *self);
typedef uint8_t (*iic_device_read_buffer_fn_t)(void *self, const uint8_t reg_add, uint8_t *data_list, uint8_t num);
typedef uint8_t (*iic_device_write_buffer_fn_t)(void *self, const uint8_t reg_add, uint8_t *data_list, uint8_t num);

typedef struct __IIC_DEVICE_INTERFACE_RRD
{
    iic_device_read_buffer_fn_t read_buffer;
    iic_device_check_device_fn_t check_device;
    iic_device_write_buffer_fn_t write_buffer;
}IIC_DEVICE_INTERFACE_RRD;

uint8_t iic_device_check_device(void *self);
uint8_t iic_device_read_buffer(void *self, const uint8_t reg_add,uint8_t *data_list, uint8_t num);
uint8_t iic_device_write_buffer(void *self, const uint8_t reg_add,uint8_t *data_list, uint8_t num);

#endif







