#pragma once

namespace util
{
	extern int seed;
	inline int rng() { util::seed = ((util::seed * 1103515245 + 12345) & 0x7fffffff); return util::seed; };

	uint32_t get_process_id(const char* name);
	HANDLE start_process(HANDLE parent, const char* path);
}