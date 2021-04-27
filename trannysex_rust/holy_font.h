#pragma once

#define b(bit) (1 << bit)
#define bit_flag(value, bit) ((value == 1) ? b(bit) : 0) 
#define bit_mask(b1, b2, b3, b4, b5, b6, b7, b8) (bit_flag(b1, 0) | bit_flag(b2, 1) | bit_flag(b3, 2) | bit_flag(b4, 3) | bit_flag(b5, 4) | bit_flag(b6, 5) | bit_flag(b7, 6) | bit_flag(b8, 7))
#define bit_mask_16(b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16) (bit_flag(b1, 0) | bit_flag(b2, 1) | bit_flag(b3, 2) | bit_flag(b4, 3) | bit_flag(b5, 4) | bit_flag(b6, 5) | bit_flag(b7, 6) | bit_flag(b8, 7) | bit_flag(b9, 8) | bit_flag(b10, 9) |  bit_flag(b11, 10) | bit_flag(b12, 11) | bit_flag(b13, 12) | bit_flag(b14, 13) | bit_flag(b15, 14) | bit_flag(b16, 15))
enum font_flags : unsigned long
{
	flag_outline = (1 << 0),
	flag_centered = (1 << 1),
};

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

class c_letter_16
{
public:
	uint16_t lines[16];
	uint8_t w;
	uint8_t h;

	c_letter_16(uint8_t w, uint16_t l1, uint16_t l2, uint16_t l3, uint16_t l4, uint16_t l5, uint16_t l6, uint16_t l7, uint16_t l8, uint16_t l9, uint16_t l10, uint16_t l11, uint16_t l12, uint16_t l13, uint16_t l14, uint16_t l15, uint16_t l16)
	{
		this->w = w;
		this->h = 16;
		this->lines[0] = l1;
		this->lines[1] = l2;
		this->lines[2] = l3;
		this->lines[3] = l4;
		this->lines[4] = l5;
		this->lines[5] = l6;
		this->lines[6] = l7;
		this->lines[7] = l8;
		this->lines[8] = l9;
		this->lines[9] = l10;
		this->lines[10] = l11;
		this->lines[11] = l12;
		this->lines[12] = l13;
		this->lines[13] = l14;
		this->lines[14] = l15;
		this->lines[15] = l16;
	};
	c_letter_16() {};
};

namespace holy_font
{
	template<typename map_type>
	inline const map_type* convert_char_to_index(const map_type* map, char letter)
	{
		if ((letter < '!') || (letter > 'z'))
			letter = '?';
		return &map[letter - '!'];
	};

	template<typename map_type>
	inline size_t calculate_string_width(const map_type* map, const char* string)
	{
		size_t size = 0;
		const char* string_pointer = string;
		do
		{
			size += ((*string_pointer == ' ') ? 5 : (holy_font::convert_char_to_index(map, *string_pointer)->w + 1));
			string_pointer++;
		} while (*string_pointer != '\0');
		return size;
	}

	template<typename map_type>
	inline size_t calculate_wide_string_width(const map_type* map, const wchar_t* string)
	{
		size_t size = 0;
		const wchar_t* string_pointer = string;
		do
		{
			size += ((*string_pointer == L' ') ? 5 : (holy_font::convert_char_to_index(map, static_cast<char>(*string_pointer))->w + 1));
			string_pointer++;
		} while (*string_pointer != L'\0');
		return size;
	}

	template<typename map_type>
	inline void draw_string(const map_type* map, const char* string, const float _x, const float y, const float w, const unsigned long flags, const d3d_color color)
	{
		float x = _x;
		if (flags & flag_centered)
			x += ((w / 2.0f) - static_cast<float>(holy_font::calculate_string_width(map, string) / 2));

		int vertex_counter = 0;
		const char* string_pointer = string;
		do
		{
			if (*string_pointer == ' ')
				x += 5;
			else
			{
				auto letter = holy_font::convert_char_to_index(map, *string_pointer);
				if (flags & flag_outline)
					for (size_t i = 0; i < letter->h; i++)
						for (size_t j = 0; j < letter->w; j++)
							if (letter->lines[i] & b(j))
							{
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j + 1), format_y_axis(y + i), c_black };
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j - 1), format_y_axis(y + i), c_black };
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j), format_y_axis(y + i + 1), c_black };
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j), format_y_axis(y + i - 1), c_black };
							}

				for (size_t i = 0; i < letter->h; i++)
					for (size_t j = 0; j < letter->w; j++)
						if (letter->lines[i] & b(j))
							drawing::vertex[vertex_counter++] = { format_x_axis(x + j), format_y_axis(y + i), color };

				x += (letter->w + 1);
			}
			string_pointer++;
		} while (*string_pointer != '\0');
		drawing::add_point_primative(vertex_counter);
	}

	template<typename map_type>
	inline void draw_wide_string(const map_type* map, const wchar_t* string, const float _x, const float y, const float w, const unsigned long flags, const d3d_color color)
	{
		float x = _x;
		if (flags & flag_centered)
			x += ((w / 2.0f) - static_cast<float>(holy_font::calculate_wide_string_width(map, string) / 2));

		int vertex_counter = 0;
		const wchar_t* string_pointer = string;
		do
		{
			if (*string_pointer == L' ')
				x += 5;
			else
			{
				auto letter = holy_font::convert_char_to_index(map, static_cast<char>(*string_pointer));
				if (flags & flag_outline)
					for (size_t i = 0; i < letter->h; i++)
						for (size_t j = 0; j < letter->w; j++)
							if (letter->lines[i] & b(j))
							{
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j + 1), format_y_axis(y + i), c_black };
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j - 1), format_y_axis(y + i), c_black };
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j), format_y_axis(y + i + 1), c_black };
								drawing::vertex[vertex_counter++] = { format_x_axis(x + j), format_y_axis(y + i - 1), c_black };
							}

				for (size_t i = 0; i < letter->h; i++)
					for (size_t j = 0; j < letter->w; j++)
						if (letter->lines[i] & b(j))
							drawing::vertex[vertex_counter++] = { format_x_axis(x + j), format_y_axis(y + i), color };

				x += (letter->w + 1);
			}
			string_pointer++;
		} while (*string_pointer != L'\0');
		drawing::add_point_primative(vertex_counter);
	}

	extern c_letter_8 pixel_font[];
	extern c_letter_16 arial_small[];
}