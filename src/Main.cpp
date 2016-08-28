//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
#include "./OPengine.h"
#include "Main.h"
#include "GameState.h"

//////////////////////////////////////
// Application Methods
//////////////////////////////////////

OPwindow mainWindow;

void ApplicationInit() {
	OPCMAN.Init(OPIFEX_ASSETS);
	OPloadersAddDefault();

	OPrenderSetup();

	OPwindowSystemInit();
	mainWindow.Init(NULL, OPwindowParameters("Main Window", false, 1280, 720));

	OPrenderInit(&mainWindow);
	OPGAMEPADS.SetDeadzones(0.2f);
	OPVISUALDEBUGINFO.Init();

	OPfmodInit();

	OPgameState::Change(&GS_EXAMPLE);
}

OPint ApplicationUpdate(OPtimer* timer) {
	if (mainWindow.Update()) {
		// Window received an exit request
		return 1;
	}
	OPCMAN.Update(timer);
	OPfmodUpdate();
	OPVISUALDEBUGINFO.Update(timer);

	OPinputSystemUpdate(timer);
	if (OPKEYBOARD.WasPressed(OPkeyboardKey::ESCAPE)) return 1;

	return ActiveState->Update(timer);
}

void ApplicationRender(OPfloat delta) {
	ActiveState->Render(delta);
}

void ApplicationDestroy() {

}

void ApplicationSetup() {
	OPinitialize = ApplicationInit;
	OPupdate = ApplicationUpdate;
	OPrender = ApplicationRender;
	OPdestroy = ApplicationDestroy;
}

//////////////////////////////////////
// Application Entry Point
//////////////////////////////////////
OP_MAIN_START

	OPLOGLEVEL = (ui32)OPlogLevel::VERBOSE;
	OPlog("Starting up OPifex Engine");

	ApplicationSetup();

	OP_MAIN_RUN
	//OP_MAIN_RUN_STEPPED

OP_MAIN_END
