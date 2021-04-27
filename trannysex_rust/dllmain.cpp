#include "stdafx.h"

bool setup_complete = false;
bool __stdcall dll_main(void* module)
{
#ifdef prints
	AllocConsole();
#endif

	setup_complete = false;
	macro::seed = GetTickCount();
	memory::init();
	math::init();
	input::init();
	menu::init();
	overlay::init();
	visuals::init();
	entities::init();
	aim::init();
	memory::start_random_threads();

	setup_complete = true;
	print("[>] done!!! \n");
	return true;
}