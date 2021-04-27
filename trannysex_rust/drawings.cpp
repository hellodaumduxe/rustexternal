#include "stdafx.h"
#include "drawings.h"

c_matrix4x4 drawing::transposed_matrix;

float drawing::vp_width;
float drawing::vp_height;
ID3D11Device* drawing::device;
IDXGISwapChain* drawing::swap_chain;
ID3D11Buffer* drawing::vertex_buffer;
ID3D11BlendState* drawing::blend_state;
ID3D11PixelShader* drawing::pixel_shader;
ID3D11InputLayout* drawing::input_layout;
ID3D11VertexShader* drawing::vertex_shader;
ID3D11DeviceContext* drawing::device_context;
ID3D11RenderTargetView* drawing::render_view;

D3D11_PRIMITIVE_TOPOLOGY drawing::topology;
vector<c_drawing_ctx> drawing::triangle_ctx;
vector<c_drawing_ctx> drawing::point_ctx;
vector<c_drawing_ctx> drawing::line_ctx;
c_vertex* drawing::vertex;
int drawing::index;

void drawing::draw_rectangle(const float x, const float y, const float w, const float h, const d3d_color color)
{
	int i = 0;
	format_rectangle(x, y, x + w, y + h, color);
	drawing::add_triangle_primative(i);
}

void drawing::draw_outlined_rectangle(const float x, const float y, const float w, const float h, const d3d_color color, const d3d_color outline)
{
	drawing::draw_rectangle(x, y, w, h, outline);
	drawing::draw_rectangle(x + 1, y + 1, w - 2, h - 2, color);
}

void drawing::draw_esp_box(const float x, const float y, const float w, const float h, const d3d_color color, const d3d_color outline)
{
	const float width = (w / 5.0f);
	const float height = (h / 6.0f);

	int i = 0;
	format_rectangle(x - 1, y - 1, x + width + 2, y + 2, outline);
	format_rectangle(x - 1, y - 1, x + 2, y + height + 2, outline);
	format_rectangle(x + w - 1, y - 1, x + w + 2, y + height + 2, outline);
	format_rectangle((x + w) - width - 1, y - 1, x + w + 2, y + 2, outline);
	format_rectangle(x - 1, (y + h) - height - 1, x + 2, y + h + 2, outline);
	format_rectangle(x + w - 1, (y + h) - height - 1, x + w + 2, y + h + 2, outline);
	format_rectangle(x - 1, y + h - 1, x + width + 2, y + h + 2, outline);
	format_rectangle((x + w) - width - 1, y + h - 1, x + w + 2, y + h + 2, outline);

	format_rectangle(x, y, x + width + 1, y + 1, color);
	format_rectangle(x, y, x + 1, y + height + 1, color);
	format_rectangle((x + w) - width, y, x + w, y + 1, color);
	format_rectangle(x + w, y, x + w + 1, y + height + 1, color);
	format_rectangle(x, y + h, x + width + 1, y + h + 1, color);
	format_rectangle(x, (y + h) - height, x + 1, y + h, color);
	format_rectangle((x + w) - width, y + h, x + w + 1, y + h + 1, color);
	format_rectangle(x + w, (y + h) - height, x + w + 1, y + h, color);
	drawing::add_triangle_primative(i);
}

void drawing::draw_line(const float x1, const float y1, const float x2, const float y2, const d3d_color color)
{
	drawing::vertex[0] = { format_x_axis(x1), format_y_axis(y1), color };
	drawing::vertex[1] = { format_x_axis(x2), format_y_axis(y2), color };
	drawing::add_line_primative(2);
}

void drawing::draw_crosshair(const float x, const float y, const float l, const d3d_color color)
{
	int i = 0;
	format_rectangle(x - l, y, x + l + 1, y + 1, color);
	format_rectangle(x, y - l, x + 1, y + l + 1, color);
	drawing::add_triangle_primative(i);
}

void drawing::draw_outlined_crosshair(const float x, const float y, const float l, const d3d_color color, const d3d_color outline)
{
	int i = 0;
	format_rectangle(x - l - 1, y - 1, x + l + 2, y + 2, outline);
	format_rectangle(x - 1, y - l - 1, x + 2, y + l + 2, outline);
	format_rectangle(x - l, y, x + l + 1, y + 1, color);
	format_rectangle(x, y - l, x + 1, y + l + 1, color);
	drawing::add_triangle_primative(i);
}

