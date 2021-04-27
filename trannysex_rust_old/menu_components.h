#pragma once

enum menu_item_type
{
	none,
	text,
	dynamic_text,
	checkbox,
	slider,
	button,
	dropdown,
	combobox,
	toggle_button,
	divider
};

namespace menu_components
{
	void add_tab(int id, spawn_reasons event);

	/*void add_tab(int id, const char* text, menu_event event);

	void add_text(int tab_index, const char* text, POINT pos, menu_event event);
	void add_dynamic_text(int tab_index, const char* text, int& amount, POINT pos, menu_event event);
	void add_checkbox(int tab_index, const char* text, bool& setting, POINT pos, menu_event event);
	void add_slider(int tab_index, int min, int max, int& setting, POINT pos, menu_event event);
	void add_button(int tab_index, const char* text, bool& setting, POINT pos, menu_event event);
	void add_combo_box(int tab_index, const char* name, const char** items, size_t size, int& setting, POINT pos, menu_event event);
	void add_toggle_button(int tab_index, const char* name, const char* on_name, const char* off_name, bool& setting, POINT pos, menu_event event);
	void add_divider(int tab_index, const char* name, int size, POINT pos, menu_event event);*/
}