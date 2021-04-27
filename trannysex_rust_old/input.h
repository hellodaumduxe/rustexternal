#pragma once

namespace input
{
	extern POINT cursor_position;

	unsigned long __stdcall input_loop(void* param);
	void start();
}