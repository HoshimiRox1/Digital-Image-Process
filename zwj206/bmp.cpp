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

// 