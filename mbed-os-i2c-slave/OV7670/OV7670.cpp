/*
file name	:	OV7670.cpp
create date :   2020-4-8 17:19:46
author		:	Yu Xiaoyuan
email		:	yuxiaoyuan0406@qq.com
*/
#include "mbed.h"
// #include "mbed_config.h"
#include "OV7670/OV7670.h"
#include "OV7670/cmos_register.h"
#include "config.h"

#ifdef SERIAL_OUTPUT_OV7670_STATUS
extern RawSerial ser2usb;
#endif

inline void delay_us(int us)
{
	wait_us(us);
}

inline void delay_ms(int ms)
{
	wait_us(ms*1000);
}

const uint8_t cmos_addr = 0x42;

myCamera::myCamera(OV7670_pinmap_t &static_pinmap) : 
	// data_port(static_pinmap.dataPort, static_pinmap.mask),
	data_port(static_pinmap.dataPort),
	I2C_SCL0(static_pinmap.SIO_C),
	I2C_SDA0(static_pinmap.SIO_D),
	vsync(static_pinmap.VSYNC, PullUp),
	reset(static_pinmap.RESET, 1),
	pwdn(static_pinmap.PWDN, 0),
	FIFO_WR(static_pinmap.FIFO_WR),
	FIFO_WRST(static_pinmap.FIFO_WRST),
	FIFO_RCK(static_pinmap.FIFO_RCK),
	FIFO_RRST(static_pinmap.FIFO_RRST),
	FIFO_OE(static_pinmap.FIFO_OE),
	_readable(false),
	_autoread(false),
	_send_func(nullptr)
{
#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Calling myCamera class constructor\n");
#endif

	InitI2C0();
	for(int i = 1; 1 != Cmos7670_init(); i++)
	{
#ifdef SERIAL_OUTPUT_OV7670_STATUS
		ser2usb.printf("DEBUG MESSAGE: Calling myCamera::Cmos7670_init() %d\n", i);
#endif
		;
	}
#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: OV7670 initiate successfully\n");
#endif
	vsync.fall(callback(this, &myCamera::vsync_handler));

#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Exiting myCamera class constructor\n");
#endif
}

/***************************************************************************
名    称：void myCamera::vsync_handler(void)
功    能：帧同步信号处理函数
入口参数：无
出口参数：无
说    明：
调用方法：中断信号调用
***************************************************************************/
void myCamera::vsync_handler(void)
{

	Vsync_Flag++;
	switch (Vsync_Flag)
	{
	case 1:
	{
		_readable = false;
		FIFO_WRST = 1;	// FIFO写指针复位
		FIFO_WRST = 0;
		for (int i = 0; i < 100; i++)
			;
		FIFO_WRST = 1;
		FIFO_WR = 1; 	// 允许CMOS数据写入FIFO
		break;
	}
	case 2:
	{
		FIFO_WR = 0;	// 禁止CMOS数据写入FIFO
		// vsync.disable_irq();	// 禁止外部中断

		FIFO_RRST = 0;	// FIFO读指针开始复位
		FIFO_RCK = 0;
		FIFO_RCK = 1;
		FIFO_RCK = 0;
		FIFO_RCK = 1;
		FIFO_RRST = 1;	// FIFO读指针完成复位

		FIFO_OE = 0;	// 允许FIFO输出
		_readable = true;

		if(_autoread)
		{
			vsync.disable_irq();
			for(int i = 0; i < IMAGE_HEIGHT; i++)
			{
				for(int j = 0; j < IMAGE_WIDTH; j++)
				{
					// uint16_t pixel = read();
					//	todo: auto read method
					// uint16_t pixel = 0x0000;
					FIFO_RCK = 0;
					FIFO_RCK = 1;
					// pixel = (uint16_t)(data_port.read() & 0x00FF) << 8;
					if(_send_func)
					{
						_send_func(data_port.read());
					}
					FIFO_RCK = 0;
					FIFO_RCK = 1;
					// pixel |= (uint16_t)(data_port.read() & 0x00FF);
					if(_send_func)
					{
						_send_func(data_port.read());
					}

					// if(_send_func)
					// {
					// 	_send_func(pixel);
					// }
				}
			}
			abort_read();
		}

		Vsync_Flag = 0;
		break;
	}
	default:
		break;
	}
}

/***************************************************************************
名    称：void myCamera::abort_read(void)
功    能：结束读取
入口参数：无
出口参数：无
说    明：
调用方法：abort_read();
***************************************************************************/
void myCamera::abort_read()
{
	FIFO_OE = 1;			// 禁止FIFO输出
	_readable = false;
	vsync.enable_irq();		// 允许外部中断
}

