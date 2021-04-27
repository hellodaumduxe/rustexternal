#pragma once

namespace process
{
	__forceinline void stack_attach(PEPROCESS pe_process, KAPC_STATE* apc_state) { KeStackAttachProcess(pe_process, apc_state); }
	__forceinline void stack_detach(KAPC_STATE* apc_state) { KeUnstackDetachProcess(apc_state); }
	__forceinline bool reference_process(HANDLE pid, PEPROCESS* process) { return NT_SUCCESS(PsLookupProcessByProcessId(pid, process)); }

	template <typename type>
	type read(uintptr_t address)
	{
		if (!macro::is_valid_address(address))
			return type();

		type result = type();
		macro::read_virtual_memory(active_process::get_current_process(), reinterpret_cast<void*>(address), &result, sizeof(type));
		return result;
	}

	template <typename type>
	inline void read_buffer(uintptr_t address, type buffer, size_t size)
	{
		if (!macro::is_valid_address(address) || !macro::is_valid_address(address + size))
			return;
		macro::read_virtual_memory(active_process::get_current_process(), reinterpret_cast<void*>(address), buffer, size);
	}

	inline void reference_and_read(HANDLE pid, void* address, void* buffer, size_t size)
	{
		if (pid == 0)
			return;

		PEPROCESS pe_process;
		if (!process::reference_process(pid, &pe_process))
		{
			kprintf("[>][read] unable to reference process. \n");
			return;
		}

		if (!macro::is_valid_address(reinterpret_cast<uintptr_t>(address)) || !macro::is_valid_address(reinterpret_cast<uintptr_t>(address) + size))
		{
			kprintf("[>][read] invalid address @ 0x%p. \n", address);
			ObfDereferenceObject(pe_process);
			return;
		}

		macro::read_virtual_memory(pe_process, address, buffer, size);
		ObfDereferenceObject(pe_process);
	}

	inline void reference_and_write(HANDLE pid, void* address, void* buffer, size_t size)
	{
		if (pid == 0)
			return;

		PEPROCESS pe_process;
		if (!process::reference_process(pid, &pe_process))
		{
			kprintf("[>][write] unable to reference process. \n");
			return;
		}

		if (!macro::is_valid_address(reinterpret_cast<uintptr_t>(address)) || !macro::is_valid_address(reinterpret_cast<uintptr_t>(address) + size))
		{
			kprintf("[>][write] invalid address @ 0x%p \n", address);
			ObfDereferenceObject(pe_process);
			return;
		}

		macro::write_virtual_memory(pe_process, address, buffer, size);
		ObfDereferenceObject(pe_process);
	}

	void* find_module(HANDLE pid, const wchar_t* module);
	HANDLE find(const char* name);
};