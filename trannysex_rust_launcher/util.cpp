#include "stdafx.h"

int util::seed = 0;

uint32_t util::get_process_id(const char* name)
{
	PROCESSENTRY32 proc_entry;
	proc_entry.dwSize = sizeof PROCESSENTRY32;
	void* t32_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	uint32_t process_id = 0;
	Process32First(t32_snap, &proc_entry);
	do
	{
		if (strcmp(proc_entry.szExeFile, name) == 0) 
		{
			process_id = proc_entry.th32ProcessID;
			break;
		}
	} while (Process32Next(t32_snap, &proc_entry) == 1);
	CloseHandle(t32_snap);
	return process_id;
}

HANDLE util::start_process(HANDLE parent, const char * path)
{
	SIZE_T size;
	STARTUPINFOEX si = {};
	PROCESS_INFORMATION pi = {};

	InitializeProcThreadAttributeList(nullptr, 1, 0, &size);
	si.lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(HeapAlloc(GetProcessHeap(), 0, size));
	InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &size);
	UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &parent, sizeof(parent), nullptr, nullptr);

	si.StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.StartupInfo.wShowWindow = SW_SHOW;
	si.StartupInfo.cb = sizeof(STARTUPINFO);
	CreateProcessA(path, nullptr, nullptr, nullptr, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW | EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, reinterpret_cast<STARTUPINFO*>(&si), &pi);
	Sleep(500);

	HeapFree(GetProcessHeap(), 0, si.lpAttributeList);
	CloseHandle(pi.hThread);
	return pi.hProcess;
}