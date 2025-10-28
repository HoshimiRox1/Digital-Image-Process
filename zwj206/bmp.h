#pragma once

#include <Windows.h>
#include <vector>

enum class Errorstate {
	file_error = 0,
	type_error,
	bit_error,
	success
};

extern LPBITMAPINFO lpBitsInfo;

BOOL LoadBmpFile(const char* BmpFileName);

//Errorstate LoadBmpFile(const char* BmpFileName,
//	BITMAPINFOHEADER& infoheader,
//	std::vector<RGBQUAD>& colorPalette,
//	std::vector<BYTE>& pixeldata);

void Gray();