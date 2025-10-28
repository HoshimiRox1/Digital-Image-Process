#include "pch.h"
#include "bmp.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
// 本MFC使用的C++版本是 C++23preview，不是这个版本编译会报错

LPBITMAPINFO lpBitsInfo = nullptr;

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
		case 2:
			NumColors = 4;
		case 8:
			NumColors = 256;
		case 24:
			NumColors = 0;
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
	//fstream file(BmpFileName, ios::in | ios::binary);
	//if (!file.is_open()) {
	//	cerr << "Error：文件打开失败！" << endl;
	//	return Errorstate::file_error;
	//}

	//BITMAPFILEHEADER fileheader;
	//file.read(reinterpret_cast<char*>(&fileheader), sizeof(fileheader));

	//if (fileheader.bfType != 0x4D42) {	// 0x42 = B， 0x4D = M
	//	cerr << "Error：文件不是BMP类型！" << endl;
	//	file.close();
	//	return Errorstate::type_error;
	//}

	//// 读取信息头
	//file.read(reinterpret_cast<char*>(&infoheader), sizeof(infoheader));
	//
	//// 读取调色板（仅在非24位真彩下）
	//DWORD paletteSize = 0;
	//if (infoheader.biClrUsed == 0 && infoheader.biBitCount <= 8) {
	//	paletteSize = 1 << infoheader.biBitCount;
	//}
	//else if (infoheader.biClrUsed > 0) {
	//	paletteSize = infoheader.biClrUsed;
	//}

	//if (paletteSize > 0) {
	//	colorPalette.resize(paletteSize);
	//	file.read(reinterpret_cast<char*>(colorPalette.data()), paletteSize * sizeof(RGBQUAD));
	//}

	//file.seekg(fileheader.bfOffBits, ios::beg);

	//// 根据位数动态调整像素矩阵大小
	//int bitRow = infoheader.biWidth * infoheader.biBitCount;

	//int ByteRow = (bitRow + 7) / 8;

	//int pitch = (ByteRow + 3) & (~3);
	//
	//DWORD Imagesize = abs(infoheader.biHeight) * pitch;

	//pixeldata.resize(Imagesize);
	//file.read(reinterpret_cast<char*>(pixeldata.data()), Imagesize);

	//file.close();
	//return Errorstate::success;
}

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