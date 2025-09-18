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
Errorstate LoadBmpFile(const char* BmpFileName, BITMAPINFOHEADER& infoheader, vector<BYTE>& pixeldata) {
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

	file.read(reinterpret_cast<char*>(&infoheader), sizeof(infoheader));

	if (infoheader.biBitCount != 24) {
		cerr << "Error：不是24位BMP文件！" << endl;
		file.close();
		return Errorstate::bit_error;
	}

	file.seekg(fileheader.bfOffBits, ios::beg);

	DWORD Imagesize = infoheader.biWidth * abs(infoheader.biHeight) * 3;

	pixeldata.resize(Imagesize);
	file.read(reinterpret_cast<char*>(pixeldata.data()), Imagesize);

	file.close();
	return Errorstate::success;
}