#include "common.h"

using namespace common;

bool common::has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

u32 common::getMagic(const char* filename, unsigned int offset){
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)
		return 0;
	u32 magic;
	fseek(fp, offset, SEEK_SET);
	fread(&magic, 4, 1, fp);
	fclose(fp);
	return magic;
}

void common::loadData(){
	background = loadImage("defbg.png", 0);
	noicon = loadImage("noicon.png", 0);
	font = intraFontLoad("FONT.PGF", 0);
}

void common::deleteData(){
	freeImage(background);
	freeImage(noicon);
	intraFontUnload(font);
}

bool common::fileExists(const std::string &path){
	FILE* fp = fopen(path.c_str(), "rb");
	if (fp == NULL)
		return false;
	fclose(fp);
	return true;
}

Image* common::getBG(){
	return background;
}

Image* common::getNoIcon(){
	return noicon;
}

intraFont* common::getFont(){
	return font;
}


void common::printText(float x, float y, const char *text){
	intraFontSetStyle(font, 0.51f, GRAY_COLOR, BLACK_COLOR, 0);
	intraFontPrint(font, x, y, text);
}

void common::flip(){
	sceGuFinish();
	sceGuSync(0,0);
	guStart();
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuFinish();
	sceGuSync(0,0); 

	sceDisplayWaitVblankStart(); 
	flipScreen();
	sceKernelDcacheWritebackInvalidateAll();
	sceKernelDelayThread(THREAD_DELAY);
};
