#include "stdafx.h"

c_menu_ctx menu::menu_ctx;
c_menu_drawing_ctx menu::menu_drawing_ctx;
void menu::begin_tab(const char* name)
{
	menu::menu_drawing_ctx.index = 0;
	bool selected = (menu::menu_drawing_ctx.tab == menu::menu_ctx.tab);
	drawing::draw_rectangle(6 + static_cast<float>(menu::menu_drawing_ctx.tab * 46), 63, 45, 12, selected ? c_black : d3d_color(43, 43, 43, 255));
	holy_font::draw_string(holy_font::arial_small, name, 6 + static_cast<float>(menu::menu_drawing_ctx.tab * 46), 63, 45, flag_centered, c_white);
}
void menu::end_tab()
{
	if (menu::menu_drawing_ctx.tab == menu::menu_ctx.tab)
	{
		if (menu::menu_ctx.index >= menu::menu_drawing_ctx.index)
			menu::menu_ctx.index = 0;
		else if (menu::menu_ctx.index < 0)
			menu::menu_ctx.index = (menu::menu_drawing_ctx.index - 1);
	}

	menu::menu_drawing_ctx.index = 0;
	menu::menu_drawing_ctx.tab++;
}
void menu::checkbox(const char* name, bool* value)
{
	if (menu::menu_drawing_ctx.tab != menu::menu_ctx.tab)
		return;

	int i = (menu::menu_drawing_ctx.index + 1);
	bool selected = (menu::menu_drawing_ctx.index == menu::menu_ctx.index);
	if (selected)
	{
		if (menu::menu_ctx.check_flag(flag_select) || menu::menu_ctx.check_flag(flag_left) || menu::menu_ctx.check_flag(flag_right))
			*value = !*value;
		drawing::draw_outlined_rectangle(6, 64 + static_cast<float>(i * 12), 137, 12, c_black, c_pink);
	}

	holy_font::draw_string(holy_font::arial_small, name, 10, 65 + static_cast<float>(i * 12), 45, 0, c_white);
	drawing::draw_outlined_rectangle(133, 66 + static_cast<float>(i * 12), 8, 8, ((*value == true) ? c_pink : c_black), c_pink);
	menu::menu_drawing_ctx.index++;
}
void menu::slider(const char* name, int* value, int min, int max, int iterator)
{
	if (menu::menu_drawing_ctx.tab != menu::menu_ctx.tab)
		return;

	int i = (menu::menu_drawing_ctx.index + 1);
	bool selected = (menu::menu_drawing_ctx.index == menu::menu_ctx.index);
	if (selected)
	{
		drawing::draw_outlined_rectangle(6, 64 + static_cast<float>(i * 12), 137, 12, c_black, c_pink);
		holy_font::draw_string(holy_font::arial_small, s("<"), 110, 65 + static_cast<float>(i * 12), 45, 0, c_pink);
		holy_font::draw_string(holy_font::arial_small, s(">"), 135, 65 + static_cast<float>(i * 12), 45, 0, c_pink);

		if (menu::menu_ctx.check_flag(flag_left))
		{
			if (*value > min)
				*value = *value - iterator;
		}
		else if (menu::menu_ctx.check_flag(flag_right))
		{
			if (*value < max)
				*value = *value + iterator;
		}
	}

	char buf[256];
	macro::print_to_buffer(buf, s("%d"), *value);
	holy_font::draw_string(holy_font::arial_small, buf, (selected ? 110.0f : 122.0f), 65 + static_cast<float>(i * 12), 31, flag_centered, c_white);
	memset(buf, 0, sizeof(buf));

	holy_font::draw_string(holy_font::arial_small, name, 10, 65 + static_cast<float>(i * 12), 45, 0, c_white);
	menu::menu_drawing_ctx.index++;
}

void menu::on_input_loop()
{
	if (GetAsyncKeyState(VK_DELETE) & 0x8000)
	{
		menu::menu_ctx.opened = !menu::menu_ctx.opened;
		Sleep(150);
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		menu::menu_ctx.index--;
		Sleep(70);
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		menu::menu_ctx.index++;
		Sleep(70);
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		menu::menu_ctx.flags |= flag_left;
		Sleep(110);
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		menu::menu_ctx.flags |= flag_right;
		Sleep(110);
	}
	else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		menu::menu_ctx.flags |= flag_select;
		Sleep(170);
	}
	else if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		menu::menu_ctx.tab++;
		if (menu::menu_ctx.tab > 2)
			menu::menu_ctx.tab = 0;
		Sleep(170);
	}
}

void menu::on_render()
{
	if (!menu::menu_ctx.opened)
		return;

	//background
	menu::menu_drawing_ctx.reset();
	drawing::draw_outlined_rectangle(5, 50, 139, 193, c_pink, c_black);
	holy_font::draw_string(holy_font::arial_small, s("trannysex rust"), 5, 50, 139, flag_outline | flag_centered, c_white);
	drawing::draw_rectangle(5, 62, 139, 195, c_black);

	menu::begin_tab(s("aim"));
	menu::checkbox(s("enabled"), &settings::o.aim.enabled);
	menu::slider(s("fov"), &settings::o.aim.fov, 0, 200, 1);
	menu::checkbox(s("silent"), &settings::o.aim.silent);
	menu::checkbox(s("smoothing"), &settings::o.aim.smoothing.enabled);
	menu::slider(s("smoothing amount"), &settings::o.aim.smoothing.amount, 0, 100, 1);
	menu::slider(s("smoothing x"), &settings::o.aim.smoothing.x, 0, 50, 1);
	menu::slider(s("smoothing y"), &settings::o.aim.smoothing.y, 0, 50, 1);
	menu::end_tab();

	menu::begin_tab(s("visuals"));
	menu::checkbox(s("crosshair"), &settings::o.visuals.crosshair);
	menu::checkbox(s("radar"), &settings::o.visuals.radar);
	menu::slider(s("radar distance"), &settings::o.visuals.radar_distance, 0, 30, 1);
	menu::checkbox(s("npc esp"), &settings::o.visuals.npcs);
	menu::checkbox(s("sleeper esp"), &settings::o.visuals.sleepers);
	menu::checkbox(s("item esp"), &settings::o.visuals.items);
	menu::checkbox(s("chest esp"), &settings::o.visuals.chest);
	menu::checkbox(s("tc esp"), &settings::o.visuals.tc);
	menu::checkbox(s("player esp"), &settings::o.visuals.players.enabled);
	menu::checkbox(s("player name"), &settings::o.visuals.players.name);
	menu::checkbox(s("player skeleton"), &settings::o.visuals.players.skeleton);
	menu::checkbox(s("player health"), &settings::o.visuals.players.health);
	menu::end_tab();

	menu::begin_tab(s("misc"));
	menu::checkbox(s("debug"), &settings::o.misc.debug);
	menu::checkbox(s("no recoil"), &settings::o.misc.no_recoil);
	menu::checkbox(s("no spread"), &settings::o.misc.no_spread);
	menu::end_tab();
}

void menu::init()
{
	menu::menu_ctx = c_menu_ctx();
	menu::menu_drawing_ctx = c_menu_drawing_ctx();
}