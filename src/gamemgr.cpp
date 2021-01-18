/* Game Manager class */

#include "gamemgr.h"
#include <sstream>

GameManager::GameManager(){

	this->selectedCategory = 0;
	debugScreen("creating categories");
	for (int i=0; i<MAX_CATEGORIES; i++){
		this->categories[i] = new CatMenu((EntryType)i);
	}
	this->findEntries();
}

GameManager::~GameManager(){
	for (int i=0; i<MAX_CATEGORIES; i++){
		delete this->categories[i];
	}
}

void GameManager::findEntries(){
	debugScreen("finding Eboots");
	this->findEboots();
	debugScreen("finding ISOs");
	this->findISOs();
	debugScreen("finding Save Entries");
	//this->findSaveEntries();
}

void GameManager::findEboots(){
	struct dirent* dit;
	DIR* dir = opendir("ms0:/PSP/GAME/");
	
	if (dir == NULL)
		return;
		
	while ((dit = readdir(dir))){

		string fullpath = Eboot::fullEbootPath(dit->d_name);
		if (strcmp(dit->d_name, ".") == 0) continue;
		if (strcmp(dit->d_name, "..") == 0) continue;
		if (strcmp(dit->d_name, "arkMenu") == 0) continue;
		if (common::fileExists(string("ms0:/PSP/GAME/")+string(dit->d_name))) continue;
		
		Eboot* e = new Eboot(fullpath);
		switch (Eboot::getEbootType(fullpath.c_str())){
		case TYPE_HOMEBREW:	this->categories[HOMEBREW]->addEntry(e);	break;
		case TYPE_PSN:		this->categories[GAME]->addEntry(e);		break;
		case TYPE_POPS:		this->categories[POPS]->addEntry(e);		break;
		}
	}
	closedir(dir);
}

void GameManager::findISOs(){

	struct dirent* dit;
	DIR* dir = opendir("ms0:/ISO/");
	
	if (dir == NULL)
		return;
		
	while ((dit = readdir(dir))){

		string fullpath = string("ms0:/ISO/")+string(dit->d_name);

		if (strcmp(dit->d_name, ".") == 0) continue;
		if (strcmp(dit->d_name, "..") == 0) continue;
		if (!common::fileExists(fullpath)) continue;
		if (Iso::isISO(fullpath.c_str())) this->categories[GAME]->addEntry(new Iso(fullpath));
		else if (Cso::isCSO(fullpath.c_str())) this->categories[GAME]->addEntry(new Cso(fullpath));
	}
	closedir(dir);

}

void GameManager::findSaveEntries(){
	struct dirent* dit;
	DIR* dir = opendir("ms0:/PSP/SAVEDATA/");
	
	if (dir == NULL)
		return;
		
	while ((dit = readdir(dir))){

		string fullpath = string("ms0:/PSP/SAVEDATA/")+string(dit->d_name);

		if (strcmp(dit->d_name, ".") == 0) continue;
		if (strcmp(dit->d_name, "..") == 0) continue;
		if (!common::fileExists(fullpath)){
			struct dirent* savedit;
			DIR* savedir = opendir(fullpath.c_str());
			if (savedir == NULL)
				continue;
			while ((savedit = readdir(savedir))){
				string fullentrypath = fullpath + string(savedit->d_name);
				if (Iso::isISO(fullentrypath.c_str())) this->categories[GAME]->addEntry(new Iso(fullentrypath));
				else if (Cso::isCSO(fullentrypath.c_str())) this->categories[GAME]->addEntry(new Cso(fullentrypath));
				else if (Eboot::isEboot(fullentrypath.c_str())){
					Eboot* e = new Eboot(fullentrypath);
					switch (Eboot::getEbootType(fullentrypath.c_str())){
					case TYPE_HOMEBREW:	this->categories[HOMEBREW]->addEntry(e);	break;
					case TYPE_PSN:		this->categories[GAME]->addEntry(e);		break;
					case TYPE_POPS:		this->categories[POPS]->addEntry(e);		break;
					}
				}
			}
			closedir(savedir);
		}
	}
	closedir(dir);
}

Entry* GameManager::getEntry(){
	return this->categories[this->selectedCategory]->getEntry();
}

void GameManager::moveLeft(){
	if (this->selectedCategory > 0)
		this->selectedCategory--;
	else
		this->selectedCategory = MAX_CATEGORIES-1;
}
void GameManager::moveRight(){
	if (this->selectedCategory < MAX_CATEGORIES-1)
		this->selectedCategory++;
	else
		this->selectedCategory = 0;
}

void GameManager::moveUp(){
	this->categories[this->selectedCategory]->moveUp();
}
void GameManager::moveDown(){
	this->categories[this->selectedCategory]->moveDown();
}

void GameManager::draw(){
	for (int i=0; i<MAX_CATEGORIES; i++){
		this->categories[i]->draw(i == (int)this->selectedCategory);
	}
}

void GameManager::updateScreen(){
	clearScreen(CLEAR_COLOR);
	blitAlphaImageToScreen(0, 0, 480, 272, common::getBG(), 0, 0);
	
	this->draw();
	
	guStart();
	float t = ((float)(clock() % CLOCKS_PER_SEC)) / ((float)CLOCKS_PER_SEC);
	int val = (t < 0.5f) ? t*511 : (1.0f-t)*511;
	intraFontSetStyle(common::getFont(), 0.7f, LITEGRAY, (0xFF<<24)+(val<<16)+(val<<8)+(val),0);
	intraFontPrint(common::getFont(), 0, 15, this->getEntry()->getName().c_str());
	
	common::flip();
}

void GameManager::run(){

	Controller control;
	
	while(true){
		this->updateScreen();
		control.update();
		if (control.down())
			this->moveDown();
		else if (control.up())
			this->moveUp();
		else if (control.left())
			this->moveLeft();
		else if (control.right())
			this->moveRight();
		else if (control.cross()){
			if (this->getEntry()->run()){
				this->getEntry()->execute();
				break;
			}
		}		
		else if (control.circle()){
			break;
		}
	}
}