/***************************************************************************
名    称：uint16_t myCamera::read(void)
功    能：读取一个像素的数据
入口参数：无
出口参数：16位
说    明：
调用方法：abort_read();
***************************************************************************/
uint16_t myCamera::read()
{
#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Calling myCamera::read()\n");
#endif

	vsync.disable_irq();

	uint16_t pixel = 0;
	FIFO_RCK = 0;
	FIFO_RCK = 1;
	pixel = (uint16_t)(data_port.read() & 0x00FF) << 8;
	FIFO_RCK = 0;
	FIFO_RCK = 1;
	pixel |= (uint16_t)(data_port.read() & 0x00FF);

#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Exiting myCameramyCamera::read()\n");
#endif

	return pixel;
}

/***************************************************************************
名    称：void myCamera::InitI2C0(void)
功    能：初始化IO口
入口参数：无
出口参数：无
说    明：
调用方法：InitI2C0();
***************************************************************************/
void myCamera::InitI2C0(void)
{
	// RCC->APB2ENR |= 1 << 4;	  //先使能外设IO PORTC时钟
	// GPIOC->CRH &= 0XFFF00FFF; //PC11/12 推挽输出
	// GPIOC->CRH |= 0X00033000;
	// GPIOC->ODR |= 3 << 11; //PC11,12 输出高
#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Calling myCamera::InitI2C0()\n");
#endif

	I2C_SDA0.output();
	I2C_SCL0.write(1);
	I2C_SDA0.write(1);

#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Exiting myCameramyCamera::InitI2C0()\n");
#endif
}

/***************************************************************************
名    称：void myCamera::StartI2C0(void)
功    能：启动I2C总线，即发送I2C初始条件
入口参数：无
出口参数：无
说    明：
调用方法：StartI2C0();
***************************************************************************/
void myCamera::StartI2C0(void)
{
	I2C_SDA0 = 1;
	delay_us(100);

	I2C_SCL0 = 1;
	delay_us(100);

	I2C_SDA0 = 0;
	delay_us(100);

	I2C_SCL0 = 0;
	delay_us(100);
}

/***************************************************************************
名    称：void myCamera::StopI2C0(void)
功    能：结束I2C总线，即发送I2C结束条件
入口参数：无
出口参数：无
说    明：
调用方法：StopI2C0();
***************************************************************************/
void myCamera::StopI2C0(void)
{
	I2C_SDA0 = 0;
	delay_us(100);

	I2C_SCL0 = 1;
	delay_us(100);

	I2C_SDA0 = 1;
	delay_us(100);
}

/***************************************************************************
名    称：void myCamera::NoAck0(void)
功    能：无应答
入口参数：无
出口参数：无
说    明：
调用方法：NoAck0();
***************************************************************************/
void myCamera::NoAck0(void)
{

	I2C_SDA0 = 1;
	delay_us(100);

	I2C_SCL0 = 1;
	delay_us(100);

	I2C_SCL0 = 0;
	delay_us(100);

	I2C_SDA0 = 0;
	delay_us(100);
}

/***************************************************************************
名    称：void myCamera::Ack0(void)
功    能：总线应答
入口参数：无
出口参数：无
说    明：
调用方法：Ack0();
***************************************************************************/
void myCamera::Ack0(void)
{
	SDA_OUT0();
	I2C_SDA0 = 1;
	delay_us(100);
	I2C_SCL0 = 1;
	delay_us(100);
	I2C_SCL0 = 0;
	delay_us(100);
	I2C_SDA0 = 1;
}

/***************************************************************************
名    称：uint8_t myCamera::TestAck0( )
功    能：应答校验
入口参数：无
出口参数：应答状态
说    明：
调用方法：m=TestAck0();
***************************************************************************/
uint8_t myCamera::TestAck0()
{
	uint8_t ack;

	I2C_SCL0 = 1;
	delay_us(100);
	SDA_IN0();
	delay_us(100);
	ack = SDA_STATE0;
	delay_us(100);
	I2C_SCL0 = 0;
	delay_us(100);
	return (ack);
}

