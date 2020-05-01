/*
file name	:	OV7670.h
create date :   2020-4-8 17:19:46
author		:	Yu Xiaoyuan
email		:	yuxiaoyuan0406@qq.com
*/
#ifndef OV7670_H
#define OV7670_H

#include <cstdint>
#include "PortNames.h"
#include "PinNames.h"
#include "RawSerial.h"
#include "BusIn.h"
#include "DigitalInOut.h"
#include "DigitalOut.h"
#include "InterruptIn.h"

#define OV7670_DEVICE_WRITE_ADDRESS     0x42
#define OV7670_DEVICE_READ_ADDRESS      0x43

typedef struct
{
    PinName SIO_C;      //  i2c scl
    PinName SIO_D;      //  i2c sda
    PinName VSYNC;      //  帧同步信号
    PinName HERF;       //  行同步信号

    // PortName dataPort;  //  数据端口
    // int mask;           //  掩码
    PinName dataPort[16] = {NC};

    PinName RESET;      //  复位端口, 正常使用拉高
    PinName PWDN;       //  功耗选择模式, 正常使用拉低
    // PinName STROBE;  //  拍照闪光控制端口

    PinName FIFO_WR;    //  FIFO写控制端, 1为允许写入
    PinName FIFO_WRST;  //  FIFO写指针复位端
    PinName FIFO_RCK;   //  FIFO内存读取时钟控制端
    PinName FIFO_RRST;  //  FIFO读指针复位端
    PinName FIFO_OE;    //  FIFO关断控制
}OV7670_pinmap_t;

class myCamera
{
public:
    //  构造函数
    myCamera(OV7670_pinmap_t &static_pinmap);

    //  cmos控制函数
    void set_Cmos7670reg(void);
    uint8_t Cmos7670_init(void);
    uint8_t WrCmos7670(uint8_t regID, uint8_t regDat);
    uint8_t rdCmos7670Reg(uint8_t regID);
    // void Cmos7670_Size(unsigned int Startx, unsigned int Starty, unsigned int width, unsigned int height);

    //  读取控制
    bool readable() { return _readable; }
    uint16_t read();
    void abort_read();
    // void enable_auto_read(RawSerial* p) { _autoread = true; ser_p = p; }
    void enable_auto_read(Callback<int(int)>);
    void disable_auto_read() { _autoread = false; _send_func = nullptr; }

    virtual ~myCamera() {}

private:
    //  并行数据口
    // PortIn data_port;
    BusIn data_port;
    //  软I2C引脚
    DigitalOut I2C_SCL0;
    DigitalInOut I2C_SDA0;
    DigitalInOut &SDA_STATE0 = I2C_SDA0;
    //  同步信号
    InterruptIn vsync;
    void vsync_handler(void);

    //  控制引脚
    DigitalOut reset;
    DigitalOut pwdn;
    
    //  FIFO控制引脚
    DigitalOut FIFO_WR;
    DigitalOut FIFO_WRST;
    DigitalOut FIFO_RCK;
    DigitalOut FIFO_RRST;
    DigitalOut FIFO_OE;

    //  可读标志位
    volatile bool _readable;

    //  自动读取标志位
    bool _autoread;

    //  自动读取输出串口
    // RawSerial* ser_p;
    //  自动读取输出函数
    Callback<int(int)> _send_func;

    //  vsync标志位
	volatile uint8_t Vsync_Flag = 0;

    //  软I2C函数
    inline void SDA_IN0() { I2C_SDA0.input(); I2C_SDA0.mode(PullNone); }
    inline void SDA_OUT0() { I2C_SDA0.output(); }
    void InitI2C0(void);  //  I2C初始化
    void StartI2C0(void); //  起始信号
    void StopI2C0(void);  //  停止信号
    void NoAck0(void);    //  非应答信号
    void Ack0(void);
    uint8_t TestAck0(void);
    uint8_t I2CWrite0(uint8_t DData); //  写一个字节
    uint8_t I2CRead0(void);                 //   读一个字节
};

#endif  //  !OV7670_H
