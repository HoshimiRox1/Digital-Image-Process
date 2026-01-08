#pragma once

#include <Windows.h>
#include <vector>

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

// 计算256灰度直方图
void ComputeHistogram(int hist[256]);

// 绘制直方图
void DrawHistogram(HDC hdc, RECT rc);