#pragma once

namespace fps_counter
{
	extern char fps_buffer[16];

	unsigned long __stdcall fps_loop(void* param);
	void start();
}