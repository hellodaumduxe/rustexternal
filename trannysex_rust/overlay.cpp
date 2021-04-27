#include "stdafx.h"

int overlay::x;
int overlay::y;
int overlay::width;
int overlay::height;

bool overlay::loaded;
float overlay::rust_x;
float overlay::rust_y;
float overlay::rust_width;
float overlay::rust_height;
vec3 overlay::screen_center;

int frame;
int y_border;
int x_modifier;
int y_modifier;

HWND wmplayer_hwnd = 0;
void overlay::load()
{
	if (overlay::loaded)
		return;

	RECT rect;
	GetWindowRect(FindWindowA(nullptr, s("Rust")), &rect);
	overlay::rust_x = static_cast<float>((rect.left - overlay::x) + x_modifier);
	overlay::rust_y = static_cast<float>((rect.top - overlay::y) + y_modifier);
	overlay::rust_width = static_cast<float>((rect.right - rect.left) - (x_modifier * 2));
	overlay::rust_height = static_cast<float>((rect.bottom - rect.top) - (y_modifier + (y_border + frame)));
	overlay::screen_center = { (static_cast<float>(overlay::rust_width) / 2.0f), (static_cast<float>(overlay::rust_height) / 2.0f), 0.0f };

	D3D11_VIEWPORT view_port = { overlay::rust_x, overlay::rust_y, static_cast<float>(overlay::width), static_cast<float>(overlay::height), 0, 1 };
	drawing::device_context->RSSetViewports(1, &view_port);
	overlay::loaded = true;
}
void overlay::unload()
{
	overlay::loaded = false;
	overlay::rust_x = 0.0f;
	overlay::rust_y = 0.0f;
	overlay::rust_width = 0.0f;
	overlay::rust_height = 0.0f;
	overlay::screen_center = { 0.0f, 0.0f, 0.0f };

	D3D11_VIEWPORT view_port = { 0, 0, static_cast<float>(overlay::width), static_cast<float>(overlay::height), 0, 1 };
	drawing::device_context->RSSetViewports(1, &view_port);
}

c_matrix4x4 view_matrix;
unsigned long __stdcall overlay::loop(void* param)
{
	frame = GetSystemMetrics(SM_CXSIZEFRAME);
	y_border = GetSystemMetrics(SM_CYBORDER);
	x_modifier = (GetSystemMetrics(SM_CXBORDER) + GetSystemMetrics(SM_CXSIZEFRAME));
	y_modifier = (y_border + GetSystemMetrics(SM_CYCAPTION) + frame);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	do
	{
		drawing::begin_frame();
		if (input::is_loaded)
		{
			if (entities::loaded && (entities::camera.view_matrix != 0))
			{
				overlay::load();
				memory::read_buffer(entities::camera.view_matrix, &view_matrix, sizeof(view_matrix));
				drawing::transposed_matrix.transpose(&view_matrix);

				visuals::on_render();
				entities::on_render();
				aim::on_render();
				menu::on_render();
				visuals::draw_watermark();
			}
			else
				holy_font::draw_string(holy_font::arial_small, s("loading..."), 5, 5, 0, flag_outline, c_pink);
		}
		else if (setup_complete)
		{
			overlay::unload();
			holy_font::draw_string(holy_font::arial_small, s("press F5 to load trannysex rust."), 5, 5, 0, flag_outline, c_pink);
		}
		drawing::end_frame();
		Sleep(3);
	} while (true);
	return 0;
}

void overlay::init()
{
	print("[>] preparing overlay. \n");
	RECT desktop;
	wmplayer_hwnd = FindWindowA(s("ms_sqlce_se_notify_wndproc"), nullptr);
	if (wmplayer_hwnd == 0)
		wmplayer_hwnd = FindWindowA(s("WMPlayerApp"), nullptr);

	GetWindowRect(GetDesktopWindow(), &desktop);

	overlay::x = (macro::rng() % 86);
	overlay::y = (macro::rng() % 62);
	overlay::width = (desktop.right - desktop.left) - ((macro::rng() % 97) + overlay::x);
	overlay::height = (desktop.bottom - desktop.top) - ((macro::rng() % 70) + overlay::y);

	//menu & flags
	SetMenu(wmplayer_hwnd, nullptr);
	SetWindowLongPtrA(wmplayer_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	SetWindowLongPtrA(wmplayer_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetWindowPos(wmplayer_hwnd, HWND_TOPMOST, overlay::x, overlay::y, overlay::width, overlay::height, 0);
	SetLayeredWindowAttributes(wmplayer_hwnd, 0, 255, LWA_ALPHA);

	//transparency fix
	MARGINS margin = { 0, 0, desktop.bottom, desktop.right };
	DwmExtendFrameIntoClientArea(wmplayer_hwnd, &margin);

	print("[>] starting d3d. \n");
	if (!drawing::init(wmplayer_hwnd))
		return;

	overlay::loaded = false;
	macro::create_thread_fake_start(overlay::loop);
	print("[>] overlay ready!!! \n");
}