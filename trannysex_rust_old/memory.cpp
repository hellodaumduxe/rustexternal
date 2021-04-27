#include "stdafx.h"

HANDLE memory::driver;
HANDLE memory::rust_pid;
uintptr_t memory::unity_player;
NtDeviceIoControlFile_t memory::NtDeviceIoControlFile;

HANDLE memory::process_heap;
void* memory::transform_array;
void* memory::transform_indices;