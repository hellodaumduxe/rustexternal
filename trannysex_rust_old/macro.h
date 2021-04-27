#pragma once

namespace macro
{
	__forceinline int hash(char const* key) //hash meme 
	{
		uint32_t hash, i;
		for (hash = i = 0; i < strlen(key); ++i)
		{
			hash += key[i];
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}

	char* strstr(const char* str1, const char* str2);

	void print_to_buffer(char* buffer, const char* str, ...);
	void print(const char* str, ...);
	void system(const char* buffer);
}

#define m_pi 3.141592653589793238462f
#define m_half_pi 1.570796f
#define m_quarter_pi 0.7853982f
#define m_three_half_pi 4.7123889f
#define invtwopi 0.1591549f
#define two_pi 6.283185f

#define m_euler 2.7182818284590452353602874713527f

#define m_rad_pi 57.295779513082f
#define m_pi_rad 0.01745329251f

#define m_rad_360_deg 6.283185f

#define positive(n) ((n) < 0 ? 0 - (n) : (n))

#define concat2(a, b) a ## b
#define concat(a, b) concat2(a, b)

#define deg_to_rad(deg) ( deg * m_pi ) / 180.0f
#define rad_to_deg(rad) ( rad * 180.0f ) / m_pi
#define grd_to_bog(grd) ( m_pi / 180.0f ) * grd

#define sizeof_array(arr, type) (sizeof(arr) / sizeof(type))

#define d3d_color_argb(a, r, g, b) D3DCOLOR_ARGB(a, r, g, b)
#define c_white d3d_color_argb(255, 255, 255, 255)
#define c_red d3d_color_argb(255, 255, 000, 000)
#define c_green d3d_color_argb(255, 000, 255, 000)
#define c_blue d3d_color_argb(255, 000, 000, 255) 
#define c_black d3d_color_argb(255, 1, 1, 1)
#define c_purple d3d_color_argb(255, 125, 000, 255) 
#define c_grey d3d_color_argb(255, 128, 128, 128) 
#define c_yellow d3d_color_argb(255, 255, 255, 000) 
#define c_orange d3d_color_argb(255, 255, 125, 000)
#define c_pink d3d_color_argb(255, 255, 050, 250)
#define c_cyan d3d_color_argb(255, 000, 255, 255)
#define c_brown d3d_color_argb(255, 77, 46, 38)
#define c_light_grey d3d_color_argb(255, 211, 211, 211)

#define make_get(name, type, offset) __forceinline type##* get_##name##() { return reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); };

#define b_64(bit) (1i64 << bit)
#define bit_mask_64(b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17, b18, b19, b20, b21, b22, b23, b24, b25, b26, b27, b28, b29, b30, b31, b32, b33, b34, b35, b36, b37, b38, b39, b40, b41, b42, b43, b44, b45, b46, b47, b48, b49, b50, b51, b52, b53, b54, b55, b56, b57, b58, b59, b60, b61, b62, b63, b64) (((b1 == 1) ? b_64(0) : 0) |((b2 == 1) ? b_64(1) : 0) |((b3 == 1) ? b_64(2) : 0) |((b4 == 1) ? b_64(3) : 0) |((b5 == 1) ? b_64(4) : 0) |((b6 == 1) ? b_64(5) : 0) |((b7 == 1) ? b_64(6) : 0) |((b8 == 1) ? b_64(7) : 0) |((b9 == 1) ? b_64(8) : 0) |((b10 == 1) ? b_64(9) : 0) |((b11 == 1) ? b_64(10) : 0) |((b12 == 1) ? b_64(11) : 0) |((b13 == 1) ? b_64(12) : 0) |((b14 == 1) ? b_64(13) : 0) |((b15 == 1) ? b_64(14) : 0) |((b16 == 1) ? b_64(15) : 0) |((b17 == 1) ? b_64(16) : 0) |((b18 == 1) ? b_64(17) : 0) |((b19 == 1) ? b_64(18) : 0) |((b20 == 1) ? b_64(19) : 0) |((b21 == 1) ? b_64(20) : 0) |((b22 == 1) ? b_64(21) : 0) |((b23 == 1) ? b_64(22) : 0) |((b24 == 1) ? b_64(23) : 0) |((b25 == 1) ? b_64(24) : 0) |((b26 == 1) ? b_64(25) : 0) |((b27 == 1) ? b_64(26) : 0) |((b28 == 1) ? b_64(27) : 0) |((b29 == 1) ? b_64(28) : 0) |((b30 == 1) ? b_64(29) : 0) |((b31 == 1) ? b_64(30) : 0) |((b32 == 1) ? b_64(31) : 0) |((b33 == 1) ? b_64(32) : 0) |((b34 == 1) ? b_64(33) : 0) |((b35 == 1) ? b_64(34) : 0) |((b36 == 1) ? b_64(35) : 0) |((b37 == 1) ? b_64(36) : 0) |((b38 == 1) ? b_64(37) : 0) |((b39 == 1) ? b_64(38) : 0) |((b40 == 1) ? b_64(39) : 0) |((b41 == 1) ? b_64(40) : 0) |((b42 == 1) ? b_64(41) : 0) |((b43 == 1) ? b_64(42) : 0) |((b44 == 1) ? b_64(43) : 0) |((b45 == 1) ? b_64(44) : 0) |((b46 == 1) ? b_64(45) : 0) |((b47 == 1) ? b_64(46) : 0) |((b48 == 1) ? b_64(47) : 0) |((b49 == 1) ? b_64(48) : 0) |((b50 == 1) ? b_64(49) : 0) |((b51 == 1) ? b_64(50) : 0) |((b52 == 1) ? b_64(51) : 0) |((b53 == 1) ? b_64(52) : 0) |((b54 == 1) ? b_64(53) : 0) |((b55 == 1) ? b_64(54) : 0) |((b56 == 1) ? b_64(55) : 0) |((b57 == 1) ? b_64(56) : 0) |((b58 == 1) ? b_64(57) : 0) |((b59 == 1) ? b_64(58) : 0) |((b60 == 1) ? b_64(59) : 0) |((b61 == 1) ? b_64(60) : 0) |((b62 == 1) ? b_64(61) : 0) |((b63 == 1) ? b_64(62) : 0) |((b64 == 1) ? b_64(63) : 0))
