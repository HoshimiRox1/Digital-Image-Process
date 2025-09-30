#include "pch.h"
#include "bmp.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
// 本MFC使用的C++版本是 C++23preview，不是这个版本编译会报错
// 
// 实现了图像的加载与显示（显示是直接copy的，需要重新学习再复现）
// 复现显示后删除这两条注释
Errorstate LoadBmpFile(const char* BmpFileName,
	BITMAPINFOHEADER& infoheader,
	vector<RGBQUAD>& colorPalette,
	vector<BYTE>& pixeldata) 
{
	fstream file(BmpFileName, ios::in | ios::binary);
	if (!file.is_open()) {
		cerr << "Error：文件打开失败！" << endl;
		return Errorstate::file_error;
	}

	BITMAPFILEHEADER fileheader;
	file.read(reinterpret_cast<char*>(&fileheader), sizeof(fileheader));

	if (fileheader.bfType != 0x4D42) {	// 0x42 = B， 0x4D = M
		cerr << "Error：文件不是BMP类型！" << endl;
		file.close();
		return Errorstate::type_error;
	}

	// 读取信息头
	file.read(reinterpret_cast<char*>(&infoheader), sizeof(infoheader));

	/*if (infoheader.biBitCount == 24) {
		AfxMessageBox(_T("24位图像"), MB_OK | MB_ICONINFORMATION);
	}

	if (infoheader.biBitCount == 1) {
		AfxMessageBox(_T("2值图像"), MB_OK | MB_ICONINFORMATION);
	}*/

	// 读取调色板（仅在非24位真彩下）
	DWORD paletteSize = 0;
	if (infoheader.biClrUsed == 0 && infoheader.biBitCount <= 8) {
		paletteSize = 1 << infoheader.biBitCount;
	}
	else if (infoheader.biClrUsed > 0) {
		paletteSize = infoheader.biClrUsed;
	}

	if (paletteSize > 0) {
		colorPalette.resize(paletteSize);
		file.read(reinterpret_cast<char*>(colorPalette.data()), paletteSize * sizeof(RGBQUAD));
	}

	file.seekg(fileheader.bfOffBits, ios::beg);

	// 根据位数动态调整像素矩阵大小
	int bitRow = infoheader.biWidth * infoheader.biBitCount;

	int ByteRow = (bitRow + 7) / 8;

	int pitch = (ByteRow + 3) & (~3);
	
	DWORD Imagesize = abs(infoheader.biHeight) * pitch;

	pixeldata.resize(Imagesize);
	file.read(reinterpret_cast<char*>(pixeldata.data()), Imagesize);

	file.close();
	return Errorstate::success;
}