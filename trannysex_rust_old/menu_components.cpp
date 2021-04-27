#include "stdafx.h"
/*
__forceinline c_menu_rect generate_bounding_box(e_menu_item_type type, int x, int y)
{
	int left = menu::menu_pos.x + MENU_START_LEFT + x;
	int top = menu::menu_pos.y + MENU_START_TOP + y;
	int right = (left + MENU_ROW_WIDTH);// - 10;
	int bottom = top + MENU_COLUMN_HEIGHT;

	return { left, top, right, bottom };
}

__forceinline c_menu_rect generate_tab_bounding_box(int id)
{
	int left = menu::x + 4 + (id * 65);
	int top = menu::y + 21;
	int right = left + 65;
	int bottom = top + 18;

	return { left, top, right, bottom };
}

__forceinline bool setup_component(int tab_index, POINT& pos, e_spawn_reason reason)
{
	if (tab_index != menu::tab_index)
		return false;

	if (
		reason != e_spawn_reason::render &&
		menu_combo_box::is_combo_box_opened()
		)
		return false;

	pos.x *= MENU_ROW_WIDTH;
	pos.y *= MENU_COLUMN_HEIGHT;

	return true;
}

void menu_components::add_tab(int id, const char* text, menu_event event)
{
	c_menu_rect bounding_box = generate_tab_bounding_box(id);

	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		int x = bounding_box.left;
		int y = bounding_box.top;
		int w = bounding_box.right - bounding_box.left;
		int h = bounding_box.bottom - bounding_box.top;

		if (id == menu::tab_index)
		{
			drawing::draw_outset_menu_box(x, y - 1, w, h + 4);
			drawing::draw_rectangle(x, y, 1, h + 3, menu_color4);

			if (id == 0)
				drawing::draw_rectangle(x + 1, y + h + 1, w - 2, 2, menu_color2);
			else
				drawing::draw_rectangle(x, y + h + 1, w, 2, menu_color2);

			drawing::menu_font->DrawTextA(nullptr, text, -1, reinterpret_cast<c_rect*>(&bounding_box), DT_CENTER | DT_VCENTER | DT_NOCLIP, menu_color_yellow);
		}
		else
		{
			int addition = (id == 0) ? 1 : 0;
			drawing::draw_outset_menu_box(x, y + 1, w, h + 4);
			drawing::draw_rectangle(x + addition, y + h + 1, w, 4, menu_color2);
			drawing::draw_rectangle(x + addition, y + h + 1, w, 1, menu_color4);

			if (addition)
				drawing::draw_rectangle(x, y + 3, 1, h + 3, menu_color4);

			drawing::menu_font->DrawTextA(nullptr, text, -1, reinterpret_cast<c_rect*>(&bounding_box), DT_CENTER | DT_VCENTER | DT_NOCLIP, c_white);
		}

	}
	break;

	case e_spawn_reason::click:
	{
		if (bounding_box.is_click_in_bbox(event.cursor))
		{
			menu::tab_index = id;
		}
	}
	break;

	}
}

void menu_components::add_text(int tab_index, const char* text, POINT pos, menu_event event)
{
	if (!setup_component(tab_index, pos, event.reason))
		return;

	if (event.reason == e_spawn_reason::render)
	{
		c_menu_rect bounding_box = generate_bounding_box(e_menu_item_type::text, pos.x, pos.y);

		int x = bounding_box.left;
		int y = bounding_box.top;
		int w = bounding_box.right - bounding_box.left;
		int h = bounding_box.bottom - bounding_box.top;

		drawing::draw_centered_text(nullptr, drawing::menu_font, x, y, w, text, c_light_grey);
	}
}

char msg[256];
void menu_components::add_dynamic_text(int tab_index, const char* text, int& amount, POINT pos, menu_event event)
{
	if (!setup_component(tab_index, pos, event.reason))
		return;

	if (event.reason == e_spawn_reason::render)
	{
		c_menu_rect bounding_box = generate_bounding_box(e_menu_item_type::text, pos.x, pos.y);

		int x = bounding_box.left;
		int y = bounding_box.top;
		int w = bounding_box.right - bounding_box.left;
		int h = bounding_box.bottom - bounding_box.top;

		macro::print_to_buffer(msg, "%d %s", amount, text);
		drawing::draw_centered_text(nullptr, drawing::menu_font, x, y, w, msg, c_light_grey);
	}
}

void menu_components::add_checkbox(int tab_index, const char* text, bool& setting, POINT pos, menu_event event)
{
	if (!setup_component(tab_index, pos, event.reason))
		return;

	c_menu_rect bounding_box = generate_bounding_box(e_menu_item_type::checkbox, pos.x, pos.y);

	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		int x = bounding_box.left;
		int y = bounding_box.top;
		int w = bounding_box.right - bounding_box.left;
		int h = bounding_box.bottom - bounding_box.top;

		drawing::draw_inset_menu_box(x + 4, y + 5, 11, 11);

		if (setting == true)
			drawing::draw_checkbox(x + 5, y + 5);

		drawing::draw_text(nullptr, drawing::menu_font, x + 18, bounding_box.top + 3, text, c_light_grey);
	}
	break;

	case e_spawn_reason::click:
	{
		if (bounding_box.is_click_in_bbox(event.cursor))
			setting = !setting;
	}
	break;

	}
}

void menu_components::add_slider(int tab_index, int min, int max, int& setting, POINT pos, menu_event event)
{
	if (!setup_component(tab_index, pos, event.reason))
		return;

	c_menu_rect bounding_box = generate_bounding_box(e_menu_item_type::slider, pos.x, pos.y);

	int x = bounding_box.left;
	int y = bounding_box.top;
	int w = (bounding_box.right - bounding_box.left) - 5;
	int h = bounding_box.bottom - bounding_box.top;

	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		float percentage = static_cast<float>(setting) / static_cast<float>(max);
		float new_value = percentage * static_cast<float>(w - 11);
		int slider_x = static_cast<int>(new_value) + 3;

		drawing::draw_inset_menu_box(x + 4, y + 6, w - 3, 6);
		drawing::draw_slider(x + slider_x, y);
	}
	break;

	case e_spawn_reason::click:
	case e_spawn_reason::drag:
	{
		if (!bounding_box.is_click_in_bbox(event.cursor))
			return;

		int delta_x = (event.cursor.x - menu::menu_pos.x - MENU_START_LEFT - 6) - static_cast<int>(pos.x);

		float f_min = static_cast<float>(min);
		float f_max = static_cast<float>(max);

		float percentage = static_cast<float>(delta_x) / static_cast<float>(w - 12);
		float new_value = percentage * (f_max - f_min);
		new_value += f_min;

		math::clamp(new_value, f_min, f_max);
		setting = static_cast<int>(new_value);

	}
	break;
	}
}

void menu_components::add_button(int tab_index, const char* text, bool& setting, POINT pos, menu_event event)
{
	if (!setup_component(tab_index, pos, event.reason))
		return;

	c_menu_rect bbox = generate_bounding_box(e_menu_item_type::button, pos.x, pos.y);
	int x = bbox.left;
	int y = bbox.top;
	int w = bbox.right - bbox.left;
	int h = bbox.bottom - bbox.top;


	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		drawing::draw_outlined_outset_menu_box(x + 2, y, w - 4, h);
		drawing::draw_centered_text(nullptr, drawing::menu_font, x, y + 2, w, text, c_light_grey);
	}
	break;

	case e_spawn_reason::click:
	{
		if (bbox.is_click_in_bbox(event.cursor))
			setting = !setting;
	}
	break;
	}
}

void menu_components::add_combo_box(int tab_index, const char* name, const char** items, size_t size, int& setting, POINT pos, menu_event event)
{
	POINT pos_backup = pos;
	if (!setup_component(tab_index, pos, event.reason))
		return;

	c_menu_rect bbox = generate_bounding_box(e_menu_item_type::combobox, pos.x, pos.y);
	int x = bbox.left;
	int y = bbox.top;
	int w = bbox.right - bbox.left;
	int h = bbox.bottom - bbox.top;


	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		drawing::draw_outlined_inset_menu_box(x + 4, y + 2, w - 8, h - 4);
		drawing::draw_text(nullptr, drawing::small_menu_font, x + 5, y + 2, items[setting], c_light_grey);

		//drawing::draw_outlined_outset_menu_box(x + w - 14, y + 2, 10, 14);
		drawing::draw_combo_box_arrow((x + w) - 13, y + 7);
	}
	break;

	case e_spawn_reason::click:
	{
		if (bbox.is_click_in_bbox(event.cursor))
			menu_combo_box::create(pos_backup, tab_index, items, size, &setting);
	}
	break;

	}
}

void menu_components::add_toggle_button(int tab_index, const char* name, const char* on_name, const char* off_name, bool& setting, POINT pos, menu_event event)
{
	POINT pos_backup = pos;
	if (!setup_component(tab_index, pos, event.reason))
		return;

	c_menu_rect bbox = generate_bounding_box(e_menu_item_type::toggle_button, pos.x, pos.y);
	int x = bbox.left;
	int y = bbox.top;
	int w = bbox.right - bbox.left;
	int h = bbox.bottom - bbox.top;

	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		drawing::draw_outlined_rectangle(x + 3, y + 2, 28, h - 4, menu_color1, c_black);

		int x_pos = (setting == true) ? x + 3 : x + 17;
		drawing::draw_outlined_outset_menu_box(x_pos, y + 2, 14, h - 4);

		drawing::draw_text(nullptr, drawing::menu_font, x + 35, y + 3, ((setting == true) ? on_name : off_name), c_black);
	}
	break;

	case e_spawn_reason::click:
	{
		if (bbox.is_click_in_bbox(event.cursor))
			setting = !setting;
	}
	break;

	}
}

void menu_components::add_divider(int tab_index, const char* name, int size, POINT pos, menu_event event)
{
	POINT pos_backup = pos;
	if (!setup_component(tab_index, pos, event.reason))
		return;

	c_menu_rect bbox = generate_bounding_box(e_menu_item_type::divider, pos.x, pos.y);
	int x = bbox.left;
	int y = bbox.top;
	int w = bbox.right - bbox.left;
	int base_h = bbox.bottom - bbox.top;
	int h = base_h * (size + 1);

	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		drawing::draw_rectangle(x + 1, y + 2, w - 2, h - 1, menu_color0);
		drawing::draw_rectangle(x + 2, y + 18, w - 4, h - 18, menu_color1);

		drawing::draw_rectangle(x + 2, y + 3, w - 4, 14, menu_color4);
		drawing::draw_rectangle(x + 3, y + 4, w - 5, 13, menu_color2);
		drawing::draw_text(nullptr, drawing::small_menu_font, x + 3, y + 3, name, c_white);
	}
	break;
	}
}*/

