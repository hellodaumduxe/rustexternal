#pragma once

#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <intrin.h>

//#define kprintf(x, ...)	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, x, __VA_ARGS__)
#define kprintf //

#include "undocumented.h"
extern "C" __declspec(dllimport) POBJECT_TYPE* IoDriverObjectType;
extern "C" __declspec(dllimport) KLDR_DATA_TABLE_ENTRY* PsLoadedModuleList;
extern "C" __declspec(dllimport) NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS src_process, void* src, PEPROCESS dst_process, void* dst, SIZE_T size, KPROCESSOR_MODE prev_mode, SIZE_T* ret_size);
extern "C" __declspec(dllimport) void NTAPI ObReferenceObjectByName(PUNICODE_STRING, ULONG, PACCESS_STATE, ACCESS_MASK, POBJECT_TYPE, KPROCESSOR_MODE, PVOID OPTIONAL, PVOID*);

#include "xor.h"
#include "unicode.h"
#include "active_process.h"
#include "macro.h"
#include "memory.h"
#include "cleaning.h"
#include "eprocess.h"
#include "process.h"

#pragma region io control stuff
enum control_codes
{
	clean = 0xFFC,
	read = 0x75A90, //thanos
	write = 0xB00B,
	find_program = 0xFACE,
	find_module = 0xDEAD,
};

class c_cleaning_request
{
	void* pad;
public:
	control_codes control_code;
	bool success;
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
#pragma endregion