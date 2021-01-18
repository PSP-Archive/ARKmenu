#ifndef ENTRY_H
#define ENTRY_H

#include <string>
#include <cstdio>
#include <cstdlib>
#include <malloc.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include "common.h"
#include "controller.h"
#include "gfx.h"

#define PSN_DRIVER 2
#define ISO_DRIVER 3

#define HOMEBREW_RUNLEVEL 0x141
#define ISO_RUNLEVEL 0x123
#define POPS_RUNLEVEL 0x144

using namespace std;

class Entry{

	private:
		void executeHomebrew();
		void executePSN();
		void executePOPS();

	protected:

		string name;
		string path;
		Image* icon0;
		Image* pic0;
		Image* pic1;

		virtual Image* loadIcon()=0;
		
		void animAppear();
		void animDisappear();
		
		void executeISO();
		
		void executeEboot();
				
	public:
		Entry();
		Entry(string path);
		virtual ~Entry()=0;
		
		string getName();
		
		string getPath();
		
		Image* getIcon();
		
		virtual Image* getPic0()=0;
		
		virtual Image* getPic1()=0;
		
		bool run();
		
		virtual void execute()=0;
		
};

#endif
