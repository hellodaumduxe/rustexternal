#pragma once

typedef RECT c_rect;
typedef POINT point;
typedef DWORD d3d_color;
typedef D3DRECT d3d_rect;
typedef LPD3DXFONT d3d_font;
typedef LPD3DXSPRITE d3d_sprite;

struct c_vertex
{
	float x, y, z, rhw;
	d3d_color color;
};

#define vertex_size 0x6000
namespace drawing
{
	extern c_vertex vertex[vertex_size];
	extern d3d_rect rect[16];

	void draw_rectangle(int x, int y, int w, int h, d3d_color color);
	void draw_outlined_rectangle(int x, int y, int w, int h, d3d_color color, d3d_color outline);
	void draw_hollow_rectangle(int x, int y, int w, int h, d3d_color color);
	void draw_outlined_hollow_rectangle(int x, int y, int w, int h, d3d_color color, d3d_color outline);
	void draw_gradient_rectangle(int x, int y, int w, int h, d3d_color color1, d3d_color color2, bool vertical);
	void draw_outlined_gradient_rectangle(int x, int y, int w, int h, d3d_color color1, d3d_color color2, bool vertical, d3d_color outline);

	void draw_line(int x1, int y1, int x2, int y2, d3d_color color);


	__forceinline void draw_crosshair(int x, int y, int len, d3d_color color)
	{
		d3d_rect rect[] =
		{
			{ x - len, y, x + len + 1, y + 1 },
			{ x, y - len, x + 1, y + len + 1 }
		};
		overlay::device->Clear(2, rect, D3DCLEAR_TARGET, color, 1.0f, 0);
	}

	__forceinline void draw_outlined_crosshair(int x, int y, int len, d3d_color color, d3d_color outline)
	{
		d3d_rect rect[] =
		{
			{ x - len - 1, y - 1, x + len + 2, y + 2 },
			{ x - 1, y - len - 1, x + 2, y + len + 2 }
		};
		overlay::device->Clear(2, rect, D3DCLEAR_TARGET, outline, 1.0f, 0);
		drawing::draw_crosshair(x, y, len, color);
	}

	void draw_3d_line_from_array(vector<vec3>& array, d3d_color color);
	void draw_skeet_rainbow_shit(int x, int y, int w);
	void draw_circle(int x, int y, float radius, d3d_color color);
}