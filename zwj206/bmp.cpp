#include "pch.h"
#include "bmp.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
// 本MFC使用的C++版本是 C++23preview，不是这个版本编译会报错（可能吧）

LPBITMAPINFO lpBitsInfo = nullptr;
BITMAPINFO* lpDIB_FT;
BITMAPINFO* lpDIB_IFT;
complex<double>* gFD = NULL;

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

void Equalize()
{
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	int i, j;
	BYTE* pixel;
	int temp;
	BYTE Map[256];
	Histogram();//全局的直方图

	for (i = 0; i < 256; i++)
	{
		temp = 0;
		for (j = 0; j <= i; j++)
		{
			temp += H[j];
		}
		Map[i] = (BYTE)(temp * 255 / (w * h) + 0.5);
	}
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			*pixel = Map[*pixel];
		}
	}
}

// 傅里叶变换
void FT(complex<double>* TD, complex<double>* FD, int m)
{
	int x, u;
	double angle;
	for (u = 0; u < m; u++)
	{
		FD[u] = 0;
		for (x = 0; x < m; x++)
		{
			angle = -2 * PI * u * x / m;
			FD[u] += TD[x] * complex<double>(cos(angle), sin(angle));
		}
		FD[u] /= m;
	}
}

// 反变换
void IFT(complex<double>* FD, complex<double>* TD, int m)
{
	int x, u;
	double angle;
	for (x = 0; x < m; x++)
	{
		TD[x] = 0;
		for (u = 0; u < m; u++)
		{
			angle = 2 * PI * u * x / m;
			TD[x] += FD[u] * complex<double>(cos(angle), sin(angle));
		}
	}
}

// 傅里叶变换显示
void Fourier() {
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	// 初始化
	complex<double>* TD = new complex<double>[w * h];
	complex<double>* FD = new complex<double>[w * h];

	int i, j;
	BYTE* pixel;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// 指向像素点(i,j)的指针
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			TD[w * i + j] = complex<double>(*pixel * pow(-1, i + j), 0);//TD的初始化操作
		}
	}
	// 一次傅里叶变换，沿行方向做傅里叶变换
	for (i = 0; i < h; i++) {
		FT(&TD[i * w], &FD[i * w], w);
	}

	// 将FD转置
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			TD[h * j + i] = FD[w * i + j];
		}
	}

	// 二次傅里叶变换
	for (i = 0; i < w; i++) {
		FT(&TD[i * h], &FD[i * h], h);
	}

	// 结果可视化
	LONG size = 40 + 1024 + LineBytes * h;
	lpDIB_FT = (BITMAPINFO*)malloc(size);
	if (NULL == lpDIB_FT)
		return;
	memcpy(lpDIB_FT, lpBitsInfo, size);

	lpBits = (BYTE*)&lpDIB_FT->bmiColors[lpDIB_FT->bmiHeader.biClrUsed];

	double temp;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// 指向像素点(i,j)的指针
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			temp = sqrt(FD[j * h + i].real() * FD[j * h + i].real() +
				FD[j * h + i].imag() * FD[j * h + i].imag()) * 2000;
			if (temp > 255)
				temp = 255;
			*pixel = (BYTE)(temp);
		}
	}

	delete TD;
	gFD = FD;
}

BOOL is_gFD_OK()
{
	return(gFD != NULL);
}

// 傅里叶反变换显示
void IFourier() {

	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];

	// 初始化
	complex<double>* TD = new complex<double>[w * h];
	complex<double>* FD = new complex<double>[w * h];

	// 转置回去
	int i, j;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			FD[w * i + j] = gFD[i + h * j];
		}
	}
	// 一次傅里叶反变换
	for (i = 0; i < h; i++) {
		IFT(&FD[i * w], &TD[i * w], w);
	}

	// 将TD转置
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			FD[h * j + i] = TD[w * i + j];
		}
	}

	// 二次傅里叶反变换
	for (i = 0; i < w; i++) {
		IFT(&FD[i * h], &TD[i * h], h);
	}

	// 计算结果可视化
	DWORD size = 40 + 1024 + LineBytes * h;
	lpDIB_IFT = (BITMAPINFO*)malloc(size);
	if (NULL == lpDIB_IFT)return;
	memcpy(lpDIB_IFT, lpBitsInfo, size);

	lpBits = (BYTE*)&lpDIB_IFT->bmiColors[lpDIB_IFT->bmiHeader.biClrUsed];

	BYTE* pixel;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			// 指向像素点(i,j)的指针
			pixel = lpBits + LineBytes * (h - 1 - i) + j;
			*pixel = (BYTE)(TD[j * h + i].real() / pow(-1, i + j));
		}
	}

	delete TD;
	delete FD;
	delete gFD;
	gFD = NULL;
}

BOOL FD_Available() {
	return (gFD != NULL);
}
