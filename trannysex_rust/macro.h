#pragma once

namespace macro
{
	inline bool wcscmp(const wchar_t* string1, const wchar_t* string2)
	{
		auto to_lower = [=](wchar_t c)->wchar_t
		{
			if ((c < 'A') || (c > 'Z'))
				return c;
			return (c + ' ');
		};

		const wchar_t* string1_pointer = string1;
		const wchar_t* string2_pointer = string2;

		uint32_t size = 0;
		uint32_t match = 0;
		do
		{
			if (to_lower(*string1_pointer) == to_lower(*string2_pointer))
				match++;

			string1_pointer++;
			string2_pointer++;
			size++;
		} while ((*string1_pointer != L'\0') && (*string2_pointer != L'\0'));
		return (match == size);
	}

	inline char* strstr(const char* str1, const char* str2)
	{
		const char* p1 = str1;
		const char* p2 = str2;
		const char* r = *p2 == 0 ? str1 : 0;

		while (*p1 != 0 && *p2 != 0)
		{
			if ((unsigned char)*p1 == (unsigned char)*p2)
			{
				if (r == 0)
				{
					r = p1;
				}

				p2++;
			}
			else
			{
				p2 = str2;
				if (r != 0)
				{
					p1 = r + 1;
				}

				if ((unsigned char)*p1 == (unsigned char)*p2)
				{
					r = p1;
					p2++;
				}
				else
				{
					r = 0;
				}
			}

			p1++;
		}

		return *p2 == 0 ? (char*)r : 0;
	}

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

	inline bool is_valid_address(uintptr_t address) { return ((address > 0x10000) && (address < 0x7FFFFFFEFFFF)); }

	extern int seed;
	inline int rng() { macro::seed = ((macro::seed * 1103515245 + 12345) & 0x7fffffff); return macro::seed; };

	void create_thread(void* start_address);
	void create_thread_fake_start(void* start_address);
	void print_message(const char* str, ...);
	void print_to_buffer(char* buffer, const char* str, ...);
}

#define m_pi 3.141592653589793238462f
#define m_half_pi 1.570796f
#define m_quarter_pi 0.7853982f
#define m_three_half_pi 4.7123889f
#define m_inv_two_pi 0.1591549f
#define m_two_pi 6.283185f

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

#define array_size(arr) (sizeof(arr) / sizeof(decltype(arr)))
#define make_get(name, type, offset) __forceinline type##* get_##name##() { return reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + offset); };
#define pointer_validity_check(p) if (!macro::is_valid_address(p)) return;