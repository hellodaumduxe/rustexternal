#include "stdafx.h"
POINT input::cursor_position;

unsigned long __stdcall input::input_loop(void* param)
{
	while (true)
	{
		POINT cursor;
		GetCursorPos(&cursor);
		input::cursor_position = { (cursor.x - overlay::x), (cursor.y - overlay::y) };

		if (GetAsyncKeyState(VK_INSERT) & 1)
			menu::is_open = !menu::is_open;

		uint16_t vk_left = GetAsyncKeyState(VK_LBUTTON);
		if (vk_left == 0x8001)
			menu::on_event(click);
		else if (vk_left == 0x8000)
			menu::on_event(drag);

		Sleep(25);
	}

	return 0;
}

void input::start()
{
	HANDLE thread = CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(input::input_loop), nullptr, 0, 0);
	SetThreadPriority(thread, THREAD_MODE_BACKGROUND_BEGIN);
}