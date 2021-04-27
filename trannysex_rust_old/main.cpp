#include "stdafx.h"

int main()
{
	memory::init();
	input::start();
	menu::create();
	fps_counter::start();
	entities::start();
	aim_bot::start();
	overlay::start();
	return 0;
}