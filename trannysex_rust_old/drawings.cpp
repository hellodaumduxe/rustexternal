#include "stdafx.h"

c_vertex drawing::vertex[vertex_size];
c_rect font_rect;

void drawing::draw_rectangle(int x, int y, int w, int h, d3d_color color)
{
	d3d_rect rect[] = { { x, y, x + w, y + h } };
	overlay::device->Clear(1, rect, D3DCLEAR_TARGET, color, 1.f, 0);
}

void drawing::draw_outlined_rectangle(int x, int y, int w, int h, d3d_color color, d3d_color outline)
{
	drawing::draw_rectangle(x - 1, y - 1, w + 2, h + 2, outline);
	drawing::draw_rectangle(x, y, w, h, color);
}

void drawing::draw_hollow_rectangle(int x, int y, int w, int h, d3d_color color)
{
	d3d_rect rect[] =
	{
	{ x, y, x + w, y + 1 },
	{ x + w, y, x + w + 1, y + h },
	{ x, y + h, x + w + 1, y + h + 1 },
	{ x, y, x + 1, y + h },
	};
	overlay::device->Clear(4, rect, D3DCLEAR_TARGET, color, 1.f, 0);
}

void drawing::draw_outlined_hollow_rectangle(int x, int y, int w, int h, d3d_color color, d3d_color outline)
{
	int width = (w / 5);
	int height = (h / 6);

	d3d_rect outline_rect[] =
	{
	{ x - 1,               y - 1,                x + width + 2, y + 2 },
	{ x - 1,               y - 1,                x + 2,         y + height + 2 },
	{ x + w - 1,           y - 1,                x + w + 2,     y + height + 2 },
	{ (x + w) - width - 1, y - 1,                x + w + 2,     y + 2 },
	{ x - 1,               (y + h) - height - 1, x + 2,         y + h + 2 },
	{ x + w - 1,           (y + h) - height - 1, x + w + 2,     y + h + 2 },
	{ x - 1,               y + h - 1,            x + width + 2, y + h + 2 },
	{ (x + w) - width - 1, y + h - 1,            x + w + 2,     y + h + 2 },
	};
	overlay::device->Clear(8, outline_rect, D3DCLEAR_TARGET, outline, 1.0f, 0);

	d3d_rect rect[] =
	{
	{ x,               y,                x + width + 1, y + 1 }, //upper left - across
	{ x,               y,                x + 1,     y + height + 1 }, //upper left - down
	{ (x + w) - width, y,                x + w,     y + 1 }, //upper right - across
	{ x + w,           y,                x + w + 1, y + height + 1 }, //upper right - down
	{ x,               y + h,            x + width + 1, y + h + 1 }, //lower left - across
	{ x,               (y + h) - height, x + 1,     y + h }, //lower left - down
	{ (x + w) - width, y + h,            x + w + 1,     y + h + 1 }, //lower right - across
	{ x + w,           (y + h) - height, x + w + 1, y + h }, //lower right - down
	};
	overlay::device->Clear(8, rect, D3DCLEAR_TARGET, color, 1.0f, 0);
}

void drawing::draw_gradient_rectangle(int x, int y, int w, int h, d3d_color color1, d3d_color color2, bool vertical)
{
	vertex[0] = { static_cast<float>(x), static_cast<float>(y), 0.0f, 1.0f, color1 };
	vertex[1] = { static_cast<float>(x + w), static_cast<float>(y), 0.0f, 1.0f, vertical ? color1 : color2 };
	vertex[2] = { static_cast<float>(x), static_cast<float>(y + h), 0.0f, 1.0f, vertical ? color2 : color1 };
	vertex[3] = { static_cast<float>(x + w), static_cast<float>(y + h), 0.0f, 1.0f, color2 };

	overlay::device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(c_vertex));
}

