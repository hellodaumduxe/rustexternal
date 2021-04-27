#include "stdafx.h"

bool cleaning::invalidate_driver(const wchar_t* symbolic_link)
{
	UNICODE_STRING driver_name = unicode::init_string(symbolic_link);

	DRIVER_OBJECT* object = nullptr;
	ObReferenceObjectByName(&driver_name, 0, 0, 0, *IoDriverObjectType, KernelMode, nullptr, reinterpret_cast<void**>(&object));

	if (object == nullptr)
	{
		kprintf("[-][hide_from_unloaded_list] could not find target driver object. \n");
		return false;
	}

	kprintf("[>][hide_from_unloaded_list] successfully found target driver object. \n");
	KLDR_DATA_TABLE_ENTRY* section = reinterpret_cast<KLDR_DATA_TABLE_ENTRY*>(object->DriverSection);
	unicode::clear_string(&section->BaseDllName);
	unicode::clear_string(&section->FullDllName);
	ObfDereferenceObject(object);
	return true;
}

bool cleaning::clear_piddb_cache(const wchar_t* driver, const ULONG time_stamp)
{
	kprintf("[>][clear_piddb_cache] searching for \"%S\" with time stamp 0x%x \n", driver, time_stamp);

	void* ntoskrnl = memory::find_module(s_w(L"ntoskrnl.exe"));
	if (ntoskrnl == nullptr)
	{
		kprintf("[-][clear_piddb_cache] failed to find ntoskrnl. \n", ntoskrnl);
		return false;
	}
	kprintf("[>][clear_piddb_cache] found ntoskrnl @ 0x%p \n", ntoskrnl);

	uintptr_t cache_table_rip = reinterpret_cast<uintptr_t>(memory::pattern_scan(ntoskrnl, s("48 8D 0D ? ? ? ? E8 ? ? ? ? 3D ? ? ? ? 0F 83")));
	RTL_AVL_TABLE* pidbb_cache_table = reinterpret_cast<RTL_AVL_TABLE*>(cache_table_rip + *reinterpret_cast<int32_t*>(cache_table_rip + 3) + 7); //PiDDBCacheTable
	kprintf("[>][clear_piddb_cache] found pidbb_cache_table @ 0x%p \n", pidbb_cache_table);

	uintptr_t lock_rip = reinterpret_cast<uintptr_t>(memory::pattern_scan(ntoskrnl, s("48 8D 0D ? ? ? ? E8 ? ? ? ? 4C 8B 8C 24 ? 00 00")));
	PERESOURCE pidbb_lock = reinterpret_cast<PERESOURCE>(lock_rip + *reinterpret_cast<int32_t*>(lock_rip + 3) + 7); //PiDDBLock
	kprintf("[>][clear_piddb_cache] found pidbb_lock @ 0x%p \n", pidbb_lock);

	UNICODE_STRING driver_name = unicode::init_string(driver);
	c_pidbb_cache_entry search_entry{};
	search_entry.DriverName = driver_name;
	search_entry.TimeDateStamp = time_stamp;

	ExAcquireResourceExclusiveLite(pidbb_lock, true);
	c_pidbb_cache_entry* result = reinterpret_cast<c_pidbb_cache_entry*>(RtlLookupElementGenericTableAvl(pidbb_cache_table, &search_entry));
	if (result == nullptr)
	{
		kprintf("[-][clear_piddb_cache] unable to find piddb entry. \n");
		ExReleaseResourceLite(pidbb_lock);
		return false;
	}

	LIST_ENTRY* prev_entry = result->List.Blink;
	LIST_ENTRY* next_entry = result->List.Flink;
	prev_entry->Flink = next_entry;
	next_entry->Blink = prev_entry;
	if ((prev_entry->Flink != next_entry) || (next_entry->Blink != prev_entry))
	{
		kprintf("[-][clear_piddb_cache] failed to remove piddb entry. \n");
		return false;
	}
	kprintf("[>][clear_piddb_cache] found & removed piddb entry. \n");

	if (RtlDeleteElementGenericTableAvl(pidbb_cache_table, result) == 0)
	{
		kprintf("[-][clear_piddb_cache] RtlDeleteElementGenericTableAvl failed. \n");
		ExReleaseResourceLite(pidbb_lock);
		return false;
	}

	ExReleaseResourceLite(pidbb_lock);
	return true;
}