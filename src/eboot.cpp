#include "eboot.h"

Eboot::Eboot(string path){
	
	size_t lastSlash = path.rfind("/", string::npos);
	size_t substrPos = path.rfind("/", lastSlash-1)+1;

	this->path = path;
	this->ebootName = path.substr(lastSlash+1, string::npos);
	this->name = path.substr(substrPos, lastSlash-substrPos);
	readHeader();
	this->icon0 = loadIcon();
}

Eboot::~Eboot(){
	delete this->icon0;
}

void Eboot::readHeader(){
	void* data = malloc(sizeof(PBPHeader));
	
	FILE* fp = fopen(this->path.c_str(), "rb");
	fread(data, 1, sizeof(PBPHeader), fp);
	fclose(fp);
	
	this->header = (PBPHeader*)data;
}

Image* Eboot::loadIcon(){
	Image* icon = loadImage(this->path.c_str(), this->header->icon0_offset);
	return (icon == NULL)? common::getNoIcon() : icon;
}
		
string Eboot::getEbootName(){
	return this->ebootName;
}

Image* Eboot::getIcon(){
	return this->icon0;
}

Image* Eboot::getPic0(){
	return loadImage(this->path.c_str(), this->header->pic0_offset);
}

Image* Eboot::getPic1(){
	return loadImage(this->path.c_str(), this->header->pic1_offset);
}

int Eboot::getEbootType(const char* path){

	int ret = UNKNOWN_TYPE;

	FILE* fp = fopen(path, "rb");
	if (fp == NULL)
		return ret;
	
	fseek(fp, 48, SEEK_SET);
	
	u32 labelstart;
	u32 valuestart;
	u32 valueoffset;
	u32 entries;
	u16 labelnameoffset;
	char* labelname = (char*)malloc(9);
	u16 categoryType;
	int cur;

	fread(&labelstart, 4, 1, fp);
	fread(&valuestart, 4, 1, fp);
	fread(&entries, 4, 1, fp);
	while (entries>0 && ret == UNKNOWN_TYPE){
	
		entries--;
		cur = ftell(fp);
		fread(&labelnameoffset, 2, 1, fp);
		fseek(fp, labelnameoffset + labelstart + 40, SEEK_SET);
		fread(labelname, 8, 1, fp);

		if (!strncmp(labelname, "CATEGORY", 8)){
			fseek(fp, cur+12, SEEK_SET);
			fread(&valueoffset, 1, 4, fp);
			fseek(fp, valueoffset + valuestart + 40, SEEK_SET);
			fread(&categoryType, 2, 1, fp);
			switch(categoryType){
			case HMB_CAT:		ret = TYPE_HOMEBREW;	break;
			case PSN_CAT:		ret = TYPE_PSN;			break;
			case PS1_CAT:		ret = TYPE_POPS;		break;
			default:									break;
			}
		}
		fseek(fp, cur+16, SEEK_SET);
	}
	fclose(fp);
	return ret;
}

string Eboot::fullEbootPath(string app){
	// Return the full path of a homebrew given only the homebrew name
	if (common::fileExists(app))
		return app; // it's already a full path

	else if (common::fileExists("ms0:/PSP/GAME/"+app+"/FBOOT.PBP"))
		return "ms0:/PSP/GAME/"+app+"/FBOOT.PBP";

	else if (common::fileExists("ms0:/PSP/GAME/"+app+"/EBOOT.PBP"))
		return "ms0:/PSP/GAME/"+app+"/EBOOT.PBP";

	return "";
}

void Eboot::execute(){
	this->executeEboot();
}

bool Eboot::isEboot(const char* path){
	return (common::getMagic(path, 0) == EBOOT_MAGIC);
}
