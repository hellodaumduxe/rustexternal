#pragma once
#include <string>
#include <vector>

namespace utils
{
	void log_message(int color, const char* str, ...);
	void print_to_buffer(char* buffer, const char* str, ...);

	bool clear_traces();
	void flush_dns_cache();
	void clear_event_logs();
	void delete_shadow_copies();
	void reset_verifier();
}

namespace drvmap::util
{
	void open_binary_file(const std::string& file, std::vector<uint8_t>& data);
}