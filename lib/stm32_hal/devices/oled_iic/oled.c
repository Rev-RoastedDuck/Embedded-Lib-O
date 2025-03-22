/*
 * oled.c
 *	
 *  Created on: 2024_11_01
 *      Author: Rev_RoastDuck
 *      Github: https://github.com/Rev-RoastedDuck
 * 
 * :copyright: (c) 2023 by Rev-RoastedDuck.
 */
 
#include "oled.h"
#include "oled_font.h"

/******************************************************************************/
/*----------------------------------Adaptation--------------------------------*/
/******************************************************************************/
/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
static void oled_write_command(RRD_DEVICE_OLED*self, uint8_t command)
{
	iic_device_write_buffer(self->__iic_device,0x00,&command,1);
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
static void oled_write_data(RRD_DEVICE_OLED*self, uint8_t data)
{
	iic_device_write_buffer(self->__iic_device,0x40,&data,1);
}

/******************************************************************************/
/*----------------------------------FUNCTION----------------------------------*/
/******************************************************************************/
/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void oled_set_cursor(RRD_DEVICE_OLED*self, uint8_t Y, uint8_t X)
{
	oled_write_command(self, 0xB0 | Y);									//设置Y位置
	oled_write_command(self, 0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	oled_write_command(self, 0x00 | (X & 0x0F));					//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void oled_clear(RRD_DEVICE_OLED*self)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++){
		oled_set_cursor(self, j, 0);
		for(i = 0; i < 128; i++){
			oled_write_data(self, 0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void oled_show_char(RRD_DEVICE_OLED *self, uint8_t Line, uint8_t Column, char Char)
{  
	#if USE_OLED_F8x16
	uint8_t i;
	oled_set_cursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		oled_write_data(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	oled_set_cursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		oled_write_data(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
	#else
	uint8_t i;
	oled_set_cursor(self, (Line - 1) + 1, (Column - 1) * 6 +2);
	for (i = 0; i < 6; i++){
		oled_write_data(self, OLED_F6x8[Char - ' '][i]);
	}
	#endif
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void oled_show_string(RRD_DEVICE_OLED*self, uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++){
		oled_show_char(self, Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t oled_pow(RRD_DEVICE_OLED*self, uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--){
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void oled_show_num(RRD_DEVICE_OLED *self, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++){
		oled_show_char(self, Line, Column + i, Number / oled_pow(self, 10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void oled_show_signed_num(RRD_DEVICE_OLED *self, uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0){
		oled_show_char(self, Line, Column, '+');
		Number1 = Number;
	}
	else{
		oled_show_char(self, Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++){
		oled_show_char(self, Line, Column + i + 1, Number1 / oled_pow(self, 10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void oled_show_hex_num(RRD_DEVICE_OLED*self, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++){
		SingleNumber = Number / oled_pow(self, 16, Length - i - 1) % 16;
		if (SingleNumber < 10){
			oled_show_char(self, Line, Column + i, SingleNumber + '0');
		}
		else{
			oled_show_char(self, Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void oled_show_bin_num(RRD_DEVICE_OLED*self, uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++){
		oled_show_char(self, Line, Column + i, Number / oled_pow(self, 2, Length - i - 1) % 2 + '0');
	}
}


/**
  * @brief  OLED显示浮点数
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的浮点数
  * @param  Precision 小数点后位数
  * @retval 无
  */
void oled_show_float(RRD_DEVICE_OLED*self, uint8_t Line, uint8_t Column, float Number, uint8_t Precision) {
    char buffer[20];
    snprintf(buffer, 20, Number >= 0? "+%.*f" : "%.*f", Precision, Number);
    oled_show_string(self, Line, Column, buffer);
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void oled_init(RRD_DEVICE_OLED *self)
{
	uint32_t i, j;
	
	for (i = 0; i < 500; i++){
		for (j = 0; j < 500; j++);
	}
	
	if(iic_device_check_device(self->__iic_device)){
		DEBUG_PRINT(0,"not responding");
	}
	
	oled_write_command(self, 0xAE);	//关闭显示
	
	oled_write_command(self, 0xD5);	//设置显示时钟分频比/振荡器频率
	oled_write_command(self, 0x80);
	
	oled_write_command(self, 0xA8);	//设置多路复用率
	oled_write_command(self, 0x3F);
	
	oled_write_command(self, 0xD3);	//设置显示偏移
	oled_write_command(self, 0x00);
	
	oled_write_command(self, 0x40);	//设置显示开始行
	
	oled_write_command(self, 0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	oled_write_command(self, 0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	oled_write_command(self, 0xDA);	//设置COM引脚硬件配置
	oled_write_command(self, 0x12);
	
	oled_write_command(self, 0x81);	//设置对比度控制
	oled_write_command(self, 0xCF);

	oled_write_command(self, 0xD9);	//设置预充电周期
	oled_write_command(self, 0xF1);

	oled_write_command(self, 0xDB);	//设置VCOMH取消选择级别
	oled_write_command(self, 0x30);

	oled_write_command(self, 0xA4);	//设置整个显示打开/关闭

	oled_write_command(self, 0xA6);	//设置正常/倒转显示

	oled_write_command(self, 0x8D);	//设置充电泵
	oled_write_command(self, 0x14);

	oled_write_command(self, 0xAF);	//开启显示
		
	oled_clear(self);				//OLED清屏
}

/******************************************************************************/
/*--------------------------Construction/Destruction--------------------------*/
/******************************************************************************/
void oled_del(void **self){
    if (self != NULL && *self != NULL) {
        free(*self);
        *self = NULL;
    }
}

/**
 * @brief  			初始化iic设备
 * @param[in]   driver iic设备指针
 * @return			0:初始化成功  -1:未初始化成功
 */
RRD_DEVICE_OLED *oled_device_new(void *iic_driver,bool use_soft_iic,uint8_t slave_address,uint16_t timeout)
{	
	if(NULL == iic_driver){
		// debug_log("RRD_USART_MANAGER malloc failed");
		return NULL;
	}
	
	RRD_DEVICE_OLED* self;
	self = malloc(sizeof(*self));
	if(NULL == self){
		// debug_log("RRD_USART_MANAGER malloc failed");
		return NULL;
	}

	self->__iic_device = iic_device_new(iic_driver,use_soft_iic,slave_address,timeout);
	if(NULL == self->__iic_device){
		// debug_log("RRD_USART_MANAGER malloc failed");
		return NULL;
	}

	self->del = oled_del;
	self->init = oled_init;
	self->clear = oled_clear;
	self->show_char = oled_show_char;
	self->show_string = oled_show_string;
	self->show_num = oled_show_num;
	self->show_signed_num = oled_show_signed_num;
	self->show_hex_num = oled_show_hex_num;
	self->show_bin_num = oled_show_bin_num;
	self->show_float = oled_show_float;
	
	self->init(self);
	
	return self;
}

/******************************************************************************/
/*-------------------------------------DEBUG----------------------------------*/
/******************************************************************************/
#if OPEN_OLED_TEST
#include "mpu6050.h"
#include "mpu6050_dmp.h"
void oled_test_start(void){
	RRD_DRIVER_IIC *g_OLED_IIC_DEVICE = iic_driver_new();
	g_OLED_IIC_DEVICE->config_gpios(g_OLED_IIC_DEVICE,IIC_SCL_GPIO_Port,
																	IIC_SCL_Pin,IIC_SDA_GPIO_Port,IIC_SDA_Pin);
	RRD_DEVICE_OLED *oled = oled_device_new(g_OLED_IIC_DEVICE,true,0x78,1);
	oled->init(oled);
	oled->show_num(oled,1,1,22,2);
	oled->show_float(oled,2, 1, 1.222, 2);
	oled->show_string(oled,3,1,"roastduck");
}
#endif /* \} if OPEN_OLED_TEST*/

