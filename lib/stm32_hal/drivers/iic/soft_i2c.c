#include "soft_i2c.h"

static GPIO_InitTypeDef g_GPIO_CONFIG = {
	.Pull = GPIO_NOPULL,
	.Speed = GPIO_SPEED_FREQ_HIGH,
};

/******************************************************************************/
/*-----------------------------------配置层-----------------------------------*/
/******************************************************************************/

static inline void iic_scl_1(RRD_DRIVER_IIC *driver) {
	HAL_GPIO_WritePin(driver->params.__scl_port,driver->params.__scl_pin,GPIO_PIN_SET);
}

static inline void iic_scl_0(RRD_DRIVER_IIC *driver) {
		HAL_GPIO_WritePin(driver->params.__scl_port,driver->params.__scl_pin,GPIO_PIN_RESET);
}

static inline void iic_sda_1(RRD_DRIVER_IIC *driver) {
		HAL_GPIO_WritePin(driver->params.__sda_port,driver->params.__sda_pin,GPIO_PIN_SET);
}

static inline void iic_sda_0(RRD_DRIVER_IIC *driver) {
		HAL_GPIO_WritePin(driver->params.__sda_port,driver->params.__sda_pin,GPIO_PIN_RESET);
}

static inline uint8_t iic_sda_read(RRD_DRIVER_IIC *driver) {
	return HAL_GPIO_ReadPin(driver->params.__sda_port,driver->params.__sda_pin);
}

static void iic_to_write_mode(RRD_DRIVER_IIC *self)
{
	iic_scl_0(self);	// 防止在切换读写模式的时候，出现不确定电平
	g_GPIO_CONFIG.Pin = self->params.__sda_pin;
	g_GPIO_CONFIG.Mode = GPIO_MODE_OUTPUT_OD;
	HAL_GPIO_Init(self->params.__sda_port, &g_GPIO_CONFIG);
}

static void iic_to_read_mode(RRD_DRIVER_IIC *self)
{
	iic_scl_0(self);	// 防止在切换读写模式的时候，出现不确定电平
	g_GPIO_CONFIG.Pin = self->params.__sda_pin;
	g_GPIO_CONFIG.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(self->params.__sda_port, &g_GPIO_CONFIG);
}

/******************************************************************************/
/*----------------------------------内部宏定义--------------------------------*/
/******************************************************************************/
#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */


/******************************************************************************/
/*-----------------------------------内部函数---------------------------------*/
/******************************************************************************/
/**
 * @brief  			I2C总线位延迟，最快400KHz
 * @note			 下面的时间是通过安富莱AX-Pro逻辑分析仪测试得到的。
							  CPU主频72MHz时，在内部Flash运行, MDK工程不优化
							 循环次数为10时，SCL频率 = 205KHz 
							 循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
							 循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
							 IAR工程编译效率高，不能设置为7
 * @param[in]   
 * @return      
 */
static inline void iic_delay(void)
{
	uint8_t i;
	for (i = 0; i < 20; i++);
}

/******************************************************************************/
/*-----------------------------------接口实现---------------------------------*/
/******************************************************************************/
/**
 * @brief  			CPU发起I2C总线启动信号
 * @note				当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
 * @param[in]   
 * @return
 */
static void i2c_start(RRD_DRIVER_IIC *driver)
{
	iic_to_write_mode(driver);
	
	iic_sda_1(driver);
	iic_scl_1(driver);
	iic_delay();
	
	iic_sda_0(driver);
	iic_delay();
	
	iic_scl_0(driver);
	iic_delay();
}

/**
 * @brief  			CPU发起I2C总线停止信号
 * @note				当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
 * @param[in]   
 * @return
 */
static void i2c_stop(RRD_DRIVER_IIC *driver)
{
	iic_to_write_mode(driver);
	
	iic_sda_0(driver);
	iic_scl_1(driver);
	iic_delay();
	
	iic_scl_1(driver);
	iic_delay();
	
	iic_sda_1(driver);
	iic_delay();
}

/**
 * @brief  			CPU产生一个ACK信号
 * @param[in]   
 * @return
 */
static void i2c_ack(RRD_DRIVER_IIC *driver)
{
	iic_to_write_mode(driver);
	
	iic_sda_0(driver);
	iic_delay();
	iic_scl_1(driver);
	iic_delay();
	iic_scl_0(driver);
}

/**
 * @brief  			CPU产生1个NACK信号
 * @param[in]   
 * @return
 */
static void i2c_nack(RRD_DRIVER_IIC *driver)
{	
	iic_to_write_mode(driver);
	
	iic_sda_1(driver);
	iic_delay();
	iic_scl_1(driver);
	iic_delay();
	iic_scl_0(driver);
}

/**
 * @brief  			CPU向I2C总线设备发送8bit数据
 * @param[in]   _ucByte  等待发送的字节
 * @note				先发送字节的高位bit7
 * @return
 */
static void i2c_send_byte(RRD_DRIVER_IIC *driver, uint8_t byte)
{
	iic_to_write_mode(driver);
	
	for (uint8_t i = 0; i < 8; i++)
	{		
		(byte & 0x80)? iic_sda_1(driver) 
								 : iic_sda_0(driver);
		iic_delay();
		iic_scl_1(driver);
		iic_delay();	
		iic_scl_0(driver);
		iic_delay();
		byte <<= 1;
	}
	iic_sda_1(driver); // 释放总线,让从机发送信号
}


/**
 * @brief  			CPU从I2C总线设备读取8bit数据	
 * @param[in]   
 * @return			读到的数据
 */
