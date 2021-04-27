#include "stdafx.h"

size_t memory::image_size(void* image)
{
	IMAGE_DOS_HEADER* dos_headers = reinterpret_cast<IMAGE_DOS_HEADER*>(image);
	IMAGE_NT_HEADERS64* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS64*>(reinterpret_cast<uintptr_t>(image) + dos_headers->e_lfanew);
	return nt_headers->OptionalHeader.SizeOfImage;
}

uintptr_t memory::find_free_space(const void* module, const size_t module_size, const size_t size)
{
	uintptr_t address = reinterpret_cast<uintptr_t>(module);
	uintptr_t end = (address + module_size) - size;
	do
	{
		size_t s = 0;
		for (size_t t = 0; t <= size; t++)
			if (*reinterpret_cast<uint8_t*>(address + t) == 0)
				s++;

		if (s >= size)
			break;
	} while (address++ < end);
	return address;
}

void* memory::find_module(const wchar_t* name)
{
	LIST_ENTRY* begin = PsLoadedModuleList->InLoadOrderLinks.Flink;
	LIST_ENTRY* entry = begin;
	do
	{
		KLDR_DATA_TABLE_ENTRY* module = CONTAINING_RECORD(entry, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if ((module->BaseDllName.Buffer != nullptr) && macro::wcscmp(module->BaseDllName.Buffer, name))
			return module->DllBase;

		entry = entry->Flink;
	} while (entry != begin);
	return nullptr;
}

#define in_range(x, a, b) ((x >= a) && (x <= b))
#define get_bits(x) (in_range((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (in_range(x,'0','9') ? x - '0' : 0))
#define get_byte(x) (get_bits(x[0]) << 4 | get_bits(x[1]))
void* memory::pattern_scan(const void* module, const char* pattern)
{
	uintptr_t image = reinterpret_cast<uintptr_t>(module);
	size_t size = memory::image_size(const_cast<void*>(module));
	const char* end = (pattern + strlen(pattern));

	for (size_t address = image; address < (image + size); address++)
	{
		const char* p = pattern;
		size_t m = 0;
		size_t s = 0;
		do
		{
			if (*p == '?')
				m++;
			else
			{
				if (*reinterpret_cast<uint8_t*>(address + s) == get_byte(p))
					m++;
				p++;
			}

			p += 2;
			s++;
		} while (p < end);

		if (m >= s)
			return reinterpret_cast<void*>(address);
	}
	kprintf("[-][pattern_scan] unable to find pattern \"%s\" \n", pattern);
	return nullptr;
}