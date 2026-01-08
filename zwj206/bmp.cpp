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

// 快速傅里叶变换
void FFT(complex<double>* TD, complex<double>* FD, int r)
{
	// 计算傅里叶变换点数
	LONG count = 1 << r;
	// 计算加权系数
	int i;
	double angle;
	complex<double>* W = new complex<double>[count / 2];
	for (i = 0; i < count / 2; i++)
	{
		angle = -i * PI * 2 / count;
		W[i] = complex<double>(cos(angle), sin(angle));
	}
	// 将时域点写入X1
	complex<double>* X1 = new complex<double>[count];
	memcpy(X1, TD, sizeof(complex<double>) * count);

	// 采用蝶形算法进行快速付立叶变换，输出为频域值X2
	complex<double>* X2 = new complex<double>[count];

	int k, j, p, size;
	complex<double>* temp;
	for (k = 0; k < r; k++)
	{
		for (j = 0; j < 1 << k; j++)
		{
			size = 1 << (r - k);
			for (i = 0; i < size / 2; i++)
			{
				p = j * size;
				X2[i + p] = X1[i + p] + X1[i + p + size / 2];
				X2[i + p + size / 2] = (X1[i + p] - X1[i + p + size / 2]) * W[i * (1 << k)];
			}
		}
		temp = X1;
		X1 = X2;
		X2 = temp;
	}

	// 重新排序（码位倒序排列）
	for (j = 0; j < count; j++)
	{
		p = 0;
		for (i = 0; i < r; i++)
		{
			if (j & (1 << i))
			{
				p += 1 << (r - i - 1);
			}
		}
		FD[j] = X1[p];
		FD[j] /= count;
	}

	// 释放内存
	delete W;
	delete X1;
	delete X2;
}

