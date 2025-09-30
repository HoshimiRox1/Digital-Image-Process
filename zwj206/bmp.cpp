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
Errorstate LoadBmpFile(const char* BmpFileName,
	BITMAPINFOHEADER& infoheader,
	vector<RGBQUAD>& colorPalette,
	vector<BYTE>& pixeldata) 
{
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

	// ��ȡ��Ϣͷ
	file.read(reinterpret_cast<char*>(&infoheader), sizeof(infoheader));

	if (infoheader.biBitCount == 24) {
		AfxMessageBox(_T("24λͼ��"), MB_OK | MB_ICONINFORMATION);
	}

	if (infoheader.biBitCount == 1) {
		AfxMessageBox(_T("2ֵͼ��"), MB_OK | MB_ICONINFORMATION);
	}

	// ��ȡ��ɫ�壨���ڷ�24λ����£�
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

	if (infoheader.biBitCount != 24 && infoheader.biBitCount != 8 && infoheader.biBitCount != 1) {
		// ��������������Ҫ����ͨ�������
		return LOAD_FAIL_UNSUPPORTED_DEPTH;
	}

	file.seekg(fileheader.bfOffBits, ios::beg);

	DWORD Imagesize = infoheader.biWidth * abs(infoheader.biHeight) * 3;

	pixeldata.resize(Imagesize);
	file.read(reinterpret_cast<char*>(pixeldata.data()), Imagesize);

	file.close();
	return Errorstate::success;
}