/***************************************************************************
名    称：uint8_t myCamera::I2CWrite0(uint8_t DData)
功    能：向I2C总线上发送1字节数据
入口参数：DData    待发送数据
出口参数：无
说    明：
调用方法：I2CWrite0(data);
***************************************************************************/
uint8_t myCamera::I2CWrite0(uint8_t DData)
{
	uint8_t tem;

	for (int j = 0; j < 8; j++)
	{
		if ((DData << j) & 0x80)
		{
			I2C_SDA0 = 1;
		}
		else
		{
			I2C_SDA0 = 0;
		}
		delay_us(100);
		I2C_SCL0 = 1;
		delay_us(100);
		I2C_SCL0 = 0;
		delay_us(100);
	}
	delay_us(100);

	SDA_IN0();
	delay_us(100);
	I2C_SCL0 = 1;
	delay_us(1000);
	if (SDA_STATE0 == 1)
	{
		tem = 0;
	}
	else
	{
		tem = 1;
	}
	I2C_SCL0 = 0;
	delay_us(100);
	SDA_OUT0();

	return (tem);
}

/***************************************************************************
名    称：uint8_t myCamera::I2CRead0(void)
功    能：从I2C总线上接收1字节数据
入口参数：无
出口参数：接收数据
说    明：
调用方法：m=I2CRead0();
***************************************************************************/
uint8_t myCamera::I2CRead0(void)
{
	uint8_t read, j;
	read = 0x00;

	SDA_IN0();
	delay_us(100);
	for (j = 8; j > 0; j--)
	{
		delay_us(100);
		I2C_SCL0 = 1;
		delay_us(100);
		read = read << 1;
		if (SDA_STATE0 == 1)
		{
			read = read + 1;
		}
		I2C_SCL0 = 0;
		delay_us(100);
	}
	return (read);
}

/***************************************************************************
名    称：uint8_t myCamera::WrCmos7670(uint8_t regID, uint8_t regDat)
功    能：向CMOS指定寄存器内写值
入口参数：regID  指定寄存器地址
          regDat 待写入指定寄存器的值 
出口参数：1    写入成功
          0    写入失败
说    明：
调用方法：m=WrCmos7670(0x3a,0x04);
***************************************************************************/
uint8_t myCamera::WrCmos7670(uint8_t regID, uint8_t regDat)
{
	StartI2C0();
	if (0 == I2CWrite0(OV7670_DEVICE_WRITE_ADDRESS)) //CMOS器件地址（写）
	{
		StopI2C0();
		return (0);
	}
	delay_us(100);
	if (0 == I2CWrite0(regID)) //CMOS寄存器地址
	{
		StopI2C0();
		return (0);
	}
	delay_us(100);
	if (0 == I2CWrite0(regDat)) //待写入指定寄存器的值
	{
		StopI2C0();
		return (0);
	}
	StopI2C0();

	return (1);
}

uint8_t myCamera::rdCmos7670Reg(uint8_t regID)
{
#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Calling myCamera::rdCmos7670Reg(uint8_t)\n");
#endif

	uint8_t regDat;
	StartI2C0();
	if (0 == I2CWrite0(OV7670_DEVICE_WRITE_ADDRESS))
	{
		StopI2C0();
		return (0);
	}
	delay_us(100);
	if (0 == I2CWrite0(regID))
	{
		StopI2C0();
		return (0);
	}
	StopI2C0();

	delay_us(100);

	StartI2C0();
	if (0 == I2CWrite0(OV7670_DEVICE_READ_ADDRESS))
	{
		StopI2C0();
		return (0);
	}
	delay_us(100);
	regDat = I2CRead0();
	NoAck0();
	StopI2C0();

#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Exiting myCamera::rdCmos7670Reg(uint8_t)\n");
#endif

	return regDat;
}

