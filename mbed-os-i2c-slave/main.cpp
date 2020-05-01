/*
file name	:	main.cpp
create date :   2020-4-20 01:40:15
author		:	Yu Xiaoyuan
email		:	yuxiaoyuan0406@qq.com
*/
#include "mbed.h"
#include "OV7670/OV7670.h"
#include "config.h"

const uint8_t SLAVE_ADDRESS_L = 0xA0;
const uint8_t SLAVE_ADDRESS_H = 0xB0;

I2CSlave slave(I2C_SDA, I2C_SCL);
RawSerial ser2usb(PA_9, PA_10, 256000);
DigitalOut myled(PC_13, 1);
// InterruptIn s_addr(PB_2);
DigitalIn s_addr(PB_2);

// void toggle_addr();
// void toggle_addr_1(){toggle_addr();}
// void toggle_addr_2(){toggle_addr();}

OV7670_pinmap_t camera_pinmap = {	// C8T6
	.SIO_C = PB_0,
	.SIO_D = PB_1,
	.VSYNC = PB_10,
	.HERF = PB_11,

	.dataPort =
	{
		PA_0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7,
		NC, NC, NC, NC, NC, NC, NC, NC
	},

	.RESET = PB_4,
	.PWDN = PB_5,

	.FIFO_WR = PB_15,
	.FIFO_WRST = PB_13,
	.FIFO_RCK = PB_3,
	.FIFO_RRST = PB_12,
	.FIFO_OE = PB_14,
};

int main() {
   	myCamera camera(camera_pinmap);
    uint8_t current_addr = 0x00;
    uint8_t target_addr = 0x00;

    s_addr.mode(PullDown);
    // toggle_addr();
    // s_addr.rise(&toggle_addr_1);
    // s_addr.fall(&toggle_addr_2);

    myled = 0;
    wait_us(2000);
    myled = 1;      // 指示灯闪烁表示初始化完成

    int img_addr = 0;

    while (1)
    {
        target_addr = (s_addr.read()) ? SLAVE_ADDRESS_H : SLAVE_ADDRESS_L;
        if(current_addr ^ target_addr)
        {
            slave.address(target_addr);
            current_addr = target_addr;
        }
        // I2CSlave中方法receive存在问题
        // 初始化后receive函数总是返回0, 这是正常的
        // 但是有读写操作以后, 返回值会保持上一个状态, 无论是否有操作
        switch (slave.receive())
        {
            case I2CSlave::RxStatus::WriteAddressed:
            {
                char r_buf[2] = {0};
                int read_status = slave.read(r_buf, 2);
                if (read_status)
                {
                    printf("%d\n", read_status);
                }
                else
                {
                    ser2usb.printf("%02X %02X\n", r_buf[0], r_buf[1]);
                }
                break;
            }
            case I2CSlave::RxStatus::ReadAddressed:
            {
                if(img_addr == 0)
                {
                    myled = 0;
                    while(!camera.readable())
                    {
                        ;
                    }
                }

                uint16_t t_buf = camera.read();
                // uint16_t t_buf = 0xABCD;
                slave.write((char *)(&t_buf), 2);
                // if (send_status)
                // {
                //     ser2usb.printf("%d\n", send_status);
                // }
                // else
                // {
                //     ser2usb.puts("write done\n");
                // }
                img_addr++;
                
                if(img_addr == IMAGE_HEIGHT * IMAGE_WIDTH)
                {
                    img_addr = 0;
                    camera.abort_read();
                    myled = 1;
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

// void toggle_addr()
// {
//     myled = 0;
//     // wait_us(10);
//     if(s_addr.read() == 1)
//     {
//         slave.address(SLAVE_ADDRESS_H);
//         s_addr.fall(&toggle_addr);
//         s_addr.rise(NULL);
//     }
//     else
//     {
//         slave.address(SLAVE_ADDRESS_L);
//         s_addr.rise(&toggle_addr);
//         s_addr.fall(NULL);
//     }
//     myled = 1;
// }
