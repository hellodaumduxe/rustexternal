#pragma once

namespace eprocess
{
	extern int unique_process_id;
	extern int active_process_links;
	extern int image_file_name;
	extern int active_threads;
	extern int flags;
	extern int peb;

	extern bool completed;
	void update_offsets();
}