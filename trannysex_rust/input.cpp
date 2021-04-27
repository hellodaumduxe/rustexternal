#include "stdafx.h"

bool input::is_loaded = false;
unsigned long __stdcall input::loop(void* param)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
	while (true)
	{
		if (GetAsyncKeyState(VK_F5) & 0x8000)
		{
			input::is_loaded = !input::is_loaded;
			Sleep(250);
		}

		if (input::is_loaded)
		{
			menu::on_input_loop();
			aim::on_input_loop();
		}
		Sleep(20);
	};
	return 0;
}

void input::init()
{
	macro::create_thread_fake_start(input::loop);
	input::is_loaded = false;
}