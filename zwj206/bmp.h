#pragma once

#include <Windows.h>
#include <vector>

enum class Errorstate {
	file_error = 0,
	type_error,
	bit_error,
	success
};

Errorstate LoadBmpFile(const char* BmpFileName, BITMAPINFOHEADER& infoheader, std::vector<BYTE>& pixeldata);