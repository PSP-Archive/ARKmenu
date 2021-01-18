#include <pspkernel.h>
#include "gfx.h"
#include "debug.h"
#include "common.h"
#include "gamemgr.h"

PSP_MODULE_INFO("XMENU", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

using namespace std;

volatile int pspInterruptOccurred = 0;

/*!
*	Exit callback function
*/
int exit_callback(int arg1, int arg2, void *common) {
    pspInterruptOccurred = 1;
    sceKernelExitGame();
    return 0;
}

/*!
*	Callback thread function
*/
int CallbackThread(SceSize args, void *argp) {
    int cbid;

    cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();

    return 0;
}

/*!
*	Sets up the callback thread and
*	returns its thread id function
*/
int SetupCallbacks(void) {
    int thid = 0;

    thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);   //als er foutenzijn dan  kan het hieraanliggen
    if(thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }

    return thid;
}

int main(){

	SetupCallbacks();

	intraFontInit();
	initGraphics();

	common::loadData();

	debugScreen("starting menu");
	GameManager* menu = new GameManager();
	menu->run();
	delete menu;
	
	common::deleteData();
	
	intraFontShutdown();
	disableGraphics();
		
	sceKernelExitGame();
}
