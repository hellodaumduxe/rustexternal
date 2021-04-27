#pragma once

struct d3d_color
{
	uint8_t r, g, b, a;

	d3d_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { this->r = r; this->g = g; this->b = b; this->a = a; }
	d3d_color() {}
};
struct c_vertex
{
	float x, y;
	d3d_color color;
};
struct c_drawing_ctx
{
	uint32_t start;
	uint32_t size;

	c_drawing_ctx(uint32_t start, uint32_t size) { this->start = start; this->size = size; }
	c_drawing_ctx() {}
};

#define c_white d3d_color(255, 255, 255, 255)
#define c_red d3d_color(255, 000, 000, 255)
#define c_green d3d_color(000, 255, 000, 255)
#define c_blue d3d_color(000, 000, 255, 255) 
#define c_black d3d_color(1, 1, 1, 255)
#define c_purple d3d_color(125, 000, 255, 255) 
#define c_grey d3d_color(128, 128, 128, 255) 
#define c_yellow d3d_color(255, 255, 000, 255) 
#define c_orange d3d_color(255, 125, 000, 255)
#define c_pink d3d_color(255, 050, 250, 255)
#define c_cyan d3d_color(000, 255, 255, 255)
#define c_brown d3d_color(77, 46, 38, 255)
#define c_light_grey d3d_color(211, 211, 211, 255)

#define format_x_axis(x) ((2.0f * (((x) - 0.5f) / drawing::vp_width)) - 1.0f)
#define format_y_axis(y) (1.0f - (2.0f * (((y) - 0.5f) / drawing::vp_height)))
#define format_rectangle(x, y, x2, y2, c) \
drawing::vertex[i++] = { format_x_axis(x2), format_y_axis(y), c }; \
drawing::vertex[i++] = { format_x_axis(x2), format_y_axis(y2), c }; \
drawing::vertex[i++] = { format_x_axis(x),  format_y_axis(y2), c }; \
drawing::vertex[i++] = { format_x_axis(x),  format_y_axis(y), c }; \
drawing::vertex[i++] = { format_x_axis(x2), format_y_axis(y), c }; \
drawing::vertex[i++] = { format_x_axis(x),  format_y_axis(y2), c }

#define draw_primative(t, l) \
if (l.size() > 0) \
{ \
	drawing::device_context->IASetPrimitiveTopology(t); \
	for (size_t i = 0; i < l.size(); i++) \
	{ \
		c_drawing_ctx* ctx = &l[i]; \
		drawing::device_context->Draw(ctx->size, ctx->start); \
	} \
}

namespace drawing
{
	extern c_matrix4x4 transposed_matrix;
	inline bool world_to_screen(vec3 in, vec3* out)
	{
		vec3 translation_vector = { drawing::transposed_matrix._41, drawing::transposed_matrix._42, drawing::transposed_matrix._43 };
		vec3 up = { drawing::transposed_matrix._21, drawing::transposed_matrix._22, drawing::transposed_matrix._23 };
		vec3 right = { drawing::transposed_matrix._11, drawing::transposed_matrix._12, drawing::transposed_matrix._13 };

		float w = translation_vector.dot(in) + drawing::transposed_matrix._44;
		if (w < 0.098f)
			return false;

		float x = in.dot(right) + drawing::transposed_matrix._14;
		float y = in.dot(up) + drawing::transposed_matrix._24;
		out->x = overlay::screen_center.x * (1.0f + x / w);
		out->y = overlay::screen_center.y * (1.0f - y / w);
		return true;
	}

	extern float vp_width;
	extern float vp_height;
	extern ID3D11Device* device;
	extern IDXGISwapChain* swap_chain;
	extern ID3D11Buffer* vertex_buffer;
	extern ID3D11BlendState* blend_state;
	extern ID3D11PixelShader* pixel_shader;
	extern ID3D11InputLayout* input_layout;
	extern ID3D11VertexShader* vertex_shader;
	extern ID3D11DeviceContext* device_context;
	extern ID3D11RenderTargetView* render_view;

	extern D3D11_PRIMITIVE_TOPOLOGY topology;
	extern vector<c_drawing_ctx> triangle_ctx;
	extern vector<c_drawing_ctx> point_ctx;
	extern vector<c_drawing_ctx> line_ctx;
	extern c_vertex* vertex;
	extern int index;

	inline void begin_frame()
	{
		const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		drawing::device_context->ClearRenderTargetView(render_view, clear_color);

		D3D11_MAPPED_SUBRESOURCE ms;
		drawing::device_context->Map(drawing::vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
		drawing::vertex = reinterpret_cast<c_vertex*>(ms.pData);
		drawing::index = 0;

		drawing::triangle_ctx.clear();
		drawing::point_ctx.clear();
		drawing::line_ctx.clear();
	}
	inline void end_frame()
	{
		drawing::device_context->Unmap(drawing::vertex_buffer, 0);
		draw_primative(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, drawing::line_ctx);
		draw_primative(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, drawing::triangle_ctx);
		draw_primative(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, drawing::point_ctx);
		drawing::topology = D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
		drawing::swap_chain->Present(0, 0);
	}

	inline void add_primative(unsigned int size) { drawing::vertex = reinterpret_cast<c_vertex*>(reinterpret_cast<uintptr_t>(drawing::vertex) + (size * sizeof(c_vertex))); drawing::index += size; }
	inline void add_point_primative(unsigned int size)
	{
		if (drawing::topology != D3D11_PRIMITIVE_TOPOLOGY_POINTLIST)
			drawing::point_ctx.push_back(c_drawing_ctx(drawing::index, size));
		else
			drawing::point_ctx[drawing::point_ctx.size() - 1].size += size;
		drawing::topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		add_primative(size);
	}
	inline void add_line_primative(unsigned int size)
	{
		drawing::line_ctx.push_back(c_drawing_ctx(drawing::index, size)); 
		drawing::topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		add_primative(size);
	}
	inline void add_triangle_primative(unsigned int size)
	{
		if (drawing::topology != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
			drawing::triangle_ctx.push_back(c_drawing_ctx(drawing::index, size));
		else
			drawing::triangle_ctx[drawing::triangle_ctx.size() - 1].size += size;
		drawing::topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		add_primative(size);
	}

	void draw_rectangle(const float x, const float y, const float w, const float h, const d3d_color color);
	void draw_outlined_rectangle(const float x, const float y, const float w, const float h, const d3d_color color, const d3d_color outline);
	void draw_esp_box(const float _x, const float _y, const float w, const float h, const d3d_color color, const d3d_color outline);
	void draw_line(const float x1, const float y1, const float x2, const float y2, const d3d_color color);
	void draw_crosshair(const float x, const float y, const float l, const d3d_color color);
	void draw_outlined_crosshair(const float x, const float y, const float l, const d3d_color color, const d3d_color outline);
	void draw_gamer_crosshair(const float x, const float y);
	void draw_circle(const float x, const float y, const float q, const float r, const d3d_color color);
	void draw_filled_circle(const float x, const float y, const float q, const float r, const d3d_color color);
	void draw_triangle(const float x, const float y, const float height, const d3d_color color);
	void draw_3d_line_from_array(vector<vec3>* array, const d3d_color color);

	bool init(HWND hwnd);
}