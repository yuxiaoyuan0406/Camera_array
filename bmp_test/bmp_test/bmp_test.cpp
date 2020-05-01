#include <cstdio>
#include <cstdint>
#include <string>
#include "libbmp.h"

#define WIDTH 320
#define HEIGHT 240

int main(int argc, const char* argv[])
{
	BmpImg img(WIDTH, HEIGHT);
	FILE* rgb = nullptr;

	if (argc < 2)
	{
		printf("need a input file\n");
		return -1;
	}

	std::string filename(argv[1]);

	rgb = fopen(filename.c_str(), "rb");
	if (rgb == nullptr)
	{
		printf("fail to poen file\nerrno = %d, message = %s\n", errno, strerror(errno));
		return errno;
	}
	fseek(rgb, 0, SEEK_END);
	int size = ftell(rgb);

	fclose(rgb);
	rgb = nullptr;

	printf("file size: %dB\n", size);
	if (size < WIDTH * HEIGHT * 2)
	{
		rgb = fopen(filename.c_str(), "ab");
		if (rgb == nullptr)
		{
			printf("fail to poen file\nerrno = %d, message = %s\n", errno, strerror(errno));
			return errno;
		}

		int offset = WIDTH * HEIGHT * 2 - size;
		uint8_t _0 = 0x00;
		fwrite(&_0, 1, offset, rgb);

		printf("resize done\n");

		fclose(rgb);
		rgb = nullptr;
	}

	rgb = fopen(filename.c_str(), "rb");
	if (rgb == nullptr)
	{
		printf("fail to poen file\nerrno = %d, message = %s\n", errno, strerror(errno));
		return errno;
	}

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			uint16_t buff = 0;
			fread((uint8_t*)(&buff) + 1, 1, 1, rgb);	// 从文件中先读到的1字节应置于buff的高8位
			fread((uint8_t*)(&buff) + 0, 1, 1, rgb);	// 从文件中后读到的1字节应置于buff的低8位
			//fread((uint8_t*)(&buff), 1, 2, rgb);
			uint8_t R, G, B;
			R = (uint8_t)((buff >> 8) & 0x00F8);
			G = (uint8_t)((buff >> 3) & 0x00FC);
			B = (uint8_t)((buff << 3) & 0x00F8);
			//R = (uint8_t)((buff & 0xF800) >> 8);
			//G = (uint8_t)((buff & 0x07E0) >> 3);
			//B = (uint8_t)((buff & 0x001F) << 3);
			//printf("x: %d, y: %d, %d %d %d\n", x, y, R, G, B);
			img.set_pixel(x, y, R, G, B);
		}
	}

	img.write(filename + ".bmp");

	printf("bmp generated\n");

	fclose(rgb);
	return 0;
}