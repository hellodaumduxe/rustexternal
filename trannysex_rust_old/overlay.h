#pragma once

namespace overlay
{
	extern int x;
	extern int y;
	extern int width;
	extern int height;
	extern vec3 screen_center;

	extern IDirect3DDevice9* device;


	bool init_d3d();
	bool init_overlay();
	bool start();
};