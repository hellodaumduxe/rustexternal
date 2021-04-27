#pragma once

#define get_current_process_handle reinterpret_cast<HANDLE>(-1)
namespace active_process
{
	__forceinline PEPROCESS get_current_process() { return IoGetCurrentProcess(); }
	__forceinline HANDLE get_current_process_id() { return PsGetCurrentProcessId(); }
};