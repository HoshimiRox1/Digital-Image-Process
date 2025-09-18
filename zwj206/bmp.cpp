#include "pch.h"
#include "bmp.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;
// ��MFCʹ�õ�C++�汾�� C++23preview����������汾����ᱨ��
// 
// ʵ����ͼ��ļ�������ʾ����ʾ��ֱ��copy�ģ���Ҫ����ѧϰ�ٸ��֣�
// ������ʾ��ɾ��������ע��
Errorstate LoadBmpFile(const char* BmpFileName, BITMAPINFOHEADER& infoheader, vector<BYTE>& pixeldata) {
	fstream file(BmpFileName, ios::in | ios::binary);
	if (!file.is_open()) {
		cerr << "Error���ļ���ʧ�ܣ�" << endl;
		return Errorstate::file_error;
	}

	BITMAPFILEHEADER fileheader;
	file.read(reinterpret_cast<char*>(&fileheader), sizeof(fileheader));

	if (fileheader.bfType != 0x4D42) {	// 0x42 = B�� 0x4D = M
		cerr << "Error���ļ�����BMP���ͣ�" << endl;
		file.close();
		return Errorstate::type_error;
	}

	file.read(reinterpret_cast<char*>(&infoheader), sizeof(infoheader));

	if (infoheader.biBitCount != 24) {
		cerr << "Error������24λBMP�ļ���" << endl;
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