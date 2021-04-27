#pragma once

#define driver_tag 'weRf'
namespace memory
{
	__forceinline void* allocate_paged_pool(size_t size) { return ExAllocatePoolWithTag(PagedPool, static_cast<SIZE_T>(size), driver_tag); }
	__forceinline void* allocate_non_paged_pool(size_t size) { return ExAllocatePoolWithTag(NonPagedPool, static_cast<SIZE_T>(size), driver_tag); }
	__forceinline void free_pool(void* pool) { ExFreePoolWithTag(pool, driver_tag); }

	__forceinline void disable_wp()
	{
		//disable wp bit
		__int64 cr0 = __readcr0();
		cr0 = cr0 & ~0x10000;
		__writecr0(cr0);
	}

	__forceinline void enable_wp()
	{
		//enable wp bit
		__int64 cr0 = __readcr0();
		cr0 = cr0 | 0x10000;
		__writecr0(cr0);
	}

	size_t image_size(void* image);
	uintptr_t find_free_space(const void* module, const size_t module_size, const size_t size);
	void* find_module(const wchar_t* name);
	void* pattern_scan(const void* module, const char* pattern);
}