static uint8_t i2c_read_byte(RRD_DRIVER_IIC *driver, uint8_t ack)
{
	iic_to_read_mode(driver);
	
	uint8_t value;

	for (uint8_t i = 0; i < 8; i++)
	{
		value <<= 1;
		iic_scl_1(driver);
		iic_delay();
		if (iic_sda_read(driver)){
			value++;
		}
		iic_scl_0(driver);
		iic_delay();
	}
	
	(ack)? i2c_ack(driver) 
			 : i2c_nack(driver);

	return value;
}

/**
 * @brief  			CPU产生一个时钟，并读取器件的ACK应答信号
 * @param[in]   
 * @return			0:正确应答，1:无器件响应
 */
static uint8_t i2c_wait_ack(RRD_DRIVER_IIC *driver)
{
	iic_sda_1(driver);	// CPU释放SDA总线,让从机发送数据
	iic_delay();
	
	iic_to_read_mode(driver);
	
	iic_scl_1(driver);
	iic_delay();
	
	uint8_t re = iic_sda_read(driver);
	
	iic_scl_0(driver);
	iic_delay();
	return re;
}

/**
 * @brief  			检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
 * @param[in]   address 设备的I2C总线地址
 * @return			0:正确， 1:未探测到
 */
static uint8_t i2c_check_device(RRD_DRIVER_IIC *driver,uint8_t address)
{
	i2c_start(driver);
	
	i2c_send_byte(driver,address|I2C_WR);		// 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传
	uint8_t ack = i2c_wait_ack(driver);			// 检测设备的ACK应答

	i2c_stop(driver);

	return ack;
}

/**
 * @brief   	 连续读取数据
 * @param[in]  slave_add 	从机地址
 * @param[in]  reg_add 		目标寄存器地址
 * @param[in]  data_list	存放读取数据的缓冲区
 * @param[in]  num 			要读取的数据字节数
 * @return  	 0 成功  1 失败
 */
static uint8_t iic_read_buffer(RRD_DRIVER_IIC *driver,
								const uint8_t slave_add,
								const uint8_t reg_add,
								uint8_t* data_list,
								uint8_t num)
{
	i2c_start(driver);
	i2c_send_byte(driver,slave_add);
	i2c_wait_ack(driver);
	i2c_send_byte(driver,reg_add);
	i2c_wait_ack(driver);
	
	i2c_start(driver);
	i2c_send_byte(driver,slave_add+1);
	i2c_wait_ack(driver);
	
	while(num --){
		*data_list++ = i2c_read_byte(driver,num != 0);
	}
	
	i2c_stop(driver);
	return 0;
}

/**
 * @brief   	 连续发送数据
 * @param[in]  reg_add 	目标寄存器地址
 * @param[in]  data_list	存放读取数据的缓冲区
 * @param[in]  num 			要读取的数据字节数
 * @return  
 */
static uint8_t iic_write_buffer(RRD_DRIVER_IIC *driver,
								const uint8_t slave_add,
								const uint8_t reg_add,
								uint8_t* data_list,
								uint8_t num)
{
	i2c_start(driver);
	i2c_send_byte(driver,slave_add);
	i2c_wait_ack(driver);
	i2c_send_byte(driver,reg_add);
	i2c_wait_ack(driver);
	
	while(num--){
		i2c_send_byte(driver,*data_list++);
		i2c_wait_ack(driver);
	}   
	i2c_stop(driver);
	return 0;
}

void iic_config_gpios(RRD_DRIVER_IIC *driver,
						GPIO_TypeDef* scl_port,
						uint16_t scl_pin,
						GPIO_TypeDef* sda_port,
						uint16_t sda_pin,
						uint16_t timeout)
{
	driver->params.__scl_port = scl_port;
	driver->params.__scl_pin = scl_pin;
	driver->params.__sda_port = sda_port;
	driver->params.__sda_pin = sda_pin;
	driver->params.__timeout = timeout;
}

/******************************************************************************/
/*----------------------------------接口初始化--------------------------------*/
/******************************************************************************/
SOFT_IIC_DRIVER_INTERFACE_RRD SOFT_IIC_DRIVER_INTERFACE = {
	.read_byte = (soft_iic_read_byte_fn_t)i2c_read_byte,
	.send_byte = (soft_iic_send_byte_fn_t)i2c_send_byte,
	.read_buffer = (soft_iic_read_buffer_fn_t)iic_read_buffer,
	.write_buffer = (soft_iic_write_buffer_fn_t)iic_write_buffer,
	.config_gpios = (soft_iic_config_gpios_fn_t)iic_config_gpios,
	.check_device = (soft_iic_check_device_fn_t)i2c_check_device
};

/**
 * @brief  			初始化iic设备
 * @param[in]   driver iic设备指针
 * @return			0:初始化成功  -1:未初始化成功
 */
RRD_DRIVER_IIC *iic_driver_new(void)
{
	RRD_DRIVER_IIC* driver;
	driver = malloc(sizeof(*driver));
	if(NULL == driver){
		// debug_log("RRD_USART_MANAGER malloc failed");
		return NULL;
	}

	/** \addtogroup Removed 
	 ** \{  */
	// driver->__ack = i2c_ack;
	// driver->__nack = i2c_nack;
	// driver->__stop = i2c_stop;
	// driver->__start = i2c_start;
	// driver->__wait_ack = i2c_wait_ack;
	/** \}  */
	driver->interface = &SOFT_IIC_DRIVER_INTERFACE;
	return driver;
}
