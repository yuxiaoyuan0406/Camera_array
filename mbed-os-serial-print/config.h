/*
file name	:	config.h
create date :   2020-4-14 18:35:44
author		:	Yu Xiaoyuan
email		:	yuxiaoyuan0406@qq.com
*/
#ifndef CONFIG_H
#define CONFIG_H

// User configuration
// #define SERIAL_OUTPUT_OV7670_STATUS
// #define RCT6
#define C8T6

// CMOS configuration
#define CMOS_QVGA_MODE
// #define CMOS_VGA_MODE

#ifdef CMOS_QVGA_MODE
#undef IMAGE_WIDTH 
#define IMAGE_WIDTH 320 
#undef IMAGE_HEIGHT
#define IMAGE_HEIGHT 240
#elif defined(CMOS_VGA_MODE)
#undef IMAGE_WIDTH 
#define IMAGE_WIDTH 640 
#undef IMAGE_HEIGHT
#define IMAGE_HEIGHT 480
#endif

#endif // !CONFIG_H
