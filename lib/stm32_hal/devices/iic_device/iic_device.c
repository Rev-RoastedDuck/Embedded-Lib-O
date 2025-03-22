/*
 * iic_device.c
 *	
 *  Created on: 2024_10_24
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */
#include "iic_device.h"

/******************************************************************************/
/*----------------------------------FUNCTION----------------------------------*/
/******************************************************************************/
uint8_t check_device(IIC_DEVICE_RRD *self)  
{
	return NULL != self->__soft_iic_driver? soft_i2c_check_device(self->__soft_iic_driver,self->slave_address)
					: HAL_I2C_IsDeviceReady(self->__hard_iic_driver,self->slave_address,1,self->timeout);
}

uint8_t read_buffer(IIC_DEVICE_RRD *self,const uint8_t reg_add,uint8_t* data_list,uint8_t num)
{
	NULL != self->__soft_iic_driver? soft_i2c_read_buffer(self->__soft_iic_driver,self->slave_address,reg_add,data_list,num)
					: HAL_I2C_Master_Receive(self->__hard_iic_driver,self->slave_address,data_list,num,self->timeout);
	return 0;
}

uint8_t write_buffer(IIC_DEVICE_RRD *self,const uint8_t reg_add,uint8_t* data_list,uint8_t num)
{
	NULL != self->__soft_iic_driver? soft_i2c_write_buffer(self->__soft_iic_driver,self->slave_address,reg_add,data_list,num)
					: HAL_I2C_Master_Transmit(self->__hard_iic_driver,self->slave_address,data_list,num,self->timeout);
	return 0;
}

static IIC_DEVICE_INTERFACE_RRD IIC_DEVICE_INTERFACE = {
	.check_device = (iic_device_check_device_fn_t)check_device,
	.read_buffer = (iic_device_read_buffer_fn_t)read_buffer,
	.write_buffer = (iic_device_write_buffer_fn_t)write_buffer
};

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
void iic_device_del(void **self){
    if (self != NULL && *self != NULL) {
        free(*self);
        *self = NULL;
    }
}

IIC_DEVICE_RRD *iic_device_new(void *iic_driver,
								bool use_soft_iic,
								uint8_t slave_address,
								uint16_t timeout)
{
	if(NULL == iic_driver){
		// log
		return NULL;
	}

	IIC_DEVICE_RRD* self;
	self = malloc(sizeof(*self));
	if(NULL == self){
		// debug_log("RRD_USART_MANAGER malloc failed");
		return NULL;
	}

	if(use_soft_iic){
		self->__hard_iic_driver = NULL;
		self->__soft_iic_driver = (RRD_DRIVER_IIC*)iic_driver;
		DEBUG_PRINT(0,"use_soft_iic\r\n");
	}else{
		self->__soft_iic_driver = NULL;
		self->__hard_iic_driver = (I2C_HandleTypeDef*)iic_driver;
		DEBUG_PRINT(0,"use_hard_iic\r\n");
	}
	
	self->slave_address = slave_address;
	self->timeout = timeout;

	self->del = iic_device_del;
	self->interface = &IIC_DEVICE_INTERFACE;

	return self;
}

