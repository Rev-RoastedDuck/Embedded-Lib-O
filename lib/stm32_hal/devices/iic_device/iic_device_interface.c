#include "iic_device_interface.h"

inline uint8_t iic_device_check_device(void *self)
{
    return (*(IIC_DEVICE_INTERFACE_RRD**)self)->check_device(self);
}

inline uint8_t iic_device_read_buffer(void *self, const uint8_t reg_add,
                                uint8_t *data_list, uint8_t num)
{
    return (*(IIC_DEVICE_INTERFACE_RRD**)self)->read_buffer(self,reg_add,data_list,num);
}

inline uint8_t iic_device_write_buffer(void *self, const uint8_t reg_add, 
                                uint8_t *data_list, uint8_t num)
{
    return (*(IIC_DEVICE_INTERFACE_RRD**)self)->write_buffer(self,reg_add,data_list,num);
}


