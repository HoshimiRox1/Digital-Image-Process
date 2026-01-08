#include "pch.h"
#include "bmp.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
// 本MFC使用的C++版本是 C++23preview，不是这个版本编译会报错

LPBITMAPINFO lpBitsInfo = nullptr;

// 加载图像文件
BOOL LoadBmpFile(const char* BmpFileName) 
{
	FILE* fp;
	if (NULL == (fp = fopen(BmpFileName, "rb")))
		return FALSE;

	BITMAPFILEHEADER bf;
	fread(&bf, 14, 1, fp);

	BITMAPINFOHEADER bi;
	fread(&bi, 40, 1, fp);

	DWORD NumColors;
	if (bi.biClrUsed != 0)
		NumColors = bi.biClrUsed;
	else {
		switch (bi.biBitCount)
		{
		case 1:
			NumColors = 2;
			break;
		case 2:
			NumColors = 4;
			break;
		case 4:
			NumColors = 16;
			break;
		case 8:
			NumColors = 256;
			break;
		case 24:
			NumColors = 0;
			break;
		}
	}
	
	DWORD LineBytes = (bi.biWidth * bi.biBitCount + 31) / 32 * 4;
	DWORD ImgSize = LineBytes * bi.biHeight;
	DWORD size = 40 + NumColors * 4 + ImgSize;
	lpBitsInfo = (LPBITMAPINFO)malloc(size);

	fseek(fp, 14, SEEK_SET);
	fread((char*)lpBitsInfo, size, 1, fp);
	lpBitsInfo->bmiHeader.biClrUsed = NumColors;

	fclose(fp);

	return TRUE;
}

// 24位真彩转灰度图像
void Gray()
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	int i, j;
	BYTE* R, * G, * B, avg;

	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			B = lpBits + LineBytes * (h - 1 - i) + j * 3;
			G = B + 1;
			R = G + 1;
			avg = (*B + *G + *R) / 3;
			*R = *G = *B = avg;
		}
	}
}

// 检测是否为灰度图像
bool IsGray() {
	int r, g, b;
	if (8 == lpBitsInfo->bmiHeader.biBitCount) {
		r = lpBitsInfo->bmiColors[128].rgbRed;
		g = lpBitsInfo->bmiColors[128].rgbGreen;
		b = lpBitsInfo->bmiColors[128].rgbBlue;

		if (r == b && r == g)
			return true;
	}
	return false;
}

// 读取像素数据
void pixel(int i, int j, char* str) {
	if (nullptr == lpBitsInfo)
		return;

	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	if (i >= h || j >= w)
		return;

	BYTE* pixel, bv, r, g, b;

	switch (lpBitsInfo->bmiHeader.biBitCount) {
	case 24:
		pixel = lpBits + LineBytes * (h - 1 - i) + j * 3;
		b = *pixel;
		g = *(pixel + 1);
		r = *(pixel + 2);
		sprintf(str, "RGB(%d,%d,%d)", r, g, b);
		break;
	case 8:
		pixel = lpBits + LineBytes * (h - 1 - i) + j;
		
		if(IsGray())
			sprintf(str, "灰度值：%d", *pixel);
		else {
			r = lpBitsInfo->bmiColors[*pixel].rgbRed;
			g = lpBitsInfo->bmiColors[*pixel].rgbGreen;
			b = lpBitsInfo->bmiColors[*pixel].rgbBlue;
			sprintf(str, "RGB(%d,%d,%d)", r, g, b);
		}
		break;
	case 4:
		pixel = lpBits + LineBytes * (h - 1 - i) + j / 2;

		BYTE index;
		if (j % 2 == 0) {
			index = (*pixel & 0xF0) >> 4;
		}
		else {
			index = *pixel & 0x0F;
		}

		r = lpBitsInfo->bmiColors[index].rgbRed;
		g = lpBitsInfo->bmiColors[index].rgbGreen;
		b = lpBitsInfo->bmiColors[index].rgbBlue;
		sprintf(str, "RGB(%d,%d,%d)", r, g, b);
		break;
	case 1:
		bv = *(lpBits + LineBytes * (h - 1 - i) + j / 8) & (1 << (7 - j % 8));
		if (0 == bv)
			strcpy(str, "背景点");
		else
			strcpy(str, "前景点");
		break;
	}
}

// 直方图数组
DWORD H[256];
void Histogram() {
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	int i, j;
	BYTE* pixel;

	for (i = 0; i < 256; i++) {
		H[i] = 0;
	}

	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			H[*pixel]++;
		}
	}
}

// 线性点运算
void LineTrans(float a, float b)
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	float temp;
	BYTE* pixel;
	int i, j;
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			temp = a * (*pixel) + b;
			if (temp < 0)
				*pixel = 0;
			else if (temp > 255)
				*pixel = 255;
			else
				*pixel = (BYTE)(temp + 0.5);
		}
	}
}

