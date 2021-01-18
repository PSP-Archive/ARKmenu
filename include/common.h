#ifndef COMMON_H
#define COMMON_H

#include "debug.h"
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <malloc.h>
#include "gfx.h"

#define THREAD_DELAY 1000

// Colors
enum colors {
	RED =	0xFF0000FF,
	GREEN =	0xFF00FF00,
	BLUE =	0xFFFF0000,
	WHITE =	0xFFFFFFFF,
	LITEGRAY = 0xFFBFBFBF,
	GRAY =  0xFF7F7F7F,
	DARKGRAY = 0xFF3F3F3F,		
	BLACK = 0xFF000000,
};

namespace common{

	static Image* background;
	static Image* noicon;
	static intraFont* font;

	extern bool has_suffix(const std::string &str, const std::string &suffix);
	extern u32 getMagic(const char* filename, unsigned int offset);
	extern void loadData();
	extern void deleteData();
	extern bool fileExists(const std::string &path);
	extern Image* getBG();
	extern Image* getNoIcon();
	extern intraFont* getFont();
	extern void printText(float x, float y, const char *text);
	extern void flip();

}

#endif