/***************************************************************************
名    称：void myCamera::set_Cmos7670reg(void)
功    能：CMOS寄存器配置
入口参数：无
出口参数：无
说    明：
调用方法：set_Cmos7670reg();
***************************************************************************/
void myCamera::set_Cmos7670reg(void)
{	 

	WrCmos7670(0x3a, 0x04);
	//	数据字节格式
	WrCmos7670(CMOS_COM15, 0xd0);	// RGB 565

	//	数据格式设置
#ifdef CMOS_QVGA_MODE
	WrCmos7670(CMOS_COM7, 0x14);	// QVGA & RGB
	// WrCmos7670(CMOS_COM7, 0x11);	// QVGA & Bayer RAW
#elif defined(CMOS_VGA_MODE)
	WrCmos7670(CMOS_COM7, 0x04);	//  VGA & RGB
#endif

	WrCmos7670(CMOS_HERF, 0x80);
	WrCmos7670(0x17, 0x16);
	WrCmos7670(0x18, 0x04);
	WrCmos7670(0x19, 0x02);
	WrCmos7670(0x1a, 0x7b);
	WrCmos7670(0x03, 0x06);
	WrCmos7670(0x0c, 0x00);
	WrCmos7670(0x3e, 0x00);
	WrCmos7670(0x70, 0x3a);
	WrCmos7670(0x71, 0x35);
	WrCmos7670(0x72, 0x11);
	WrCmos7670(0x73, 0x00);
	WrCmos7670(0xa2, 0x02);
	WrCmos7670(0x11, 0x81);
	
	WrCmos7670(0x7a, 0x20);
	WrCmos7670(0x7b, 0x1c);
	WrCmos7670(0x7c, 0x28);
	WrCmos7670(0x7d, 0x3c);
	WrCmos7670(0x7e, 0x55);
	WrCmos7670(0x7f, 0x68);
	WrCmos7670(0x80, 0x76);
	WrCmos7670(0x81, 0x80);
	WrCmos7670(0x82, 0x88);
	WrCmos7670(0x83, 0x8f);
	WrCmos7670(0x84, 0x96);
	WrCmos7670(0x85, 0xa3);
	WrCmos7670(0x86, 0xaf);
	WrCmos7670(0x87, 0xc4);
	WrCmos7670(0x88, 0xd7);
	WrCmos7670(0x89, 0xe8);
	
	WrCmos7670(0x13, 0xe0);
	WrCmos7670(0x00, 0x00);
	
	WrCmos7670(0x10, 0x00);
	WrCmos7670(0x0d, 0x00);
	WrCmos7670(0x14, 0x28);
	WrCmos7670(0xa5, 0x05);
	WrCmos7670(0xab, 0x07);
	WrCmos7670(0x24, 0x75);
	WrCmos7670(0x25, 0x63);
	WrCmos7670(0x26, 0xA5);
	WrCmos7670(0x9f, 0x78);
	WrCmos7670(0xa0, 0x68);
	WrCmos7670(0xa1, 0x03);
	WrCmos7670(0xa6, 0xdf);
	WrCmos7670(0xa7, 0xdf);
	WrCmos7670(0xa8, 0xf0);
	WrCmos7670(0xa9, 0x90);
	WrCmos7670(0xaa, 0x94);
	WrCmos7670(0x13, 0xe5);

	WrCmos7670(0x0e, 0x61);
	WrCmos7670(0x0f, 0x4b);
	WrCmos7670(0x16, 0x02);
	WrCmos7670(0x1e, 0x37);
	WrCmos7670(0x21, 0x02);
	WrCmos7670(0x22, 0x91);
	WrCmos7670(0x29, 0x07);
	WrCmos7670(0x33, 0x0b);
	WrCmos7670(0x35, 0x0b);
	WrCmos7670(0x37, 0x1d);
	WrCmos7670(0x38, 0x71);
	WrCmos7670(0x39, 0x2a);
	WrCmos7670(0x3c, 0x78);
	WrCmos7670(0x4d, 0x40);
	WrCmos7670(0x4e, 0x20);
	WrCmos7670(0x69, 0x00);
	WrCmos7670(0x6b, 0x60);
	WrCmos7670(0x74, 0x19);
	WrCmos7670(0x8d, 0x4f);
	WrCmos7670(0x8e, 0x00);
	WrCmos7670(0x8f, 0x00);
	WrCmos7670(0x90, 0x00);
	WrCmos7670(0x91, 0x00);
	WrCmos7670(0x92, 0x00);
	WrCmos7670(0x96, 0x00);
	WrCmos7670(0x9a, 0x80);
	WrCmos7670(0xb0, 0x84);
	WrCmos7670(0xb1, 0x0c);
	WrCmos7670(0xb2, 0x0e);
	WrCmos7670(0xb3, 0x82);
	WrCmos7670(0xb8, 0x0a);



	WrCmos7670(0x43, 0x14);
	WrCmos7670(0x44, 0xf0);
	WrCmos7670(0x45, 0x34);
	WrCmos7670(0x46, 0x58);
	WrCmos7670(0x47, 0x28);
	WrCmos7670(0x48, 0x3a);
	WrCmos7670(0x59, 0x88);
	WrCmos7670(0x5a, 0x88);
	WrCmos7670(0x5b, 0x44);
	WrCmos7670(0x5c, 0x67);
	WrCmos7670(0x5d, 0x49);
	WrCmos7670(0x5e, 0x0e);
	WrCmos7670(0x64, 0x04);
	WrCmos7670(0x65, 0x20);
	WrCmos7670(0x66, 0x05);
	WrCmos7670(0x94, 0x04);
	WrCmos7670(0x95, 0x08);
	WrCmos7670(0x6c, 0x0a);
	WrCmos7670(0x6d, 0x55);
	WrCmos7670(0x6e, 0x11);
	WrCmos7670(0x6f, 0x9f);
	WrCmos7670(0x6a, 0x40);
	WrCmos7670(0x01, 0x40);
	WrCmos7670(0x02, 0x40);
	WrCmos7670(0x13, 0xe7);
	WrCmos7670(0x15, 0x00);  
	
	
	WrCmos7670(0x4f, 0x80);
	WrCmos7670(0x50, 0x80);
	WrCmos7670(0x51, 0x00);
	WrCmos7670(0x52, 0x22);
	WrCmos7670(0x53, 0x5e);
	WrCmos7670(0x54, 0x80);
	WrCmos7670(0x58, 0x9e);
	
	WrCmos7670(0x41, 0x08);
	WrCmos7670(0x3f, 0x00);
	WrCmos7670(0x75, 0x05);
	WrCmos7670(0x76, 0xe1);
	WrCmos7670(0x4c, 0x00);
	WrCmos7670(0x77, 0x01);
	WrCmos7670(CMOS_COM13, 0xc2);	
	WrCmos7670(0x4b, 0x09);
	WrCmos7670(0xc9, 0x60);
	WrCmos7670(0x41, 0x38);
	WrCmos7670(0x56, 0x40);
	
	WrCmos7670(0x34, 0x11);
	WrCmos7670(0x3b, 0x02); 
								
	WrCmos7670(0xa4, 0x89);
	WrCmos7670(0x96, 0x00);
	WrCmos7670(0x97, 0x30);
	WrCmos7670(0x98, 0x20);
	WrCmos7670(0x99, 0x30);
	WrCmos7670(0x9a, 0x84);
	WrCmos7670(0x9b, 0x29);
	WrCmos7670(0x9c, 0x03);
	WrCmos7670(0x9d, 0x4c);
	WrCmos7670(0x9e, 0x3f);
	WrCmos7670(0x78, 0x04);
	
	WrCmos7670(0x79, 0x01);
	WrCmos7670(0xc8, 0xf0);
	WrCmos7670(0x79, 0x0f);
	WrCmos7670(0xc8, 0x00);
	WrCmos7670(0x79, 0x10);
	WrCmos7670(0xc8, 0x7e);
	WrCmos7670(0x79, 0x0a);
	WrCmos7670(0xc8, 0x80);
	WrCmos7670(0x79, 0x0b);
	WrCmos7670(0xc8, 0x01);
	WrCmos7670(0x79, 0x0c);
	WrCmos7670(0xc8, 0x0f);
	WrCmos7670(0x79, 0x0d);
	WrCmos7670(0xc8, 0x20);
	WrCmos7670(0x79, 0x09);
	WrCmos7670(0xc8, 0x80);
	WrCmos7670(0x79, 0x02);
	WrCmos7670(0xc8, 0xc0);
	WrCmos7670(0x79, 0x03);
	WrCmos7670(0xc8, 0x40);
	WrCmos7670(0x79, 0x05);
	WrCmos7670(0xc8, 0x30);
	WrCmos7670(0x79, 0x26); 
	WrCmos7670(0x09, 0x00);	
	
	
}

