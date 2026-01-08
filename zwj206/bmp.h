#pragma once

#include <Windows.h>
#include <vector>
#include <complex>

#define PI 3.1415926535

using namespace std;

extern LPBITMAPINFO lpBitsInfo;

// 加载bmp图像
BOOL LoadBmpFile(const char* BmpFileName);

// 24位真彩转灰度
void Gray();

// 检测是否为灰度图像
bool IsGray();

// 读取像素数据
void pixel(int i, int j, char* str);

// 直方图数组
void Histogram();

// 线性点运算
void LineTrans(float a,float b);

// 图像均衡变换
void Equalize();

// 傅里叶变换
void FT(complex<double>* TD, complex<double>* FD, int m);

// 反变换
void IFT(complex<double>* FD, complex<double>* TD, int m);

// 傅里叶变换显示
void Fourier();
BOOL is_gFD_OK();

// 傅里叶反变换显示
void IFourier();
BOOL FD_Available();

// 快速傅里叶变换
void FFT(complex<double>* TD, complex<double>* FD, int r);
void FFourier();

// 快速反变换
void IFFT(complex<double>* FD, complex<double>* TD, int r);
void IFFourier();

// 模板函数
void Template(int* Array, float coef);

// 均值滤波
void AvgSmooth();

// 获取中值
BYTE WINAPI GetMedianNum(BYTE* Array);

// 中值滤波
void MidSmooth();

// 拉普拉斯锐化
void RaplasSharp();

// 梯度锐化
void GradSharp();

// 理想滤波：D>0低通，D<0高通
void FFT_Filter(int D);