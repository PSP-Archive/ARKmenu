#include "entry.h"

#include "eboot.h"
#include "iso.h"
#include "cso.h"

Entry::Entry(){
}

Entry::Entry(string path){
}

void Entry::animAppear(){
	for (int i=480; i>=0; i-=10){
		clearScreen(CLEAR_COLOR);
		blitAlphaImageToScreen(0, 0, 480-i, 272, this->pic1, i, 0);
		flipScreen();
	}
}

void Entry::animDisappear(){
	for (int i=0; i<=480; i+=10){
		clearScreen(CLEAR_COLOR);
		blitAlphaImageToScreen(0, 0, 480-i, 272, this->pic1, i, 0);
		flipScreen();
	}
}

string Entry::getName(){
	return this->name;
}

string Entry::getPath(){
	return this->path;
}

Image* Entry::getIcon(){
	return this->icon0;
}

void Entry::executeHomebrew(){
	struct SceKernelLoadExecVSHParam param;
	
	param.args = strlen(this->path.c_str()) + 1;
	param.argp = (char*)this->path.c_str();
	param.key = "game";
	sctrlKernelLoadExecVSHWithApitype(HOMEBREW_RUNLEVEL, this->path.c_str(), &param);
}

void Entry::executePSN(){
	struct SceKernelLoadExecVSHParam param;

	param.args = 33;  // lenght of "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN" + 1
	param.argp = (char*)"disc0:/PSP_GAME/SYSDIR/EBOOT.BIN";
	param.key = "umdemu";
	sctrlSESetBootConfFileIndex(PSN_DRIVER);
	sctrlSESetUmdFile("");
	sctrlKernelLoadExecVSHWithApitype(ISO_RUNLEVEL, this->path.c_str(), &param);
}

void Entry::executePOPS(){
	struct SceKernelLoadExecVSHParam param;
	
	param.args = strlen(this->path.c_str()) + 1;
	param.argp = (char*)this->path.c_str();
	param.key = "pops";
	sctrlKernelLoadExecVSHWithApitype(POPS_RUNLEVEL, this->path.c_str(), &param);
}

void Entry::executeEboot(){
	switch (Eboot::getEbootType(this->path.c_str())){
	case TYPE_HOMEBREW:	this->executeHomebrew();	break;
	case TYPE_PSN:		this->executePSN();			break;
	case TYPE_POPS:		this->executePOPS();		break;
	}
}

void Entry::executeISO(){

	struct SceKernelLoadExecVSHParam param;

	if (Iso::isPatched(this->path) || Cso::isPatched(this->path))
		param.argp = (char*)"disc0:/PSP_GAME/SYSDIR/EBOOT.OLD";
	else
		param.argp = (char*)"disc0:/PSP_GAME/SYSDIR/EBOOT.BIN";
	sctrlSEUmountUmd();

	param.key = "umdemu";
	param.args = 33;  // lenght of "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN" + 1
	sctrlSESetBootConfFileIndex(ISO_DRIVER);
	sctrlSESetUmdFile((char*)this->path.c_str());
	sctrlKernelLoadExecVSHWithApitype(ISO_RUNLEVEL, this->path.c_str(), &param);
}

bool Entry::run(){
	this->pic0 = getPic0();
	this->pic1 = getPic1();
	if (this->pic1 == NULL)
		this->pic1 = common::getBG();
	
	animAppear();
	blitAlphaImageToScreen(0, 0, 480, 272, this->pic1, 0, 0);
	blitAlphaImageToScreen(0, 0, this->icon0->imageWidth, this->icon0->imageHeight, this->icon0, 10, 98);
	if (this->pic0 != NULL)
		blitAlphaImageToScreen(0, 0, this->pic0->imageWidth, this->pic0->imageHeight, this->pic0, 160, 85);
	guStart();
	common::printText(0, 0, "You have selected:");
	common::printText(10, 10, this->name.c_str());
	common::printText(10, 20, this->path.c_str());
	common::printText(0, 40, "Press Cross to continue");
	common::printText(0, 50, "Press Circle to cancel");
	common::flip();
	
	Controller control;
	bool ret;
	
	while (true){
		control.update();
		if (control.cross()){
			ret = true;
			break;
		}
		else if (control.circle()){
			ret = false;
			break;
		}
	}
	animDisappear();
	delete this->pic0;
	delete this->pic1;
	return ret;
}

Entry::~Entry(){
}
