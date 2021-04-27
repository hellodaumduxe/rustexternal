#pragma once

namespace macro
{
	inline wchar_t to_lower(wchar_t c)
	{
		if ((c < 'A') || (c > 'Z'))
			return c;
		return (c + ' ');
	}
	inline bool wcscmp(const wchar_t* string1, const wchar_t* string2)
	{
		const wchar_t* string1_pointer = string1;
		const wchar_t* string2_pointer = string2;

		uint32_t size = 0;
		uint32_t match = 0;
		do
		{
			if (macro::to_lower(*string1_pointer) == macro::to_lower(*string2_pointer))
				match++;

			string1_pointer++;
			string2_pointer++;
			size++;
		} while ((*string1_pointer != L'\0') && (*string2_pointer != L'\0'));
		return (match == size);
	}

	inline void sleep(uint32_t ms)
	{
		LARGE_INTEGER delay;
		delay.HighPart = 0xffffffff;
		delay.LowPart = ms * -10000;
		KeDelayExecutionThread(KernelMode, false, &delay);
	}

	inline void read_virtual_memory(PEPROCESS pe_process, void* address, void* buffer, size_t size)
	{
		SIZE_T read;
		MmCopyVirtualMemory(pe_process, address, active_process::get_current_process(), buffer, size, KernelMode, &read);
	}
	inline void write_virtual_memory(PEPROCESS pe_process, void* address, void* buffer, size_t size)
	{
		SIZE_T read;
		MmCopyVirtualMemory(active_process::get_current_process(), buffer, pe_process, address, size, KernelMode, &read);
	}

	inline bool is_valid_address(uintptr_t address) { return ((address > 0x10000) && (address < 0x7ffffffeffff)); }
	inline bool is_valid_x86_address(uint32_t address) { return ((address > 0x10000) && (address < 0xffff0000)); }
};