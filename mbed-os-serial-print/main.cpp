#include "mbed.h"
#include "OV7670/OV7670.h"
#include "config.h"

#ifdef RCT6
OV7670_pinmap_t camera_pinmap = {	// RCT6
	.SIO_C = PC_0,
	.SIO_D = PC_1,
	.VSYNC = PC_2,
	.HERF = PC_3,

	.dataPort =
	{
		PA_0, PA_1, PA_2, PA_3, PA_4, PA_5, PA_6, PA_7,
		NC, NC, NC, NC, NC, NC, NC, NC
	},

	.RESET = PC_7,
	.PWDN = PC_8,

	.FIFO_WR = PB_15,
	.FIFO_WRST = PB_13,
	.FIFO_RCK = PC_6,
	.FIFO_RRST = PB_12,
	.FIFO_OE = PB_14,
};
#endif

#ifdef C8T6
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
#endif


void rx_handler();
int send_func(int);
void read_send(myCamera&);

RawSerial ser2usb(PA_9, PA_10, 256000);
volatile bool read_flag = false;

#ifdef RCT6
DigitalOut myled(PD_2, 1);
#endif

#ifdef C8T6
DigitalOut myled(PC_13, 1);
#endif

int main()
{
	myCamera camera(camera_pinmap);
	ser2usb.attach(&rx_handler);

	myled = 0;
	ThisThread::sleep_for(500);
	myled = 1;

	// for(uint8_t i = 0; i <= 0xC9; i++)
	// {
	// 	uint8_t reg = camera.rdCmos7670Reg(i);
	// 	ser2usb.printf("0x%02X\t0x%02X\n", i, reg);
	// 	wait_us(100);
	// }

	// read_send(camera);
	while (1)
	{
		if(read_flag)
		{
			read_send(camera);
		}
	}

	// camera.enable_auto_read(callback(&ser2usb, &RawSerial::putc));

	// while(1)
	// {
	// 	;
	// }
}

void rx_handler()
{
	ser2usb.getc();
	if(!read_flag)
		read_flag = true;
}

int send_func(int data)
{
	ser2usb.putc((data>>8)&0x00FF);
	ser2usb.putc(data);
	return 0;
}

void read_send(myCamera& camera)
{
	myled = 0;
	while (!camera.readable())
	{
		;
	}
	
	for(int i = 0; i < IMAGE_HEIGHT; i++)
	{
		for(int j = 0; j < IMAGE_WIDTH; j++)
		{
			uint16_t pixel = camera.read();
			uint8_t * pixel_b = (uint8_t *)&pixel;
			ser2usb.putc(*(pixel_b + 1));
			ser2usb.putc(*pixel_b);
			// ser2usb.printf("%d\n", i);
		}
	}
	read_flag = false;
	camera.abort_read();
	myled = 1;
}