/*
	int left = menu::x + 4 + (id * 65);
	int top = menu::y + 21;
	int right = left + 65;
	int bottom = top + 18;

	return { left, top, right, bottom };
}

void menu_components::add_tab(int id, const char* text, menu_event event)
{
	c_menu_rect bounding_box = generate_tab_bounding_box(id);

	switch (event.reason)
	{
	case e_spawn_reason::render:
	{
		int x = bounding_box.left;
		int y = bounding_box.top;
		int w = bounding_box.right - bounding_box.left;
		int h = bounding_box.bottom - bounding_box.top;

		if (id == menu::tab_index)
		{
			drawing::draw_outset_menu_box(x, y - 1, w, h + 4);
			drawing::draw_rectangle(x, y, 1, h + 3, menu_color4);

			if (id == 0)
				drawing::draw_rectangle(x + 1, y + h + 1, w - 2, 2, menu_color2);
			else
				drawing::draw_rectangle(x, y + h + 1, w, 2, menu_color2);

			drawing::menu_font->DrawTextA(nullptr, text, -1, reinterpret_cast<c_rect*>(&bounding_box), DT_CENTER | DT_VCENTER | DT_NOCLIP, menu_color_yellow);
		}
		else
		{
			int addition = (id == 0) ? 1 : 0;
			drawing::draw_outset_menu_box(x, y + 1, w, h + 4);
			drawing::draw_rectangle(x + addition, y + h + 1, w, 4, menu_color2);
			drawing::draw_rectangle(x + addition, y + h + 1, w, 1, menu_color4);

			if (addition)
				drawing::draw_rectangle(x, y + 3, 1, h + 3, menu_color4);

			drawing::menu_font->DrawTextA(nullptr, text, -1, reinterpret_cast<c_rect*>(&bounding_box), DT_CENTER | DT_VCENTER | DT_NOCLIP, c_white);
		}

	}
	break;

	case e_spawn_reason::click:
	{
		if (bounding_box.is_click_in_bbox(event.cursor))
		{
			menu::tab_index = id;
		}
	}
	break;

	}*/

void menu_components::add_tab(int id, spawn_reasons event)
{
	int left = menu::x + 7;
	int top = menu::y + 17 + (id * 78);
	int right = left + 77;
	int bottom = top + 78;
	c_menu_rect bbox = { left, top, right, bottom };

	switch (event)
	{
	case render:
	{
		int x = bbox.left;
		int y = bbox.top;
		int w = bbox.right - bbox.left;
		int h = bbox.bottom - bbox.top;

		bool selected = (id == menu::tab_index);
		if (selected)
		{
			drawing::draw_rectangle(x, y, w, h, d3d_color_argb(255, 47, 47, 47));
			drawing::draw_rectangle(x + 1, y + 1, w, h - 2, d3d_color_argb(255, 24, 24, 24));
		}

		switch (id)
		{
		case aim:
			skeet_components::draw_aim_button(x, y, selected);
			break;
		case visual:
			skeet_components::draw_visuals_button(x, y, selected);
			break;
		case settings:
			skeet_components::draw_settings_button(x, y, selected);
			break;
		}
	}
	break;

	case drag:
	case click:
		if (bbox.is_click_in_bounds(input::cursor_position))
			menu::tab_index = id;
		break;
	}
}