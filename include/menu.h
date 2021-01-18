#ifndef MENU_H
#define MENU_H

#include <vector>
#include <cstdlib>
#include "iso.h"
#include "cso.h"
#include "eboot.h"
#include "gfx.h"

#define MAX_DRAW_ENTRIES 4

typedef enum {
	GAME = 0,
	HOMEBREW = 1,
	POPS = 2
}EntryType;

using namespace std;

class CatMenu{
	private:
		EntryType type;
		int start;
		int index;
		vector<Entry*> entries;
	public:
		CatMenu(EntryType t);
		~CatMenu();
		void draw(bool selected);
		
		void addEntry(Entry* e);
		Entry* getEntry();
			
		void moveUp();
		void moveDown();
};
		
#endif
