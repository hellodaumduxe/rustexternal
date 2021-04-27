#include "stdafx.h"

void allocate_random_pages(HANDLE process)
{
	for (size_t i = 0; i < (114 + (util::rng() % 501)); i++)
	{
		size_t page_size = ((util::rng() % 410) * 0x1000);
		void* buffer = HeapAlloc(GetProcessHeap(), 0, page_size);
		for (size_t t = (util::rng() % 0x1100); t < page_size; t++)
			*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(buffer) + t) = static_cast<uint8_t>(util::rng() % 0xff);

		void* page = VirtualAllocEx(process, nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(process, page, buffer, page_size, nullptr);
		HeapFree(GetProcessHeap(), 0, buffer);
	}
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	util::seed = GetTickCount();

	HANDLE explorer = OpenProcess(PROCESS_ALL_ACCESS, false, util::get_process_id(s("explorer.exe")));
	HANDLE wmplayer = util::start_process(explorer, s("C:\\Program Files\\Windows Media Player\\wmplayer.exe"));
	Sleep(1500);
	allocate_random_pages(wmplayer);

	HANDLE file = CreateFileA(s("trannysex_rust.dll"), GENERIC_READ, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE)
		TerminateProcess(GetCurrentProcess(), -1);

	uint32_t size = GetFileSize(file, nullptr);
	void* image = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

	DWORD output;
	if (ReadFile(file, image, size, &output, nullptr) == 0)
		TerminateProcess(GetCurrentProcess(), -2);

	CloseHandle(file);
	IMAGE_DOS_HEADER* dos_headers = reinterpret_cast<IMAGE_DOS_HEADER*>(image);
	IMAGE_NT_HEADERS64* nt_headers = reinterpret_cast<IMAGE_NT_HEADERS64*>(reinterpret_cast<uintptr_t>(image) + dos_headers->e_lfanew);
	if ((nt_headers->Signature != IMAGE_NT_SIGNATURE) || !(nt_headers->FileHeader.Characteristics & IMAGE_FILE_DLL))
		TerminateProcess(GetCurrentProcess(), -3);

	size_t memory_page_size = nt_headers->OptionalHeader.SizeOfImage;
	void* game_memory_page = VirtualAllocEx(wmplayer, nullptr, memory_page_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	void* buffer_memory_page = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, memory_page_size);

	//headers
	memcpy(buffer_memory_page, image, nt_headers->OptionalHeader.SizeOfHeaders);

	//sections
	IMAGE_SECTION_HEADER* section_headers = reinterpret_cast<IMAGE_SECTION_HEADER*>(nt_headers + 1);
	for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; i++)
	{
		void* address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + section_headers[i].VirtualAddress);
		void* data = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(image) + section_headers[i].PointerToRawData);
		size_t size_of_data = section_headers[i].SizeOfRawData;
		memcpy(address, data, size_of_data);
	}

	//relocations
	IMAGE_BASE_RELOCATION* relocations = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	uintptr_t delta = (reinterpret_cast<uintptr_t>(game_memory_page) - nt_headers->OptionalHeader.ImageBase);
	while (relocations->SizeOfBlock >= 8)
	{
		uint32_t count = (relocations->SizeOfBlock - 8) / 2;
		uint16_t* list = reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(relocations) + 8);
		for (size_t i = 0; i < count; i++)
		{
			if (!list[i])
				continue;

			uint32_t* pointer = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + (relocations->VirtualAddress + (list[i] & 0xfff)));
			*pointer += delta;
		}

		relocations = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reinterpret_cast<uintptr_t>(relocations) + relocations->SizeOfBlock);
	};

	IMAGE_IMPORT_DESCRIPTOR* imports = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	do
	{
		IMAGE_THUNK_DATA* first_thunk = reinterpret_cast<IMAGE_THUNK_DATA*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + imports->FirstThunk);
		IMAGE_THUNK_DATA* orig_first_thunk = reinterpret_cast<IMAGE_THUNK_DATA*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + imports->OriginalFirstThunk);

		const char* module_name = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + imports->Name);
		HMODULE module = LoadLibraryA(module_name);
		do
		{
			if (orig_first_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				first_thunk->u1.Function = reinterpret_cast<unsigned long long>(GetProcAddress(module, reinterpret_cast<const char*>(orig_first_thunk->u1.Ordinal & 0xffff)));

				const char* function_name = reinterpret_cast<const char*>(orig_first_thunk->u1.Ordinal & 0xffff);
				memset(const_cast<char*>(function_name), 0, strlen(function_name));
			}
			else
			{
				IMAGE_IMPORT_BY_NAME* import_by_name = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(reinterpret_cast<uintptr_t>(buffer_memory_page) + orig_first_thunk->u1.AddressOfData);
				first_thunk->u1.Function = reinterpret_cast<unsigned long long>(GetProcAddress(module, import_by_name->Name));

				memset(const_cast<char*>(import_by_name->Name), 0, strlen(import_by_name->Name));
				import_by_name->Hint = 0;
			}
			first_thunk++;
			orig_first_thunk++;
		} while (orig_first_thunk->u1.AddressOfData);

		memset(const_cast<char*>(module_name), 0, strlen(module_name));
		imports++;
	} while (imports->Characteristics);

	//cleaning
	memset(buffer_memory_page, 0, nt_headers->OptionalHeader.SizeOfHeaders);

	//start da thread
	WriteProcessMemory(wmplayer, game_memory_page, buffer_memory_page, memory_page_size, nullptr);
	LPTHREAD_START_ROUTINE start_address = reinterpret_cast<LPTHREAD_START_ROUTINE>(reinterpret_cast<uintptr_t>(game_memory_page) + nt_headers->OptionalHeader.AddressOfEntryPoint);
	CreateRemoteThreadEx(wmplayer, nullptr, 0, start_address, game_memory_page, 0, nullptr, nullptr);
	TerminateProcess(GetCurrentProcess(), 0);
	return 0;
}