void drawing::draw_outlined_gradient_rectangle(int x, int y, int w, int h, d3d_color color1, d3d_color color2, bool vertical, d3d_color outline)
{
	drawing::draw_rectangle(x - 1, y - 1, w + 2, h + 2, outline);
	drawing::draw_gradient_rectangle(x, y, w, h, color1, color2, vertical);
}

void drawing::draw_line(int x1, int y1, int x2, int y2, d3d_color color)
{
	vertex[0] = { static_cast<float>(x1), static_cast<float>(y1), 0.0f, 1.0f, color };
	vertex[1] = { static_cast<float>(x2), static_cast<float>(y2), 0.0f, 1.0f, color };

	overlay::device->DrawPrimitiveUP(D3DPT_LINELIST, 2, vertex, sizeof(c_vertex));
}

void drawing::draw_3d_line_from_array(vector<vec3>& array, d3d_color color)
{
	int vertex_index = 0;
	for (int i = 0; i < array.size(); i++)
	{
		vec3 screen_pos;
		if (visuals::world_to_screen(array[i], &screen_pos))
		{
			vertex[vertex_index] = { screen_pos.x, screen_pos.y, 0.0f, 1.0f, color };
			vertex_index++;
		}
	}
	vertex_index--;
	if (vertex_index > 0)
		overlay::device->DrawPrimitiveUP(D3DPT_LINESTRIP, vertex_index, vertex, sizeof(c_vertex));
}

void drawing::draw_skeet_rainbow_shit(int x, int y, int w)
{
	d3d_color color1 = d3d_color_argb(255, 59, 176, 223);
	d3d_color color2 = d3d_color_argb(255, 203, 70, 206);
	d3d_color color3 = d3d_color_argb(255, 202, 227, 58);

	d3d_color bcolor1 = d3d_color_argb(255, 28, 84, 107);
	d3d_color bcolor2 = d3d_color_argb(255, 97, 33, 98);
	d3d_color bcolor3 = d3d_color_argb(255, 96, 108, 27);

	float gradient_nigger = static_cast<float>(w) / 2.0f;
	vertex[0] = { static_cast<float>(x), static_cast<float>(y), 0.0f, 1.0f, color1 };
	vertex[1] = { static_cast<float>(x) + gradient_nigger, static_cast<float>(y), 0.0f, 1.0f, color2 };

	vertex[2] = { static_cast<float>(x) + gradient_nigger, static_cast<float>(y), 0.0f, 1.0f, color2 };
	vertex[3] = { static_cast<float>(x + w), static_cast<float>(y), 0.0f, 1.0f, color3 };

	vertex[4] = { static_cast<float>(x), static_cast<float>(y + 1), 0.0f, 1.0f, bcolor1 };
	vertex[5] = { static_cast<float>(x) + gradient_nigger, static_cast<float>(y + 1), 0.0f, 1.0f, bcolor2 };

	vertex[6] = { static_cast<float>(x) + gradient_nigger, static_cast<float>(y + 1), 0.0f, 1.0f, bcolor2 };
	vertex[7] = { static_cast<float>(x + w), static_cast<float>(y + 1), 0.0f, 1.0f, bcolor3 };

	overlay::device->DrawPrimitiveUP(D3DPT_LINELIST, 8, vertex, sizeof(c_vertex));
}

void drawing::draw_circle(int x, int y, float radius, d3d_color color)
{
	int vertex_index = 0;
	float rotations = m_rad_360_deg / 50.0f;

	for (float rotation = 0.0f; rotation < m_rad_360_deg; rotation += rotations)
	{
		float vertex_x = (radius * math::cos(rotation)) + static_cast<float>(x);
		float vertex_y = (radius * math::sin(rotation)) + static_cast<float>(y);

		vertex[vertex_index] = { vertex_x, vertex_y, 0.0f, 1.0f, color };
		vertex_index++;
	}

	vertex[vertex_index] = vertex[0];
	overlay::device->DrawPrimitiveUP(D3DPT_LINESTRIP, vertex_index, vertex, sizeof(c_vertex));
}
