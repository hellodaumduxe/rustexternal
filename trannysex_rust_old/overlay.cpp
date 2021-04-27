#include "stdafx.h"

int overlay::x = 1;
int overlay::y = 1;
int overlay::width = 1;
int overlay::height = 1;
vec3 overlay::screen_center;

int x = 1;
int y = 1;
int x_modifier = 1;
int y_modifier = 1;

HWND wmplayer_hwnd = 0x0;
HWND rust_hwnd = 0x0;

IDirect3D9* d3d = nullptr;
IDirect3DDevice9* overlay::device = nullptr;
bool overlay::init_d3d()
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS present_params;
	memset(&present_params, 0x0, sizeof(present_params));

	present_params.Windowed = true;
	present_params.BackBufferCount = 0;
	present_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	present_params.BackBufferWidth = overlay::width;
	present_params.BackBufferHeight = overlay::height;
	present_params.MultiSampleType = D3DMULTISAMPLE_NONE;
	present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	present_params.hDeviceWindow = wmplayer_hwnd;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wmplayer_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &present_params, &overlay::device);
	return true;
}

bool overlay::init_overlay()
{
	rust_hwnd = FindWindowA(nullptr, s("Rust"));
	if (rust_hwnd == 0x0)
	{
		macro::print(s("[-] failed to find rust window. \n"));
		return false;
	}

	macro::print(s("[>] starting wmplayer.exe. \n"));
	macro::system(s("/K start wmplayer.exe"));

	int counter = 0;
	while (wmplayer_hwnd == 0x0)
	{
		macro::print(s("[>] waiting for wmplayer.exe to start. \n"));
		wmplayer_hwnd = FindWindowA(s("WMPlayerApp"), nullptr);

		if (counter > 1000)
		{
			macro::print(s("[-] failed to find wmplayer window. \n"));
			return false;
		}

		counter++;
		Sleep(15);
	}

	//fix width and height
	int frame = GetSystemMetrics(SM_CXSIZEFRAME);
	x_modifier = (GetSystemMetrics(SM_CXBORDER) + frame);

	int y_border = GetSystemMetrics(SM_CYBORDER);
	y_modifier = (y_border + GetSystemMetrics(SM_CYCAPTION) + frame);

	RECT rust_rect;
	GetWindowRect(rust_hwnd, &rust_rect);
	overlay::width = (rust_rect.right - rust_rect.left) - (x_modifier * 2);
	overlay::height = (rust_rect.bottom - rust_rect.top) - (y_modifier + (y_border + frame));
	overlay::screen_center = { static_cast<float>(overlay::width) / 2.0f, static_cast<float>(overlay::height) / 2.0f };

	overlay::x = rust_rect.left;
	overlay::y = rust_rect.top;

	//menu & flags
	SetMenu(wmplayer_hwnd, nullptr);
	SetWindowLongPtrA(wmplayer_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	SetWindowLongPtrA(wmplayer_hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetWindowLongPtrA(wmplayer_hwnd, GWL_WNDPROC, 0x0);
	SetWindowPos(wmplayer_hwnd, HWND_TOPMOST, overlay::x + x_modifier, overlay::y + y_modifier, overlay::width, overlay::height, 0);
	SetLayeredWindowAttributes(wmplayer_hwnd, 0, 255, LWA_ALPHA);

	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	MARGINS margin = { 0, 0, desktop.bottom, desktop.right };
	DwmExtendFrameIntoClientArea(wmplayer_hwnd, &margin);

	return true;
}

bool overlay::start()
{
	macro::print(s("[>] preparing overlay. \n"));
	if (!overlay::init_overlay())
		return false;

	macro::print(s("[>] starting d3d. \n"));
	if (!overlay::init_d3d())
		return false;

	overlay::device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	overlay::device->SetRenderState(D3DRS_LIGHTING, false);
	overlay::device->SetRenderState(D3DRS_FOGENABLE, false);
	overlay::device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	overlay::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	overlay::device->SetRenderState(D3DRS_ZENABLE, false);
	overlay::device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	overlay::device->SetRenderState(D3DRS_ZWRITEENABLE, true);
	overlay::device->SetRenderState(D3DRS_STENCILENABLE, false);
	overlay::device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
	overlay::device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, false);
	overlay::device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	overlay::device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	overlay::device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);
	overlay::device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	overlay::device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	overlay::device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	overlay::device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
	overlay::device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	overlay::device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	overlay::device->SetTexture(0, nullptr);
	macro::print(s("[>] done!!! \n"));

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	while (true)
	{
		overlay::device->Clear(0, 0, D3DCLEAR_TARGET, d3d_color_argb(0, 0, 0, 0), 1.0f, 0);
		overlay::device->BeginScene();

		visuals::on_render();
		aim_bot::on_render();
		menu::on_render();

		overlay::device->EndScene();
		overlay::device->Present(0, 0, 0, 0);
		Sleep(3);
	}

}
