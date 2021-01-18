#ifndef GAMEMGR_H
#define GAMEMGR_H

#include <time.h>
#include <cstdlib>
#include <dirent.h>
#include <cstdio>
#include <string>
#include <cstring>
#include "menu.h"
#include "controller.h"
#include "gfx.h"

#define MAX_CATEGORIES 3

using namespace std;

class GameManager{

	private:
	
		/* Array of game menus */
		CatMenu* categories[MAX_CATEGORIES];
		/* Selected game menu */
		int selectedCategory;
		
		/* find all available menu entries
			ms0:/PSP/GAME for eboots
			ms0:/ISO for ISOs
			ms0:/PSP/SAVEDATA for both
		 */
		void findEntries();
		void findEboots();
		void findISOs();
		void findSaveEntries();

		/* move the menu in the specified direction */
		void moveLeft();
		void moveRight();
		void moveUp();
		void moveDown();
	
	public:
	
		GameManager();
		~GameManager();
		
		/* obtain the currently selected entry */
		Entry* getEntry();
		
		/* draw all three menus */
		void draw();
		/* clears the screen and updates it with the menus and text */
		void updateScreen();
		
		/* control the menus */
		void run();
		
};

#endif
