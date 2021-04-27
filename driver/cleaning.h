#pragma once

namespace cleaning
{
	bool invalidate_driver(const wchar_t* symbolic_link);
	bool clear_piddb_cache(const wchar_t* driver, const ULONG time_stamp);
}