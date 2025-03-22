#include "soft_i2c_interface.h"

void soft_i2c_config_gpios(void *self, GPIO_TypeDef *scl_port, uint16_t scl_pin, 
                            GPIO_TypeDef *sda_port, uint16_t sda_pin, uint16_t timeout)
{
    (*(SOFT_IIC_DRIVER_INTERFACE_RRD **)self)->config_gpios(self,scl_port,scl_pin,
                                                                        sda_port,sda_pin,timeout);
}

uint8_t soft_i2c_read_byte(void *self, uint8_t ack)
{
    return (*(SOFT_IIC_DRIVER_INTERFACE_RRD **)self)->read_byte(self,ack);
}

void soft_i2c_send_byte(void *self, uint8_t byte)
{
    (*(SOFT_IIC_DRIVER_INTERFACE_RRD **)self)->send_byte(self,byte);
}

uint8_t soft_i2c_read_buffer(void *self, const uint8_t slave_add, const uint8_t reg_add, uint8_t *data_list, uint8_t num)
{
    return (*(SOFT_IIC_DRIVER_INTERFACE_RRD **)self)->read_buffer(self,slave_add,reg_add,data_list,num);
}

uint8_t soft_i2c_write_buffer(void *self, const uint8_t slave_add, const uint8_t reg_add, uint8_t *data_list, uint8_t num)
{
    return (*(SOFT_IIC_DRIVER_INTERFACE_RRD **)self)->write_buffer(self,slave_add,reg_add,data_list,num);
}

uint8_t soft_i2c_check_device(void *self, uint8_t address)
{
    return (*(SOFT_IIC_DRIVER_INTERFACE_RRD **)self)->check_device(self,address);
}