void FFourier()
{
	//图像的宽度和高度
	int width = lpBitsInfo->bmiHeader.biWidth;
	int height = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (width * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	//指向图像数据指针
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[256];

	// FFT宽度（必须为2的整数次方）
	int FFT_w = 1;
	// FFT宽度的幂数，即迭代次数
	int wp = 0;
	while (FFT_w * 2 <= width)
	{
		FFT_w *= 2;
		wp++;
	}

	// FFT高度（必须为2的整数次方）
	int FFT_h = 1;
	// FFT高度的幂数，即迭代次数
	int hp = 0;
	while (FFT_h * 2 <= height)
	{
		FFT_h *= 2;
		hp++;
	}

	// 分配内存
	complex<double>* TD = new complex<double>[FFT_w * FFT_h];
	complex<double>* FD = new complex<double>[FFT_w * FFT_h];

	int i, j;
	BYTE* pixel;

	for (i = 0; i < FFT_h; i++)  // 行
	{
		for (j = 0; j < FFT_w; j++)  // 列
		{
			// 指向DIB第i行，第j个象素的指针
			pixel = lpBits + LineBytes * (height - 1 - i) + j;

			// 给时域赋值
			TD[j + FFT_w * i] = complex<double>(*pixel * pow(-1, i + j), 0);
		}
	}

	for (i = 0; i < FFT_h; i++)
	{
		// 对y方向进行快速付立叶变换
		FFT(&TD[FFT_w * i], &FD[FFT_w * i], wp);
	}

	// 保存中间变换结果
	for (i = 0; i < FFT_h; i++)
	{
		for (j = 0; j < FFT_w; j++)
		{
			TD[i + FFT_h * j] = FD[j + FFT_w * i];
		}
	}

	for (i = 0; i < FFT_w; i++)
	{
		// 对x方向进行快速付立叶变换
		FFT(&TD[i * FFT_h], &FD[i * FFT_h], hp);
	}

	// 删除临时变量
	delete TD;

	//生成频谱图像
	//为频域图像分配内存
	LONG size = 40 + 1024 + LineBytes * height;
	lpDIB_FT = (LPBITMAPINFO)malloc(size);
	if (NULL == lpDIB_FT)
		return;
	memcpy(lpDIB_FT, lpBitsInfo, size);

	//指向频域图像数据指针
	lpBits = (BYTE*)&lpDIB_FT->bmiColors[256];

	double temp;
	for (i = 0; i < FFT_h; i++) // 行
	{
		for (j = 0; j < FFT_w; j++) // 列
		{
			// 计算频谱幅度
			temp = sqrt(FD[j * FFT_h + i].real() * FD[j * FFT_h + i].real() +
				FD[j * FFT_h + i].imag() * FD[j * FFT_h + i].imag()) * 2000;

			// 判断是否超过255
			if (temp > 255)
			{
				// 对于超过的，直接设置为255
				temp = 255;
			}

			pixel = lpBits + LineBytes * (height - 1 - i) + j;

			// 更新源图像
			*pixel = (BYTE)(temp);
		}
	}

	gFD = FD;

}

//IFFT反变换
void IFFT(complex<double>* FD, complex<double>* TD, int r)
{
	// 傅里叶变换点数
	LONG count;
	// 计算傅里叶变换点数
	count = 1 << r;

	// 分配运算所需存储器
	complex<double>* X = new complex<double>[count];
	// 将频域点写入X
	memcpy(X, FD, sizeof(complex<double>) * count);

	// 求共轭
	for (int i = 0; i < count; i++)
		X[i] = complex<double>(X[i].real(), -X[i].imag());

	// 调用快速傅里叶变换
	FFT(X, TD, r);

	// 求时域点的共轭
	for (int i = 0; i < count; i++)
		TD[i] = complex<double>(TD[i].real() * count, -TD[i].imag() * count);

	// 释放内存
	delete X;
}


void IFFourier()
{
	//图像的宽度和高度
	int width = lpBitsInfo->bmiHeader.biWidth;
	int height = lpBitsInfo->bmiHeader.biHeight;
	int LineBytes = (width * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;

	// FFT宽度（必须为2的整数次方）
	int FFT_w = 1;
	// FFT宽度的幂数，即迭代次数
	int wp = 0;
	while (FFT_w * 2 <= width)
	{
		FFT_w *= 2;
		wp++;
	}

	// FFT高度（必须为2的整数次方）
	int FFT_h = 1;
	// FFT高度的幂数，即迭代次数
	int hp = 0;
	while (FFT_h * 2 <= height)
	{
		FFT_h *= 2;
		hp++;
	}

	// 分配内存
	complex<double>* TD = new complex<double>[FFT_w * FFT_h];
	complex<double>* FD = new complex<double>[FFT_w * FFT_h];

	int i, j;
	for (i = 0; i < FFT_h; i++)  // 行
		for (j = 0; j < FFT_w; j++)  // 列
			FD[j + FFT_w * i] = gFD[i + FFT_h * j];

	// 沿水平方向进行快速付立叶变换
	for (i = 0; i < FFT_h; i++)
		IFFT(&FD[FFT_w * i], &TD[FFT_w * i], wp);

	// 保存中间变换结果
	for (i = 0; i < FFT_h; i++)
		for (j = 0; j < FFT_w; j++)
			FD[i + FFT_h * j] = TD[j + FFT_w * i];

	// 沿垂直方向进行快速付立叶变换
	for (i = 0; i < FFT_w; i++)
		IFFT(&FD[i * FFT_h], &TD[i * FFT_h], hp);

	//为反变换图像分配内存
	LONG size = 40 + 1024 + LineBytes * height;

	lpDIB_IFT = (LPBITMAPINFO)malloc(size);
	if (NULL == lpDIB_IFT)
		return;
	memcpy(lpDIB_IFT, lpBitsInfo, size);

	//指向反变换图像数据指针
	BYTE* lpBits = (BYTE*)&lpDIB_IFT->bmiColors[256];
	BYTE* pixel;
	double temp;
	for (i = 0; i < FFT_h; i++) // 行
	{
		for (j = 0; j < FFT_w; j++) // 列
		{
			pixel = lpBits + LineBytes * (height - 1 - i) + j;
			temp = (TD[j * FFT_h + i].real() / pow(-1, i + j));
			if (temp < 0)
				temp = 0;
			else if (temp > 255)
				temp = 255;
			*pixel = (BYTE)temp;
		}
	}

	// 删除临时变量
	delete FD;
	delete TD;
	delete gFD;
	gFD = NULL;
}

// 模板函数
void Template(int* Array, float coef)
{
	// 图像的宽度和高度
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	// 每行的字节数（必须是4的倍数）
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	// 指向原图像数据的指针
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	// 为新图象分配内存
	BITMAPINFO* new_lpBitsInfo;
	LONG size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + h * LineBytes;
	if (NULL == (new_lpBitsInfo = (LPBITMAPINFO)malloc(size)))
		return;
	// 复制BMP
	memcpy(new_lpBitsInfo, lpBitsInfo, size);
	// 找到新图像象素起始位置
	BYTE* lpNewBits = (BYTE*)&new_lpBitsInfo->bmiColors[new_lpBitsInfo->bmiHeader.biClrUsed];

	int i, j, k, l;
	BYTE* pixel, * new_pixel;
	float result;

	// 行(除去边缘几行)
	for (i = 1; i < h - 1; i++)
	{
		// 列(除去边缘几列)
		for (j = 1; j < w - 1; j++)
		{
			// 指向新图第i行，第j个象素的指针
			new_pixel = lpNewBits + LineBytes * (h - 1 - i) + j;
			result = 0;
			// 计算3x3模板内像素灰度值的和
			for (k = 0; k < 3; k++)
			{
				for (l = 0; l < 3; l++)
				{
					// 指向原图在模板内每个像素点的灰度值，第i - 1 + k行，第j - 1 + l个象素的指针
					pixel = lpBits + LineBytes * (h - i - k) + j - 1 + l;
					// 灰度值乘上模板系数后累加
					result += (*pixel) * Array[k * 3 + l];
				}
			}
			// 乘上系数
			result *= coef;
			if (result < 0)
				*new_pixel = 0;
			else if (result > 255)
				*new_pixel = 255;
			else
				*new_pixel = (BYTE)(result + 0.5);
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = new_lpBitsInfo;
}

// 均值滤波
void AvgSmooth() {
	int Array[9]; //3x3模板
	//标准均值滤波
	Array[0] = 1;	Array[1] = 1;	Array[2] = 1;
	Array[3] = 1;	Array[4] = 1;	Array[5] = 1;
	Array[6] = 1;	Array[7] = 1;	Array[8] = 1;
	Template(Array, (float)1 / 9);
}

// 获取中值
BYTE WINAPI GetMedianNum(BYTE* Array)
{
	int i, j;
	BYTE temp;
	// 用冒泡法对数组进行排序
	for (j = 0; j < 9 - 1; j++)
	{
		for (i = 0; i < 9 - j - 1; i++)
		{
			if (Array[i] > Array[i + 1])
			{
				temp = Array[i];
				Array[i] = Array[i + 1];
				Array[i + 1] = temp;
			}
		}
	}
	// 返回中值
	return Array[4];
}

// 中值滤波
void MidSmooth()
{
	// 图像的宽度和高度
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	// 每行的字节数（必须是4的倍数）
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	// 指向原图像数据的指针
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	// 为新图象分配内存
	BITMAPINFO* new_lpBitsInfo;
	LONG size = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD) + h * LineBytes;
	if (NULL == (new_lpBitsInfo = (LPBITMAPINFO)malloc(size)))
		return;
	// 复制BMP
	memcpy(new_lpBitsInfo, lpBitsInfo, size);
	// 找到新图像象素起始位置
	BYTE* lpNewBits = (BYTE*)&new_lpBitsInfo->bmiColors[new_lpBitsInfo->bmiHeader.biClrUsed];

	int i, j, k, l;
	BYTE* pixel, * new_pixel;
	BYTE Value[9]; //3x3模板
	// 行(除去边缘几行)
	for (i = 1; i < h - 1; i++)
	{
		// 列(除去边缘几列)
		for (j = 1; j < w - 1; j++)
		{
			// 指向新图第i行，第j个象素的指针
			new_pixel = lpNewBits + LineBytes * (h - 1 - i) + j;
			// 计算 3x3模板内像素的灰度值
			for (k = 0; k < 3; k++)
			{
				for (l = 0; l < 3; l++)
				{
					// 指向原图第i - 1 + k行，第j - 1 + l个象素的指针
					pixel = lpBits + LineBytes * (h - i - k) + j - 1 + l;
					// 保存象素值
					Value[k * 3 + l] = *pixel;
				}
			}
			// 获取中值
			*new_pixel = GetMedianNum(Value);
		}
	}
	free(lpBitsInfo);
	lpBitsInfo = new_lpBitsInfo;
}

// 拉普拉斯锐化
void RaplasSharp()
{
	int Array[9]; 

	//拉普拉斯锐化
	Array[0] = -1;	Array[1] = -1;	Array[2] = -1;
	Array[3] = -1;	Array[4] = 9;	Array[5] = -1;
	Array[6] = -1;	Array[7] = -1;	Array[8] = -1;
	Template(Array, (float)1);
}

// 梯度锐化
void GradSharp()
{
	// 图像的宽度和高度
	int w = lpBitsInfo->bmiHeader.biWidth;
	int h = lpBitsInfo->bmiHeader.biHeight;
	// 每行的字节数（必须是4的倍数）
	int LineBytes = (w * lpBitsInfo->bmiHeader.biBitCount + 31) / 32 * 4;
	// 指向原图像数据的指针
	BYTE* lpBits = (BYTE*)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	// 指向源图像的指针
	BYTE* lpSrc, * lpSrc1, * lpSrc2;
	int i, j;
	BYTE temp;
	// 每行
	for (i = 0; i < h - 1; i++)
	{
		// 每列
		for (j = 0; j < w - 1; j++)
		{
			// 指向图像第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpBits + LineBytes * (h - 1 - i) + j;
			// 指向图像第i+1行，第j个象素的指针
			lpSrc1 = (unsigned char*)lpBits + LineBytes * (h - 2 - i) + j;
			// 指向图像第i行，第j+1个象素的指针
			lpSrc2 = (unsigned char*)lpBits + LineBytes * (h - 1 - i) + j + 1;
			//梯度算子
			temp = abs((*lpSrc) - (*lpSrc1)) + abs((*lpSrc) - (*lpSrc2));
			if (temp > 255)
				*lpSrc = 255;
			else
				*lpSrc = temp;
		}
	}
}