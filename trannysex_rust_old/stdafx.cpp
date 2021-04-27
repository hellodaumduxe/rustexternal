#include "stdafx.h"

extern "C"
{
	int _fltused = 0;
}

#pragma function(memset)
void * __cdecl memset(void *pTarget, int value, size_t cbTarget) {
	char *p = reinterpret_cast<char *>(pTarget);
	while (cbTarget-- > 0) {
		*p++ = static_cast<char>(value);
	}
	return pTarget;
}

#pragma function(memcpy)
void* memcpy(void* dest, const void* src, size_t count)
{
	uint8_t* dest_arr = reinterpret_cast<uint8_t*>(dest);
	const uint8_t* src_arr = reinterpret_cast<const uint8_t*>(src);

	for (size_t i = 0; i < count; i++)
	{
		dest_arr[i] = src_arr[i];
	}

	return dest;
}