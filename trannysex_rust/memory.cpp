#include "stdafx.h"

HANDLE memory::driver;
HANDLE memory::pid;
uintptr_t memory::unity_player;
uintptr_t memory::game_assembly;

HANDLE memory::process_heap;
void* memory::transform_array;
void* memory::transform_indices;

c_linked_list<void*> memory::random_pages;
void memory::allocate_random_pages()
{
	memory::random_pages.allocate_list();
	memory::random_pages.clear();

	size_t num_pages = (221 + (macro::rng() % 401));
	print("[>] creating %d random pages. \n", num_pages);

	for (size_t i = 0; i < num_pages; i++)
	{
		size_t page_size = ((macro::rng() % 400) * 0x1000);
		void* page = VirtualAlloc(nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		VirtualLock(page, page_size);
		for (size_t t = (macro::rng() % 0x1100); t < page_size; t++)
			*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(page) + t) = static_cast<uint8_t>(macro::rng() % 0xff);
		VirtualUnlock(page, page_size);

		auto entry = memory::random_pages.create_entry();
		entry->_item = page;
	}
}

void memory::refresh_working_set()
{
	//print("[>] refreshing working set. \n");
	for (auto entry = memory::random_pages.list->next_entry; entry != nullptr; entry = entry->next_entry)
		*reinterpret_cast<uint8_t*>(entry->_item) = static_cast<uint8_t>(macro::rng() % 0xff);
}

uint8_t bytes[] = { 0xb9, 0x20, 0x4e, 0x0, 0x0, 0x48, 0xb8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff, 0xd0, 0xe9, 0x0, 0x0, 0x0, 0x0 };
void memory::start_random_threads()
{
	auto create_random_thread = [=]()
	{
		size_t page_size = ((macro::rng() % 230) * 0x100);
		void* page = VirtualAlloc(nullptr, page_size + 0x400, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (page == nullptr)
			return;

		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(page, &mbi, sizeof(mbi)) < sizeof(mbi))
			return;
		page_size = mbi.RegionSize;

		VirtualLock(page, page_size);
		int32_t jump_end = static_cast<int32_t>(page_size - (sizeof(bytes) + 5));
		*reinterpret_cast<uint8_t*>(page) = 0xe9;
		*reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(page) + 1) = jump_end;

		for (size_t i = 5; i < jump_end; i++)
			*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(page) + i) = static_cast<uint8_t>(macro::rng() % 0xff);

		*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(bytes) + 7) = reinterpret_cast<uintptr_t>(Sleep);

		int32_t jump_back_addr = static_cast<int32_t>(page_size);
		*reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(bytes) + 18) = (-jump_back_addr);
		memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(page) + (page_size - sizeof(bytes))), bytes, sizeof(bytes));
		macro::create_thread(page);
		VirtualUnlock(page, page_size);
	};

	int threads = 42 + (macro::rng() % 42);
	print("[>] starting %d random threads. \n", threads);
	do
	{
		create_random_thread();
	} while (threads-- > 0);
}

void memory::init()
{
	LoadLibraryA(s("D3DCOMPILER_43.dll"));
	memory::process_heap = GetProcessHeap();
	memory::transform_array = memory::heap_alloc(0x3000);
	memory::transform_indices = memory::heap_alloc(0x1000);

	memory::allocate_random_pages();
	memory::start_random_threads();
	memory::driver = CreateFileA(s("\\\\.\\UHFControl"), 0, 0, nullptr, OPEN_ALWAYS, 0, nullptr);
}