/***************************************************************************
名    称：uint8_t myCamera::Cmos7670_init(void)
功    能：CMOS初始化
入口参数：无
出口参数：1 初始化成功
          0 初始化失败
说    明：
调用方法：m=Cmos7670_init();
***************************************************************************/
uint8_t myCamera::Cmos7670_init(void)
{
	const uint8_t mmm = 1 << 7;	

	
	InitI2C0();

	if(0==WrCmos7670(CMOS_COM7, mmm)) 	// 重置所有寄存器
	{
		return 0 ;
	}
	delay_ms(10);

  	set_Cmos7670reg();

	return 1; 
} 

/***************************************************************************
名    称：void myCamera::enable_auto_read(Callback<int(int)>)
功    能：启动自动读取并设置数据发送函数
入口参数：无
出口参数：无
说    明：
调用方法：enable_auto_read(&func);
		enable_auto_read(callbacn(obj, method));
***************************************************************************/
void myCamera::enable_auto_read(Callback<int(int)> func)
{
#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Calling myCamera::enable_auto_read\n");
#endif

	_autoread = true;
	if(func)
	{
		_send_func = func;
	}
	else
	{
		_send_func = nullptr;
#ifdef SERIAL_OUTPUT_OV7670_STATUS
		ser2usb.printf("DEBUG MESSAGE: _send_func attached to NULL\n");
#endif
	}

#ifdef SERIAL_OUTPUT_OV7670_STATUS
	ser2usb.printf("DEBUG MESSAGE: Exiting myCamera::enable_auto_read\n");
#endif
}

