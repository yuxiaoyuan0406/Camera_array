/*
file name	:	main.cpp
create date :   2020-4-20 01:40:15
author		:	Yu Xiaoyuan
email		:	yuxiaoyuan0406@qq.com
*/
#include "mbed.h"
#include "config.h"
#include <cstring>

I2C master(I2C_SDA, I2C_SCL);
RawSerial ser2usb(PA_9, PA_10, 256000);
InterruptIn key0(PC_1, PullUp);
InterruptIn key1(PC_13, PullUp);
BusOut cmos_array(PC_0);

const uint8_t SLAVE_ADDRESS = 0xA0;
const uint8_t WRITE = 0x00;
const uint8_t READ = 0x01;

volatile bool read_done = true;
volatile bool write_done = true;

void ser_irq_handler();
void key0_irq_handler();
void key1_irq_handler();

int main()
{
    char _cmd[2] = {0x12, 0x34};
    char _buf[2] = {0};

    master.frequency(400000);
    cmos_array.write(0xFFFF);
    ser2usb.attach(&ser_irq_handler);
    key0.fall(&key0_irq_handler);
    key1.fall(&key1_irq_handler);

    // for(int i = 0; i <= 0xFF; i++)
    // {
    //     master.start();
    //     int status = master.write(i);
    //     master.stop();
    //     ser2usb.printf("0x%02X\t%d\n", i, status);
    //     ThisThread::sleep_for(10);
    // }

    ThisThread::sleep_for(20);
    // if (master.write((SLAVE_ADDRESS | WRITE), _cmd, 2, false) != 0)
    // {
    //     ser2usb.printf("No ack\n");
    // }
    // else
    // {
    //     ser2usb.printf("write done.\n");
    // }
    while (1)
    {
        if (!read_done)
        {
            for(int i = 0; i < IMAGE_HEIGHT; i++)
            {
                for(int j = 0; j < IMAGE_WIDTH; j++)
                {
                    if (master.read((SLAVE_ADDRESS | READ), _buf, 2) != 0)
                    {
                        // ser2usb.puts("failed\n");
                        ser2usb.putc(0);
                        ser2usb.putc(0);
                    }
                    else
                    {
                        // ser2usb.printf("read: %02X %02X\n", _buf[0], _buf[1]);
                        ser2usb.putc(_buf[1]);
                        ser2usb.putc(_buf[0]);
                    }
                    _buf[0] = _buf[1] = 0;
                }
            }
            read_done = true;
        }
        if (!write_done)
        {
            if (master.write((SLAVE_ADDRESS | WRITE), _cmd, 2) != 0)
            {
                ser2usb.puts("failed\n");
            }
            else
            {
                ser2usb.printf("write done\n");
            }
            write_done = true;
        }
    }
}

void ser_irq_handler()
{
    char ser_rx = ser2usb.getc();
    switch (ser_rx)
    {
    case 'r':
    {
        read_done = false;
        break;
    }
    case 'w':
    {
        write_done = false;
        break;
    }
    default:
    {
        ser2usb.putc(ser_rx);
        break;
    }
    }
}

void key0_irq_handler()
{
    wait_us(100);
    if(key0.read() == 0)
    {
        read_done = false;
    }
}

void key1_irq_handler()
{
    wait_us(100);
    if(key1.read() == 0)
    {
        write_done = false;
    }
}
