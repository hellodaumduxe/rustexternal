#include "stdafx.h"

void* process::find_module(HANDLE pid, const wchar_t* name)
{
	eprocess::update_offsets();
	auto find_module_internal_64 = [=](uintptr_t peb, const wchar_t* name)->void*
	{
		if (!macro::is_valid_address(peb)) return nullptr;
		uintptr_t ldr = process::read<uintptr_t>(peb + 0x18);
		if (!macro::is_valid_address(ldr)) return nullptr;
		uintptr_t flink = process::read<uintptr_t>(ldr + 0x10);
		if (!macro::is_valid_address(flink)) return nullptr;
		uintptr_t current_module = flink;
		do
		{
			wchar_t buffer[256];
			uintptr_t dll_name_ptr = process::read<uintptr_t>(current_module + 0x60);
			if (macro::is_valid_address(dll_name_ptr))
			{
				process::read_buffer(dll_name_ptr, &buffer, sizeof(buffer));
				if (macro::wcscmp(buffer, name))
					return reinterpret_cast<void*>(process::read<uintptr_t>(current_module + 0x30));
			}
			current_module = process::read<uintptr_t>(current_module);
		} while (process::read<uintptr_t>(current_module + 0x30) != 0);
		return nullptr;
	};
	auto find_module_internal_32 = [=](uintptr_t peb, const wchar_t* name)->void*
	{
		if (!macro::is_valid_x86_address(static_cast<uint32_t>(peb))) return nullptr;
		uintptr_t peb_32 = (peb + 0x1000);
		uint32_t peb_ldr = process::read<uint32_t>(peb_32 + 0xc);
		if (!macro::is_valid_x86_address(peb_ldr)) return nullptr;
		uint32_t modules_linked_list = process::read<uint32_t>(peb_ldr + 0xc);
		if (!macro::is_valid_x86_address(modules_linked_list)) return nullptr;

		for (uint32_t module = process::read<uint32_t>(modules_linked_list);
			(process::read<uint32_t>(module + 0x18) != 0x0); //module->DllBase
			module = process::read<uint32_t>(module))
		{
			wchar_t buffer[256];
			uint32_t dll_name_ptr = process::read<uint32_t>(module + 0x30);
			if (macro::is_valid_x86_address(dll_name_ptr))
			{
				process::read_buffer(dll_name_ptr, &buffer, sizeof(buffer));
				if (macro::wcscmp(buffer, name))
					return reinterpret_cast<void*>(process::read<uint32_t>(module + 0x18));
			}
		}
		return nullptr;
	};

	KAPC_STATE state;
	PEPROCESS pe_process;
	if (!process::reference_process(pid, &pe_process)) return nullptr;
	process::stack_attach(pe_process, &state);

	void* result = nullptr;
	uintptr_t peb = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(pe_process) + eprocess::peb);
	unsigned long flags = *reinterpret_cast<unsigned long*>(reinterpret_cast<uintptr_t>(pe_process) + eprocess::flags);
	result = ((flags & (1 << 9)) ? find_module_internal_32(peb, name) : find_module_internal_64(peb, name));

	process::stack_detach(&state);
	ObfDereferenceObject(pe_process);
	return result;
}

HANDLE process::find(const char* name)
{
	eprocess::update_offsets();
	PEPROCESS pe_process;
	if (process::reference_process(reinterpret_cast<HANDLE>(4), &pe_process) == false)
	{
		kprintf("[>][find_process_by_name] failed to reference system??? \n");
		return 0;
	}
	ObfDereferenceObject(pe_process);

	size_t sys_process_count = 0;
	LIST_ENTRY* begin = reinterpret_cast<LIST_ENTRY*>(reinterpret_cast<uintptr_t>(pe_process) + eprocess::active_process_links)->Flink;
	LIST_ENTRY* current_module = begin;
	do
	{
		uintptr_t current_pe_process = (reinterpret_cast<uintptr_t>(current_module) - eprocess::active_process_links);
		if (*reinterpret_cast<uint64_t*>(current_pe_process + eprocess::unique_process_id) == 4)
			if (sys_process_count++ > 1) 
				break;

		unsigned long flags = *reinterpret_cast<unsigned long*>(current_pe_process + eprocess::flags);
		unsigned long active_threads = *reinterpret_cast<unsigned long*>(current_pe_process + eprocess::active_threads);
		if (!(flags & (1 << 2)) && (active_threads > 0))
		{
			char* file_name = reinterpret_cast<char*>(current_pe_process + eprocess::image_file_name);
			if (strcmp(file_name, name) == 0)
				return *reinterpret_cast<HANDLE*>(current_pe_process + eprocess::unique_process_id);
		}
		current_module = current_module->Flink;
	} while (true);
	return 0;
}