void drawing::draw_gamer_crosshair(const float x, const float y)
{
	int i = 0;
	format_rectangle(x - 7, y - 1, x - 1, y + 2, c_black);
	format_rectangle(x + 2, y - 1, x + 8, y + 2, c_black);
	format_rectangle(x - 1, y + 2, x + 2, y + 8, c_black);
	format_rectangle(x - 1, y - 7, x + 2, y - 1, c_black);

	format_rectangle(x - 6, y, x - 2, y + 1, c_green);
	format_rectangle(x + 3, y, x + 7, y + 1, c_green);
	format_rectangle(x, y + 3, x + 1, y + 7, c_green);
	format_rectangle(x, y - 6, x + 1, y - 2, c_green);
	drawing::add_triangle_primative(i);
}

void drawing::draw_circle(const float x, const float y, const float q, const float r, const d3d_color color)
{
	int vertex_index = 0;
	float rotations = m_rad_360_deg / q;
	for (float rotation = 0.0f; rotation < m_rad_360_deg; rotation += rotations)
	{
		float v_x = (r * math::cosf(rotation)) + static_cast<float>(x);
		float v_y = (r * math::sinf(rotation)) + static_cast<float>(y);
		vertex[vertex_index++] = { format_x_axis(v_x), format_y_axis(v_y), color };
	}
	vertex[vertex_index++] = vertex[0];
	drawing::add_line_primative(vertex_index);
}

void drawing::draw_filled_circle(const float x, const float y, const float q, const float r, const d3d_color color)
{
	int vertex_index = 0;
	float rotations = m_rad_360_deg / q;
	c_vertex center = { format_x_axis(x), format_y_axis(y), color };
	for (float rotation = 0.0f; rotation < m_rad_360_deg; rotation += rotations)
	{
		float v_x1 = (r * math::cosf(rotation)) + static_cast<float>(x);
		float v_y1 = (r * math::sinf(rotation)) + static_cast<float>(y);
		vertex[vertex_index++] = { format_x_axis(v_x1), format_y_axis(v_y1), color };

		float v_x2 = (r * math::cosf(rotation + rotations)) + static_cast<float>(x);
		float v_y2 = (r * math::sinf(rotation + rotations)) + static_cast<float>(y);
		vertex[vertex_index++] = { format_x_axis(v_x2), format_y_axis(v_y2), color };
		vertex[vertex_index++] = center;
	}
	drawing::add_triangle_primative(vertex_index);
}

void drawing::draw_triangle(const float x, const float y, const float height, const d3d_color color)
{
	float width = (static_cast<float>(height) * 0.333f);
	vertex[0] = { x - width, y, color };
	vertex[1] = { x + width, y, color };
	vertex[2] = { x, y - height, color };
	drawing::add_triangle_primative(3);
}

void drawing::draw_3d_line_from_array(vector<vec3>* array, const d3d_color color)
{
	int vertex_index = 0;
	for (int i = 0; i < array->size(); i++)
	{
		vec3 screen;
		if (drawing::world_to_screen(array->operator[](i), &screen))
			drawing::vertex[vertex_index++] = { format_x_axis(screen.x), format_y_axis(screen.y), color };
	}
	drawing::add_line_primative(vertex_index);
}

