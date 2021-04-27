#pragma once

namespace input
{
	extern bool is_loaded;

	unsigned long __stdcall loop(void* param);
	void init();
};