#include "stdafx.h"

#pragma function(memset)
void* __cdecl memset(void* addr, int value, size_t size)
{
	for (size_t i = 0; i < size; i++)
		*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(addr) + i) = static_cast<uint8_t>(value);
	return addr;
}

#pragma function(memcpy)
void* memcpy(void* dest, const void* src, size_t count)
{
	uint8_t* dest_arr = reinterpret_cast<uint8_t*>(dest);
	const uint8_t* src_arr = reinterpret_cast<const uint8_t*>(src);

	for (size_t i = 0; i < count; i++)
		dest_arr[i] = src_arr[i];

	return dest;
}