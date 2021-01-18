/* Category Class that defines each of the individual menus */

#include "menu.h"

CatMenu::CatMenu(EntryType t){
	this->type = t;
	this->index = 0;
	this->start = 0;
}


CatMenu::~CatMenu(){
	this->entries.clear();
}


void CatMenu::draw(bool selected){
	int xoffset = ((int)this->type)*144 + ((int)this->type)*10;
	int yoffset = 30;
	for (int i=this->start; i<min(this->start+MAX_DRAW_ENTRIES, (int)entries.size()); i++){
		Image* icon = entries[i]->getIcon();
		if (i==this->index && selected){
			blitAlphaImageToScreen(0, 0, icon->imageWidth, \
				icon->imageHeight, icon, xoffset, min(yoffset, 272-icon->imageHeight));
			yoffset += (int)(icon->imageHeight);
		}
		else{
			blitScaleToScreen(icon, xoffset, yoffset, 0.7f, 0.7f);
			yoffset += (int)(icon->imageHeight*0.7f);
		}
	}
}

void CatMenu::addEntry(Entry* e){
	this->entries.push_back(e);
}

Entry* CatMenu::getEntry(){
	return this->entries[this->index];
}
				
void CatMenu::moveUp(){
	if (this->index == 0)
		return;
	else if (this->index == this->start){
		this->index--;
		if (this->start>0)
			this->start--;
	}
	else
		this->index--;
}

void CatMenu::moveDown(){
	if (this->index == entries.size())
		return;
	else if (this->index-this->start == MAX_DRAW_ENTRIES-1){
		if (this->index+1 < entries.size()-1)
			this->index++;
		if (this->start+MAX_DRAW_ENTRIES+1 < entries.size())
			this->start++;
	}
	else if (this->index+1 < entries.size())
		this->index++;
}
