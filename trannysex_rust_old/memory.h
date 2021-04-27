#pragma once

enum control_codes
{
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
};

class c_find_program_request
{
public:
	HANDLE pid;
	control_codes control_code;

	char buffer[32];
};

class c_find_module_request
{
public:
	HANDLE pid;
	control_codes control_code;

	void* module;
	wchar_t buffer[32];
};

struct IO_STATUS_BLOCK 
{
	union {
		NTSTATUS Status;
		PVOID    Pointer;
	} DUMMYUNIONNAME;
	ULONG_PTR Information;
};
typedef NTSTATUS(NTAPI* NtDeviceIoControlFile_t)(
	IN HANDLE            FileHandle,
	IN HANDLE            Event,
	IN void*   ApcRoutine,
	IN PVOID             ApcContext,
	OUT IO_STATUS_BLOCK* IoStatusBlock,
	IN ULONG             IoControlCode,
	IN PVOID             InputBuffer,
	IN ULONG             InputBufferLength,
	OUT PVOID            OutputBuffer,
	IN ULONG             OutputBufferLength
	);

namespace memory
{
	extern HANDLE driver;
	extern NtDeviceIoControlFile_t NtDeviceIoControlFile;

	extern HANDLE rust_pid;
	extern uintptr_t unity_player;

	extern HANDLE process_heap;
	extern void* transform_array;
	extern void* transform_indices;

	__forceinline void send_request(void* information, size_t size)
	{
		IO_STATUS_BLOCK status_block;
		NtDeviceIoControlFile(memory::driver, nullptr, nullptr, nullptr, &status_block, 0x0, information, static_cast<ULONG>(size), information, static_cast<ULONG>(size));
	}

	__forceinline uintptr_t find_module(const wchar_t* name)
	{
		c_find_module_request find_module_request;
		find_module_request.pid = memory::rust_pid;
		find_module_request.control_code = control_codes::find_module;

		memset(find_module_request.buffer, 0x0, sizeof(find_module_request.buffer));
		wcscpy(find_module_request.buffer, name);

		memory::send_request(&find_module_request, sizeof(find_module_request));
		return reinterpret_cast<uintptr_t>(find_module_request.module);
	};

	__forceinline HANDLE find_process(const char* name)
	{
		c_find_program_request find_program_request;
		find_program_request.pid = 0x0;
		find_program_request.control_code = control_codes::find_program;

		memset(find_program_request.buffer, 0x0, sizeof(find_program_request.buffer));
		strcpy(find_program_request.buffer, name);

		memory::send_request(&find_program_request, sizeof(find_program_request));
		return find_program_request.pid;
	}

	template <typename type>
	__forceinline type read(uint64_t address)
	{
		type buffer = type();

		c_kernel_basic_request basic_request;
		basic_request.pid = memory::rust_pid;
		basic_request.control_code = control_codes::read;

		basic_request.address = reinterpret_cast<void*>(address);
		basic_request.buffer = &buffer;
		basic_request.size = sizeof(type);

		memory::send_request(&basic_request, sizeof(basic_request));
		return buffer;
	}

	template <typename type>
	__forceinline void write(uint64_t address, type buffer)
	{
		c_kernel_basic_request basic_request;
		basic_request.pid = memory::rust_pid;
		basic_request.control_code = control_codes::write;

		basic_request.address = reinterpret_cast<void*>(address);
		basic_request.buffer = &buffer;
		basic_request.size = sizeof(type);

		memory::send_request(&basic_request, sizeof(basic_request));
	}

	template <typename buffer>
	__forceinline void read_buffer(uint64_t address, buffer buffer, size_t size)
	{
		c_kernel_basic_request basic_request;
		basic_request.pid = memory::rust_pid;
		basic_request.control_code = control_codes::read;

		basic_request.address = reinterpret_cast<void*>(address);
		basic_request.buffer = reinterpret_cast<void*>(buffer);
		basic_request.size = size;

		memory::send_request(&basic_request, sizeof(basic_request));
	}

	__forceinline void init()
	{
		memory::driver = CreateFileA(s("\\\\.\\UHFControl"), 0i64, 0i64, 0i64, OPEN_ALWAYS, 0, 0i64);
		memory::NtDeviceIoControlFile = reinterpret_cast<NtDeviceIoControlFile_t>(GetProcAddress(GetModuleHandle(s("ntdll.dll")), s("NtDeviceIoControlFile")));

		memory::rust_pid = memory::find_process(s("RustClient.exe"));
		while (memory::rust_pid == 0x0)
		{
			macro::print(s("[>] waiting for RustClient.exe. \n"));
			memory::rust_pid = memory::find_process(s("RustClient.exe"));
			Sleep(1000);
		}

		memory::unity_player = memory::find_module(s_w(L"UnityPlayer.dll"));
		while (memory::unity_player == 0x0)
		{
			macro::print(s("[>] waiting for UnityPlayer.dll. \n"));
			memory::unity_player = memory::find_module(s_w(L"UnityPlayer.dll"));
			Sleep(1000);
		}



		memory::process_heap = GetProcessHeap();
		memory::transform_array = HeapAlloc(memory::process_heap, 0x0, 0x2700);
		memory::transform_indices = HeapAlloc(memory::process_heap, 0x0, 0x500);
	}
};