char shader_data[] =
"struct c_vertex_out"
"{"
"	float4 color : COLOR;"
"	float4 position : SV_POSITION;"
"};"
"c_vertex_out vertex_shader(float4 color : COLOR, float4 position : POSITION)"
"{"
"	c_vertex_out vertex_out;"
"	vertex_out.position = position;"
"	vertex_out.color = color;"
"	return vertex_out;"
"}"
"float4 pixel_shader(float4 color : COLOR) : SV_TARGET"
"{"
"	return color;"
"}";
bool drawing::init(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC swap_chain_description;
	swap_chain_description.BufferDesc.Width = overlay::width;
	swap_chain_description.BufferDesc.Height = overlay::height;
	swap_chain_description.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_description.BufferDesc.RefreshRate.Denominator = 0;
	swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_description.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_description.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_description.SampleDesc.Count = 1;
	swap_chain_description.SampleDesc.Quality = 0;
	swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_description.BufferCount = 1;
	swap_chain_description.OutputWindow = hwnd;
	swap_chain_description.Windowed = true;
	swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_description.Flags = 0;
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swap_chain_description, &drawing::swap_chain, &drawing::device, nullptr, &drawing::device_context) != 0) return false;

	ID3D11Texture2D* back_buffer;
	if (drawing::swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer)) != 0) return false;
	if (drawing::device->CreateRenderTargetView(back_buffer, nullptr, &drawing::render_view) != 0) return false;
	back_buffer->Release();

	D3D11_VIEWPORT view_port = { 0, 0, static_cast<float>(overlay::width), static_cast<float>(overlay::height), 0, 1 };
	drawing::device_context->OMSetRenderTargets(1, &drawing::render_view, nullptr);
	drawing::device_context->RSSetViewports(1, &view_port);

	D3D11_BUFFER_DESC buffer_description;
	buffer_description.Usage = D3D11_USAGE_DYNAMIC;
	buffer_description.ByteWidth = (100000 * sizeof(c_vertex));
	buffer_description.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_description.MiscFlags = 0;
	if (drawing::device->CreateBuffer(&buffer_description, nullptr, &drawing::vertex_buffer) != 0) return false;

	ID3DBlob* vertex_blob;
	if (D3DCompile(shader_data, sizeof(shader_data), nullptr, nullptr, nullptr, s("vertex_shader"), s("vs_4_0"), 0, 0, &vertex_blob, nullptr) != 0) return false;
	if (drawing::device->CreateVertexShader(vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(), nullptr, &drawing::vertex_shader) != 0) return false;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ s("POSITION"), 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ s("COLOR"), 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (drawing::device->CreateInputLayout(layout, 2, vertex_blob->GetBufferPointer(), vertex_blob->GetBufferSize(), &drawing::input_layout) != 0) return false;

	ID3DBlob* pixel_blob;
	if (D3DCompile(shader_data, sizeof(shader_data), nullptr, nullptr, nullptr, s("pixel_shader"), s("ps_4_0"), 0, 0, &pixel_blob, nullptr) != 0) return false;
	if (drawing::device->CreatePixelShader(pixel_blob->GetBufferPointer(), pixel_blob->GetBufferSize(), nullptr, &drawing::pixel_shader) != 0) return false;

	D3D11_BLEND_DESC blend_state_description;
	blend_state_description.AlphaToCoverageEnable = false;
	blend_state_description.IndependentBlendEnable = false;
	blend_state_description.RenderTarget[0].BlendEnable = true;
	blend_state_description.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_state_description.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_state_description.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_state_description.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_state_description.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_state_description.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_state_description.RenderTarget[0].RenderTargetWriteMask = 15;
	if (drawing::device->CreateBlendState(&blend_state_description, &drawing::blend_state) != 0) return false;

	drawing::device_context->IASetInputLayout(drawing::input_layout);
	drawing::device_context->VSSetShader(drawing::vertex_shader, 0, 0);
	drawing::device_context->PSSetShader(drawing::pixel_shader, 0, 0);
	drawing::device_context->GSSetShader(nullptr, 0, 0);
	drawing::device_context->SetPredication(nullptr, false);

	D3D11_VIEWPORT vp;
	UINT viewport_number = 1;
	drawing::device_context->RSGetViewports(&viewport_number, &vp);
	drawing::vp_width = vp.Width;
	drawing::vp_height = vp.Height;

	drawing::topology = D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
	drawing::vertex_buffer->SetEvictionPriority(DXGI_RESOURCE_PRIORITY_MAXIMUM);

	float tmp[] = { 0.0f,0.0f,0.0f,0.0f };
	device_context->OMSetBlendState(blend_state, tmp, 0xffffffff);

	UINT offset = 0;
	UINT stride = sizeof(c_vertex);
	drawing::device_context->IASetVertexBuffers(0, 1, &drawing::vertex_buffer, &stride, &offset);

	drawing::triangle_ctx.allocate(100000);
	drawing::point_ctx.allocate(100000);
	drawing::line_ctx.allocate(100000);
	drawing::vertex = nullptr;

	memset(shader_data, 0, sizeof(shader_data));
	return true;
}