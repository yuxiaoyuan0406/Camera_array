/*
file name	:	cmos_register.h
create date :   2020-4-8 17:19:46
author		:	Yu Xiaoyuan
email		:	yuxiaoyuan0406@qq.com
*/
#ifndef CMOS_REGISTER_H
#define CMOS_REGISTER_H

#define     CMOS_GAIN               0x00
#define     CMOS_BLUE               0x01
#define     CMOS_RED                0x02
#define     CMOS_VERF               0x03
#define     CMOS_COM1               0x04
#define     CMOS_BAVE               0x05
#define     CMOS_GbAVE              0x06
#define     CMOS_AECHH              0x07
#define     CMOS_RAVE               0x08
#define     CMOS_COM2               0x09
#define     CMOS_PID                0x0A
#define     CMOS_VER                0x0B
#define     CMOS_COM3               0x0C
#define     CMOS_COM4               0x0D
#define     CMOS_COM5               0x0E
#define     CMOS_COM6               0x0F
#define     CMOS_AECH               0x10
#define     CMOS_CLKRC              0x11
#define     CMOS_COM7               0x12
#define     CMOS_COM8               0x13
#define     CMOS_COM9               0x14
#define     CMOS_COM10              0x15
//  0x16 reserved
#define     CMOS_HSTART             0x17
#define     CMOS_HSTOP              0x18
#define     CMOS_VSTART             0x19
#define     CMOS_VSTOP              0x1A
#define     CMOS_PSHFT              0x1B
#define     CMOS_MIDH               0x1C        // READ ONLY
#define     CMOS_MIDL               0x1D        // READ ONLY
#define     CMOS_MVFP               0x1E
#define     CMOS_LAEC               0x1F
#define     CMOS_ADCCTR0            0x20
#define     CMOS_ADCCTR1            0x21
#define     CMOS_ADCCTR2            0x22
#define     CMOS_ADCCTR3            0x23
#define     CMOS_AEW                0x24
#define     CMOS_AEB                0x25
#define     CMOS_VPT                0x26
#define     CMOS_BBIAS              0x27
#define     CMOS_GbBIAS             0x28
//  0x29 reserved
#define     CMOS_EXHCH              0x2A
#define     CMOS_EXHCL              0x2B
#define     CMOS_BRIAS              0x2C
#define     CMOS_ADVFL              0x2D
#define     CMOS_ADVFH              0x2E
#define     CMOS_YAVE               0x2F
#define     CMOS_HSYST              0x30
#define     CMOS_HSYEN              0x31
#define     CMOS_HERF               0x32
#define     CMOS_CHLF               0x33
#define     CMOS_ARBLM              0x34
//  0x35 ~ 0x36 reserved
#define     CMOS_ADC                0x37
#define     CMOS_ACOM               0x38
#define     CMOS_OFON               0x39
#define     CMOS_TSLB               0x3A
#define     CMOS_COM11              0x3B
#define     CMOS_COM12              0x3C
#define     CMOS_COM13              0x3D
#define     CMOS_COM14              0x3E
#define     CMOS_EDGE               0x3F
#define     CMOS_COM15              0x40
#define     CMOS_COM16              0x41
#define     CMOS_COM17              0x42
#define     CMOS_AWBC1              0x43
#define     CMOS_AWBC2              0x44
#define     CMOS_AWBC3              0x45
#define     CMOS_AWBC4              0x46
#define     CMOS_AWBC5              0x47
#define     CMOS_AWBC6              0x48
//  0x49 ~ 0x4A reserved
#define     CMOS_REG4B              0x4B
#define     CMOS_DNSTH              0x4C
//  0x4D ~ 0x4E reserved
#define     CMOS_MTX1               0x4F
#define     CMOS_MTX2               0x50
#define     CMOS_MTX3               0x51
#define     CMOS_MTX4               0x52
#define     CMOS_MTX5               0x53
#define     CMOS_MTX6               0x54
#define     CMOS_BRIGHT             0x55
#define     CMOS_CONTRAS            0x56
#define     CMOS_CONTRAS_CENTER     0x57
#define     CMOS_MTXS               0x58
//  0x59 ~ 0x61 reserved
#define     CMOS_LCC1               0x62
#define     CMOS_LCC2               0x63
#define     CMOS_LCC3               0x64
#define     CMOS_LCC4               0x65
#define     CMOS_LCC5               0x66
#define     CMOS_MANU               0x67
#define     CMOS_MANV               0x68
#define     CMOS_GFIX               0x69
#define     CMOS_GGAIN              0x6A
#define     CMOS_DBLV               0x6B
#define     CMOS_AWBCTR3            0x6C
#define     CMOS_AWBCTR2            0x6D
#define     CMOS_AWBCTR1            0x6E
#define     CMOS_AWBCTR0            0x6F
#define     CMOS_SCALING_XSC        0x70
#define     CMOS_SCALING_YSC        0x71
#define     CMOS_SCALING_DCWCTR     0x72
#define     CMOS_SCALING_PCLK_DIV   0x73
#define     CMOS_REG74              0x74
#define     CMOS_REG75              0x75
#define     CMOS_REG76              0x76
#define     CMOS_REG77              0x77
//  0x78 ~ 0x79 reserved
#define     CMOS_SLOP               0x7A
#define     CMOS_GAM1               0x7B
#define     CMOS_GAM2               0x7C
#define     CMOS_GAM3               0x7D
#define     CMOS_GAM4               0x7E
#define     CMOS_GAM5               0x7F
#define     CMOS_GAM6               0x80
#define     CMOS_GAM7               0x81
#define     CMOS_GAM8               0x82
#define     CMOS_GAM9               0x83
#define     CMOS_GAM10              0x84
#define     CMOS_GAM11              0x85
#define     CMOS_GAM12              0x86
#define     CMOS_GAM13              0x87
#define     CMOS_GAM14              0x88
#define     CMOS_GAM15              0x89
//  0x8A ~ 0x8B reserved
#define     CMOS_RGB444             0x8C
//  0x8B ~ 0x91 reserved
#define     CMOS_DM_LNL             0x92
#define     CMOS_DM_LNH             0x93
#define     CMOS_LCC6               0x94
#define     CMOS_LCC7               0x95
//  0x96 ~ 0x9C reserved
#define     CMOS_BD50ST             0x9D
#define     CMOS_BD60ST             0x9E
#define     CMOS_HAECC1             0x9F
#define     CMOS_HAECC2             0xA0
//  0xA1 reserved
#define     CMOS_SCALING_PCLK_DELAY 0xA2
//  0xA3 reserved
#define     CMOS_NT_CTRL            0xA4
#define     CMOS_BD50MAX            0xA5
#define     CMOS_HAECC3             0xA6
#define     CMOS_HAECC4             0xA7
#define     CMOS_HAECC5             0xA8
#define     CMOS_HAECC6             0xA9
#define     CMOS_HAECC7             0xAA
#define     CMOS_BD60MAX            0xAB
#define     CMOS_STR_OPT            0xAC
#define     CMOS_STR_R              0xAD
#define     CMOS_STR_G              0xAE
#define     CMOS_STR_B              0xAF
//  0xB0 reserved
#define     CMOS_ABLC1              0xB1
//  0xB2 reserved
#define     CMOS_THL_ST             0xB3
//  0xB4 reserved
#define     CMOS_THL_DLT            0xB5
//  0xB6 ~ 0xBD reserved
#define     CMOS_AD_CHB             0xBE
#define     CMOS_AD_CHR             0xBF
#define     CMOS_AD_CHGb            0xC0
#define     CMOS_AD_CHGr            0xC1
//  0xC2 ~ 0xC8 reserved
#define     CMOS_SATCTR             0xC9

#endif  //  !CMOS_REGISTER_H
