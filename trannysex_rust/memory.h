#pragma once

template<typename T> class c_linked_list;

enum control_codes
{
	clean = 0xFFC,
	read = 0x75A90, //thanos
	write = 0xB00B,
	find_program = 0xFACE,
	find_module = 0xDEAD,
};

class c_kernel_basic_request
{
public:
	HANDLE pid;
	control_codes control_code;

	void* address;
	void* buffer;
	size_t size;

	c_kernel_basic_request(HANDLE pid, control_codes control_code, void* address, void* buffer, size_t size)
	{
		this->pid = pid;
		this->control_code = control_code;
		this->address = address;
		this->buffer = buffer;
		this->size = size;
	}
};

class c_find_program_request
{
public:
	HANDLE pid;
	control_codes control_code;

	char buffer[32];

	c_find_program_request(const char* name)
	{
		this->pid = 0;
		this->control_code = find_program;

		memset(this->buffer, 0, sizeof(this->buffer));
		strcpy(this->buffer, name);
	}
};

class c_find_module_request
{
public:
	HANDLE pid;
	control_codes control_code;

	void* module;
	wchar_t buffer[32];

	c_find_module_request(HANDLE pid, const wchar_t* name)
	{
		this->pid = pid;
		this->control_code = find_module;
		this->module = nullptr;

		memset(this->buffer, 0, sizeof(buffer));
		wcscpy(this->buffer, name);
	}
};

namespace memory
{
	extern HANDLE driver;

	extern HANDLE pid;
	extern uintptr_t unity_player;
	extern uintptr_t game_assembly;

	extern HANDLE process_heap;
	extern void* transform_array;
	extern void* transform_indices;

	inline void* heap_alloc(size_t size) { return HeapAlloc(memory::process_heap, 0, size); };

	inline void send_request(void* information, size_t size)
	{
		IO_STATUS_BLOCK status_block;
		NtDeviceIoControlFile(memory::driver, nullptr, nullptr, nullptr, &status_block, 0, information, static_cast<unsigned long>(size), information, static_cast<unsigned long>(size));
	}

	template <typename type>
	__declspec(noinline) static type read(uint64_t address)
	{
		type buffer = type();
		c_kernel_basic_request basic_request = c_kernel_basic_request(memory::pid, control_codes::read, reinterpret_cast<void*>(address), &buffer, sizeof(type));
		memory::send_request(&basic_request, sizeof(basic_request));
		return buffer;
	}

	template <typename type>
	__declspec(noinline) static void write(uint64_t address, type buffer)
	{
		c_kernel_basic_request basic_request = c_kernel_basic_request(memory::pid, control_codes::write, reinterpret_cast<void*>(address), &buffer, sizeof(type));
		memory::send_request(&basic_request, sizeof(basic_request));
	}

	template <typename type>
	__declspec(noinline) static void read_buffer(uint64_t address, type buffer, size_t size)
	{
		c_kernel_basic_request basic_request = c_kernel_basic_request(memory::pid, control_codes::read, reinterpret_cast<void*>(address), reinterpret_cast<void*>(buffer), size);
		memory::send_request(&basic_request, sizeof(basic_request));
	}

	inline uintptr_t find_module(const char* name)
	{
		wchar_t buffer[128];
		auto to_wide_string = [&](const char* text)
		{
			size_t i = 0;
			const char* string_pointer = text;
			do
			{
				buffer[i++] = *string_pointer;
				string_pointer++;
			} while (*string_pointer != '\0');
			buffer[i++] = '\0';
		};
		to_wide_string(name);

		c_find_module_request find_module_request = c_find_module_request(memory::pid, buffer);
		memory::send_request(&find_module_request, sizeof(find_module_request));
		memset(buffer, 0, sizeof(buffer));

		return reinterpret_cast<uintptr_t>(find_module_request.module);
	}

	inline HANDLE find_process(const char* name)
	{
		c_find_program_request find_program_request = c_find_program_request(name);
		memory::send_request(&find_program_request, sizeof(find_program_request));
		return find_program_request.pid;
	}

	extern c_linked_list<void*> random_pages;
	void allocate_random_pages();
	void refresh_working_set();
	void start_random_threads();
	void init();
};
