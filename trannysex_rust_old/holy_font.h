#pragma once

#define b(bit) (1 << bit)
#define bit_flag(value, bit) ((value == 1) ? b(bit) : 0) 
#define bit_mask(b1, b2, b3, b4, b5, b6, b7, b8) (bit_flag(b1, 0) | bit_flag(b2, 1) | bit_flag(b3, 2) | bit_flag(b4, 3) | bit_flag(b5, 4) | bit_flag(b6, 5) | bit_flag(b7, 6) | bit_flag(b8, 7))

class c_letter_8
{
public:
	uint8_t lines[8];
	uint8_t w;
	uint8_t h;

	c_letter_8(uint8_t w, uint8_t l1, uint8_t l2, uint8_t l3, uint8_t l4, uint8_t l5, uint8_t l6, uint8_t l7, uint8_t l8)
	{
		this->w = w;
		this->h = 8;
		this->lines[0] = l1;
		this->lines[1] = l2;
		this->lines[2] = l3;
		this->lines[3] = l4;
		this->lines[4] = l5;
		this->lines[5] = l6;
		this->lines[6] = l7;
		this->lines[7] = l8;
	};

	c_letter_8() {};
};


template <typename type>
class c_font
{
	type* map;
public:

	__forceinline type* convert_char_to_index(char letter)
	{
		if ((letter < '!') || (letter > 'z'))
			letter = '?';
		return &this->map[letter - '!'];
	};

	__forceinline size_t calculate_string_width(const char* string)
	{
		size_t size = 0;
		const char* string_pointer = string;
		while (*string_pointer != '\0') //strlen is for nerds
		{
			if (*string_pointer == ' ')
			{
				size += 5;
				string_pointer++;
				continue;
			}

			auto letter = this->convert_char_to_index(*string_pointer);
			size += (letter->w + 1);
			string_pointer++;
		}
		return size;
	}

	__forceinline size_t calculate_wide_string_width(const wchar_t* string)
	{
		size_t size = 0;
		const wchar_t* string_pointer = string;
		while (*string_pointer != L'\0')
		{
			if (*string_pointer == ' ')
			{
				size += 5;
				string_pointer++;
				continue;
			}

			auto letter = this->convert_char_to_index(static_cast<char>(*string_pointer));
			size += (letter->w + 1);
			string_pointer++;
		}
		return size;
	}

	void draw_string(const char* string, int _x, int _y, int _w, unsigned long flags, d3d_color color);
	void draw_wide_string(const wchar_t* string, int _x, int _y, int _w, unsigned long flags, d3d_color color);

	c_font(type* map) { this->map = map; };
	c_font() {};
};

enum font_flags : unsigned long
{
	flag_outline = (1 << 0),
	flag_centered = (1 << 1),
};

template<typename type>
inline void c_font<type>::draw_string(const char* string, int _x, int _y, int _w, unsigned long flags, d3d_color color)
{
	int x = _x;
	int y = _y;

	if (flags & flag_centered)
	{
		x += static_cast<int>(_w / 2);
		x -= static_cast<int>(this->calculate_string_width(string) / 2);
	}

	int vertex_counter = 0;
	const char* string_pointer = string;
	while (*string_pointer != '\0')
	{
		if (*string_pointer == ' ')
		{
			x += 5;
			string_pointer++;
			continue;
		}

		auto letter = this->convert_char_to_index(*string_pointer);
		if (flags & flag_outline)
		{
			for (size_t i = 0; i < letter->h; i++)
				for (size_t j = 0; j < letter->w; j++)
					if (letter->lines[i] & b(j))
					{
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j - 1), static_cast<float>(y + i), 0.0f, 1.0f, c_black };
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j + 1), static_cast<float>(y + i), 0.0f, 1.0f, c_black };
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j), static_cast<float>(y + i - 1), 0.0f, 1.0f, c_black };
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j), static_cast<float>(y + i + 1), 0.0f, 1.0f, c_black };
					}
		}

		for (size_t i = 0; i < letter->h; i++)
			for (size_t j = 0; j < letter->w; j++)
				if (letter->lines[i] & b(j))
					drawing::vertex[vertex_counter++] = { static_cast<float>(x + j), static_cast<float>(y + i), 0.0f, 1.0f, color };

		x += (letter->w + 1);
		string_pointer++;
	}
	overlay::device->DrawPrimitiveUP(D3DPT_POINTLIST, vertex_counter, drawing::vertex, sizeof(c_vertex));
}

template<typename type>
inline void c_font<type>::draw_wide_string(const wchar_t * string, int _x, int _y, int _w, unsigned long flags, d3d_color color)
{
	int x = _x;
	int y = _y;

	if (flags & flag_centered)
	{
		x += static_cast<int>(_w / 2);
		x -= static_cast<int>(this->calculate_wide_string_width(string) / 2);
	}

	int vertex_counter = 0;
	const wchar_t* string_pointer = string;
	while (*string_pointer != L'\0')
	{
		if (*string_pointer == L' ')
		{
			x += 5;
			string_pointer++;
			continue;
		}

		auto letter = this->convert_char_to_index(static_cast<char>(*string_pointer));
		if (flags & flag_outline)
		{
			for (size_t i = 0; i < letter->h; i++)
				for (size_t j = 0; j < letter->w; j++)
					if (letter->lines[i] & b(j))
					{
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j - 1), static_cast<float>(y + i), 0.0f, 1.0f, c_black };
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j + 1), static_cast<float>(y + i), 0.0f, 1.0f, c_black };
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j), static_cast<float>(y + i - 1), 0.0f, 1.0f, c_black };
						drawing::vertex[vertex_counter++] = { static_cast<float>(x + j), static_cast<float>(y + i + 1), 0.0f, 1.0f, c_black };
					}
		}

		for (size_t i = 0; i < letter->h; i++)
			for (size_t j = 0; j < letter->w; j++)
				if (letter->lines[i] & b(j))
					drawing::vertex[vertex_counter++] = { static_cast<float>(x + j), static_cast<float>(y + i), 0.0f, 1.0f, color };

		x += (letter->w + 1);
		string_pointer++;
	}
	overlay::device->DrawPrimitiveUP(D3DPT_POINTLIST, vertex_counter, drawing::vertex, sizeof(c_vertex));
}

namespace fonts
{
	extern c_font<c_letter_8> pixel_font;
}