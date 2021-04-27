#include "stdafx.h"

char fps_counter::fps_buffer[16];
uint64_t loop_counter = 0;
uint64_t current_loop = 0;

unsigned long __stdcall fps_counter::fps_loop(void* param)
{
	while (true)
	{
		loop_counter++;
		if (loop_counter > current_loop)
		{
			current_loop = loop_counter;
			macro::print_to_buffer(fps_counter::fps_buffer, s("%d"), menu::counter);
			menu::counter = 0;
		}

		Sleep(1000);
	}

	return 0;
}

void fps_counter::start()
{
	loop_counter = 0;
	current_loop = 0;
	strcpy(fps_counter::fps_buffer, s("60"));


	HANDLE thread = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fps_counter::fps_loop), nullptr, 0, 0);
	SetThreadPriority(thread, THREAD_MODE_BACKGROUND_BEGIN);
}
