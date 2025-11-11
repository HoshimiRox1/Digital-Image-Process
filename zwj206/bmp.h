#pragma once

#include <Windows.h>
#include <vector>

extern LPBITMAPINFO lpBitsInfo;

BOOL LoadBmpFile(const char* BmpFileName);

void Gray();