#include "stdafx.h"

extern "C" int _fltused = 0;

#pragma function(memset)
void* memset(void* __restrict dest, const int value, const size_t size)
{
	size_t remaining = size;
	uint8_t* pointer = reinterpret_cast<uint8_t*>(dest);
	while (remaining-- > 0)
		*pointer++ = static_cast<uint8_t>(value);
	return dest;
}

#pragma function(memcpy)
void* memcpy(void* __restrict dest, const void* __restrict src, size_t count) {
	uint8_t* dest_arr = reinterpret_cast<uint8_t*>(dest);
	const uint8_t* src_arr = reinterpret_cast<const uint8_t*>(src);
	for (size_t i = 0; i < count; i++)
		dest_arr[i] = src_arr[i];
	return dest;
}