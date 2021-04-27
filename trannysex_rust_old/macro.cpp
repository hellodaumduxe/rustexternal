#include "stdafx.h"

char* macro::strstr(const char* str1, const char* str2)
{
	const char* p1 = str1;
	const char* p2 = str2;
	const char* r = *p2 == 0 ? str1 : 0;

	while (*p1 != 0 && *p2 != 0)
	{
		if ((unsigned char)*p1 == (unsigned char)*p2)
		{
			if (r == 0)
			{
				r = p1;
			}

			p2++;
		}
		else
		{
			p2 = str2;
			if (r != 0)
			{
				p1 = r + 1;
			}

			if ((unsigned char)*p1 == (unsigned char)*p2)
			{
				r = p1;
				p2++;
			}
			else
			{
				r = 0;
			}
		}

		p1++;
	}

	return *p2 == 0 ? (char*)r : 0;
}

char print_buffer[256];
void macro::print_to_buffer(char* buffer, const char* str, ...)
{
	va_list list;
	va_start(list, str);
	wvsprintfA(buffer, str, list);
	va_end(list);
}

void macro::print(const char* str, ...)
{
	va_list list;
	va_start(list, str);
	wvsprintfA(print_buffer, str, list);
	va_end(list);

	DWORD ret;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), print_buffer, static_cast<DWORD>(strlen(print_buffer)), &ret, 0);
}

void macro::system(const char* buffer)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&si, 0x0, sizeof(si));
	memset(&pi, 0x0, sizeof(pi));
	si.cb = sizeof(si);

	CreateProcessA(s("C:\\Windows\\System32\\cmd.exe"), const_cast<char*>(buffer), nullptr, nullptr, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
	Sleep(500);

	TerminateProcess(pi.hProcess, 0x0);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}