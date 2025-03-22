/*
 * soft_i2c.h
 *	
 *  Created on: 2024Äê7ÔÂ30ÈÕ
 *      Author: Rev_RoastDuck
 */

#ifndef DRIVERS_SOFT_I2C_INTERFACE_H_
#define DRIVERS_SOFT_I2C_INTERFACE_H_

/******************************************************************************/
/*----------------------------------INCLUDE-----------------------------------*/
/******************************************************************************/
#include "stddef.h"
#include "stm32g4xx_hal.h"

/******************************************************************************/
/*----------------------------------STRUCT------------------------------------*/
/******************************************************************************/

typedef struct __SOFT_IIC_DRIVER_PARAMS_RRD {
		GPIO_TypeDef* 	__scl_port;
		uint16_t 		__scl_pin;
		
		GPIO_TypeDef* 	__sda_port;
		uint16_t 		__sda_pin;

		uint16_t        __timeout;
}SOFT_IIC_DRIVER_PARAMS_RRD;

typedef uint8_t 	(*soft_iic_read_byte_fn_t) 			(void *self, uint8_t ack);   		
typedef void 		(*soft_iic_send_byte_fn_t) 			(void *self, uint8_t byte);
typedef uint8_t 	(*soft_iic_check_device_fn_t)		(void *self, uint8_t address);
typedef uint8_t 	(*soft_iic_read_buffer_fn_t)		(void *self, const uint8_t slave_add,const uint8_t reg_add,uint8_t* data_list,uint8_t num);
typedef uint8_t 	(*soft_iic_write_buffer_fn_t)		(void *self, const uint8_t slave_add,const uint8_t reg_add,uint8_t* data_list,uint8_t num);
typedef void		(*soft_iic_config_gpios_fn_t)		(void *self,GPIO_TypeDef* scl_port,uint16_t scl_pin,GPIO_TypeDef* sda_port,uint16_t sda_pin,uint16_t timeout);

typedef struct __SOFT_IIC_DRIVER_INTERFACE_RRD {
	soft_iic_read_byte_fn_t 	read_byte;
	soft_iic_send_byte_fn_t 	send_byte;
	soft_iic_check_device_fn_t 	check_device;
	soft_iic_read_buffer_fn_t 	read_buffer;
	soft_iic_write_buffer_fn_t 	write_buffer;
	soft_iic_config_gpios_fn_t 	config_gpios;
}SOFT_IIC_DRIVER_INTERFACE_RRD;


/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
void soft_i2c_send_byte(void *self, uint8_t byte);
uint8_t soft_i2c_read_byte(void *self, uint8_t ack);
uint8_t soft_i2c_check_device(void *self, uint8_t address);
uint8_t soft_i2c_read_buffer(void *self, const uint8_t slave_add, const uint8_t reg_add, uint8_t *data_list, uint8_t num);
uint8_t soft_i2c_write_buffer(void *self, const uint8_t slave_add, const uint8_t reg_add, uint8_t *data_list, uint8_t num);
void soft_i2c_config_gpios(void *self, GPIO_TypeDef *scl_port, uint16_t scl_pin, GPIO_TypeDef *sda_port, uint16_t sda_pin, uint16_t timeout);

#endif

