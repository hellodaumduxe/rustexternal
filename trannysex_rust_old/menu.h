#pragma once

enum spawn_reasons : uint8_t
{
	render,
	click,
	drag
};

enum tabs : int
{
	aim,
	visual,
	settings
};

namespace menu
{
	extern int x;
	extern int y;
	extern int watermark_x;
	extern int watermark_y;
	extern int counter;

	extern int tab_index;
	extern bool is_open;

	void draw_menu_box(int x, int y, int w, int h);

	void on_event(spawn_reasons event);
	void on_render();
	void create();
};