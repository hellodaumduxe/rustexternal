#include "stdafx.h"

int eprocess::unique_process_id;
int eprocess::active_process_links;
int eprocess::image_file_name;
int eprocess::active_threads;
int eprocess::flags;
int eprocess::peb;

bool eprocess::completed = false;
void eprocess::update_offsets()
{
	if (eprocess::completed)
		return;

	RTL_OSVERSIONINFOW rtl_version_info;
	RtlGetVersion(&rtl_version_info);

	switch (rtl_version_info.dwBuildNumber)
	{
	case 17134:
	case 17763:
		eprocess::unique_process_id = 0x2e0;
		eprocess::active_process_links = 0x2e8;
		eprocess::image_file_name = 0x450;
		eprocess::active_threads = 0x498;
		eprocess::flags = 0x304;
		eprocess::peb = 0x3f8;
		break;

	case 18362:
	case 18363:
		eprocess::unique_process_id = 0x2e8;
		eprocess::active_process_links = 0x2f0;
		eprocess::image_file_name = 0x450;
		eprocess::active_threads = 0x498;
		eprocess::flags = 0x30c;
		eprocess::peb = 0x3f8;
		break;
	};
	eprocess::completed = true;
}
