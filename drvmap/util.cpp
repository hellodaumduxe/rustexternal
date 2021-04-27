#include "util.hpp"
#include <iterator>
#include <fstream>
#include <windows.h>

namespace drvmap::util
{
	void open_binary_file(const std::string& file, std::vector<uint8_t>& data)
	{
		std::ifstream file_stream(file, std::ios::binary);
		file_stream.unsetf(std::ios::skipws);
		file_stream.seekg(0, std::ios::end);

		const auto file_size = file_stream.tellg();

		file_stream.seekg(0, std::ios::beg);
		data.reserve(static_cast<uint32_t>(file_size));
		data.insert(data.begin(), std::istream_iterator<uint8_t>(file_stream), std::istream_iterator<uint8_t>());
	}
}

char buffer[1024];
void utils::log_message(int color, const char* str, ...)
{
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console_handle, color);

	va_list list;
	va_start(list, str);
	wvsprintfA(buffer, str, list);
	va_end(list);

	DWORD ret;
	WriteConsoleA(console_handle, buffer, static_cast<DWORD>(strlen(buffer)), &ret, 0);
}

void utils::print_to_buffer(char* buffer, const char* str, ...)
{
	va_list list;
	va_start(list, str);
	wvsprintfA(buffer, str, list);
	va_end(list);
}

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




bool utils::clear_traces()
{
	c_cleaning_request request;
	request.control_code = clean;
	request.success = false;

	HANDLE ntusbf_driver = CreateFileA("\\\\.\\UHFControl", 0i64, 0i64, 0i64, OPEN_ALWAYS, 0, 0i64);
	DeviceIoControl(ntusbf_driver, 0, &request, sizeof(request), &request, sizeof(request), nullptr, nullptr);
	CloseHandle(ntusbf_driver);

	return request.success;
}

void utils::flush_dns_cache()
{
	system("ipconfig /flushdns > nul");
	utils::log_message(14, "[>] flushing dns cache. \n");
}

void utils::clear_event_logs()
{
	system("wevtutil cl Application");
	system("wevtutil cl Security");
	system("wevtutil cl Setup");
	system("wevtutil cl System");
	utils::log_message(14, "[>] clearing event logs. \n");
}

void utils::delete_shadow_copies()
{
	system("vssadmin delete shadows /All > nul");
	utils::log_message(14, "[>] deleting system shadow copies. \n");
}

void utils::reset_verifier()
{
	system("verifier /reset > nul");
}