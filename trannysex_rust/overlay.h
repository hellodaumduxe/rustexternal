#pragma once

namespace overlay
{
	extern int x;
	extern int y;
	extern int width;
	extern int height;

	extern bool loaded;
	extern float rust_x;
	extern float rust_y;
	extern float rust_width;
	extern float rust_height;
	extern vec3 screen_center;
	void load();
	void unload();

	unsigned long __stdcall loop(void* param);
	void init();
};