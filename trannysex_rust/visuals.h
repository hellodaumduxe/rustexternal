#pragma once

namespace visuals
{
	inline void draw_skeleton(c_entity* entity, d3d_color color);
	void draw_radar();

	void on_render();
	void draw_watermark();
	void init();
};