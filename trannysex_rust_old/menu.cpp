#include "stdafx.h"

int menu::x;
int menu::y;
int menu::watermark_x;
int menu::watermark_y;
int menu::counter;

int menu::tab_index;
bool menu::is_open;

inline void menu::draw_menu_box(int x, int y, int w, int h)
{
	drawing::draw_rectangle(x, y, w, h, d3d_color_argb(255, 12, 12, 12));
	drawing::draw_rectangle(x + 1, y + 1, w - 2, h - 2, d3d_color_argb(255, 57, 57, 57));
	drawing::draw_rectangle(x + 2, y + 2, w - 4, h - 4, d3d_color_argb(255, 37, 37, 37));
	drawing::draw_rectangle(x + 5, y + 5, w - 10, h - 10, d3d_color_argb(255, 57, 57, 57));
	drawing::draw_rectangle(x + 6, y + 6, w - 12, h - 12, d3d_color_argb(255, 12, 12, 12));
	drawing::draw_skeet_rainbow_shit(x + 7, y + 7, w - 14);
}

void menu::on_event(spawn_reasons event)
{
	menu_components::add_tab(0, event);
	menu_components::add_tab(1, event);
	menu_components::add_tab(2, event);
}

void menu::on_render()
{
#pragma region watermark
	menu::draw_menu_box(menu::watermark_x, menu::watermark_y, 114, 31);
	fonts::pixel_font.draw_string(s("trannysex"), menu::watermark_x + 10, menu::watermark_y + 11, 0, 0, c_white);
	fonts::pixel_font.draw_string(s("rust"), menu::watermark_x + 28, menu::watermark_y + 11, 0, 0, c_pink);
	fonts::pixel_font.draw_string(s("fps"), menu::watermark_x + 70, menu::watermark_y + 11, 0, 0, c_white);
	fonts::pixel_font.draw_string(fps_counter::fps_buffer, menu::watermark_x + 88, menu::watermark_y + 11, 0, 0, c_white);
#pragma endregion

	if (menu::is_open)
	{
		int w = 350;
		int h = 400;

		menu::draw_menu_box(menu::x, menu::y, w, h);
		drawing::draw_rectangle(menu::x + 84, menu::y + 9, w - 90, h - 15, d3d_color_argb(255, 47, 47, 47));
		drawing::draw_rectangle(menu::x + 85, menu::y + 9, w - 91, h - 15, d3d_color_argb(255, 24, 24, 24));
		menu::on_event(render);
	}

	menu::counter++;
}

void menu::create()
{
	menu::x = 60;
	menu::y = 60;
	menu::watermark_x = 20;
	menu::watermark_y = 20;
	menu::counter = 0;

	menu::tab_index = 0;
	menu::is_open = false;
}