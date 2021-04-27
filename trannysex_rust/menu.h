#pragma once

enum context_flags
{
	flag_select = (1 << 0),
	flag_left = (1 << 1),
	flag_right = (1 << 2)
};

class c_menu_ctx
{
public:
	bool opened;
	uint8_t tab;
	int8_t index;
	uint8_t flags;
	bool check_flag(int flag)
	{
		uint8_t backup_flags = this->flags;
		this->flags &= ~flag;
		return (backup_flags & flag);
	}

	c_menu_ctx() { this->opened = false; this->tab = 0; this->index = 0; this->flags = 0; }
};

class c_menu_drawing_ctx
{
public:
	uint8_t tab;
	uint8_t index;

	void reset() { this->tab = 0; this->index = 0; }
	c_menu_drawing_ctx() { this->reset(); }
};

namespace menu
{
	extern c_menu_ctx menu_ctx;
	extern c_menu_drawing_ctx menu_drawing_ctx;

	void begin_tab(const char* name);
	void end_tab();
	void checkbox(const char* name, bool* value);
	void slider(const char* name, int* value, int min, int max, int iterator);

	void on_input_loop();
	void on_render();
	void init();
};