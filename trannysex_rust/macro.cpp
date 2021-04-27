#include "stdafx.h"

int macro::seed = 0;
void macro::create_thread(void* start_address)
{
	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(start_address), nullptr, 0, nullptr);
}

void macro::create_thread_fake_start(void* start_address)
{
	HMODULE module = (macro::rng() % 2) ? GetModuleHandleA(s("wmp.dll")) : GetModuleHandleA(0);
	void* address = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(module) + 0x1000) + (macro::rng() % 0x20000));

	unsigned char hook[] =
	{
		0xff, 0x25, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(hook) + 6) = reinterpret_cast<uintptr_t>(start_address);

	DWORD old;
	VirtualProtect(address, 5, PAGE_EXECUTE_READWRITE, &old);

	uint8_t backup[14];
	memcpy(backup, address, sizeof(hook));
	memcpy(address, hook, sizeof(hook));

	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), nullptr, 0, nullptr);
	Sleep(750);

	memcpy(address, backup, sizeof(hook));
	VirtualProtect(address, 5, old, &old);
}

char buffer[1024];
void macro::print_message(const char* str, ...)
{
	va_list list;
	va_start(list, str);
	wvsprintfA(buffer, str, list);

	DWORD ret;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), buffer, static_cast<DWORD>(strlen(buffer)), &ret, nullptr);
	va_end(list);
}

void macro::print_to_buffer(char* buffer, const char* str, ...)
{
	va_list list;
	va_start(list, str);
	wvsprintfA(buffer, str, list);
	va_end(list);
}