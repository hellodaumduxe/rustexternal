#include <cstdio>
#include "drv_image.hpp"
#include "util.hpp"
#include "capcom.hpp"
#include "structs.hpp"
#include "loader.hpp"
#include "capcomsys.hpp"
#include <cassert>

#pragma comment(lib, "capcom.lib")

#define freeze while (true) Sleep(5000);
int __stdcall main()
{
	SetConsoleTitleA("trannysex rust drvmap baste \n");
	bool capcom_load_result = loader::load_vuln_driver(reinterpret_cast<uint8_t*>(capcom_sys), sizeof(capcom_sys), L"C:\\Windows\\Capcom.sys", L"Capcom");
	if (capcom_load_result)
		utils::log_message(7, "[>] loaded capcom driver. \n");
	else
	{
		utils::log_message(12, "[-] failed to load capcom, please disable real time protection. \n");
		freeze;
	}

#pragma region lambda stuff
	const auto capcom = std::make_unique<capcom::capcom_driver>();
	const auto _get_module = [&capcom](std::string_view name)
	{
		return capcom->get_kernel_module(name);
	};

	const auto _get_export_name = [&capcom](uintptr_t base, const char* name)
	{
		return capcom->get_export(base, name);
	};

	const std::function<uintptr_t(uintptr_t, uint16_t)> _get_export_ordinal = [&capcom](uintptr_t base, uint16_t ord)
	{
		return capcom->get_export(base, ord);
	};
#pragma endregion

	std::vector<uint8_t> driver_image;
	drvmap::util::open_binary_file("t_hanos.sys", driver_image);
	drvmap::drv_image driver(driver_image);

	const auto kernel_memory = capcom->allocate_pool(driver.size(), 'weRf', kernel::NonPagedPool, true);
	if (kernel_memory == 0x0)
	{
		utils::log_message(12, "[-] failed to allocate memory, please unload capcom and try again. \n");
		freeze;
	}
	utils::log_message(7, "[>] allocated 0x%x bytes @ 0x%I64X\n", driver.size(), kernel_memory);

	driver.fix_imports(_get_module, _get_export_name, _get_export_ordinal);
	utils::log_message(7, "[>] imports fixed. \n");

	driver.map();
	utils::log_message(7, "[>] sections mapped in memory. \n");

	driver.relocate(kernel_memory);
	utils::log_message(7, "[>] relocations fixed. \n");

	const auto _RtlCopyMemory = capcom->get_system_routine<drvmap::structs::RtlCopyMemoryFn>(L"RtlCopyMemory");

	const auto size = driver.size();
	const auto source = driver.data();
	const auto entry_point = kernel_memory + driver.entry_point();

	capcom->run([&kernel_memory, &source, &size, &_RtlCopyMemory](auto get_mm)
	{
		_RtlCopyMemory((void*)kernel_memory, source, size);
	});
	Sleep(2000);

	utils::log_message(14, "[>] calling entry point @ 0x%I64X \n", entry_point);
	auto status = STATUS_SUCCESS;
	const auto capcom_base = capcom->get_kernel_module("Capcom");
	capcom->run([&entry_point, &status, &kernel_memory, &capcom_base](auto mm_get) {
		using namespace drvmap::structs;
		status = ((PDRIVER_INITIALIZE)entry_point)((_DRIVER_OBJECT*)kernel_memory, (PUNICODE_STRING)capcom_base);
	}, false);
	Sleep(2000);

	if (!utils::clear_traces())
	{
		utils::log_message(12, "[-] not good, you will get banned. \n");
		freeze;
	}

	if (NT_SUCCESS(status))
	{
		utils::log_message(7, "[>] successfully created driver object. \n");
		const auto _RtlZeroMemory = capcom->get_system_routine<drvmap::structs::RtlZeroMemoryFn>(L"RtlZeroMemory");
		const auto header_size = driver.header_size();

		capcom->run([&_RtlZeroMemory, &kernel_memory, &header_size](auto mm_get) {
			_RtlZeroMemory((void*)kernel_memory, header_size);
		});
		utils::log_message(7, "[>] wiped headers. \n");
	}
	else
	{
		utils::log_message(12, "[-] not good, you will get banned. \n");
		freeze;
	}
	capcom->close_driver_handle();

	bool capcom_unload_result = loader::unload_vuln_driver("C:\\Windows\\Capcom.sys", L"Capcom");
	if (capcom_unload_result)
		utils::log_message(7, "[>] unloaded capcom driver. \n");
	else
	{
		utils::log_message(12, "[-] failed to unload capcom. \n");
		freeze;
	}

	utils::flush_dns_cache();
	utils::clear_event_logs();
	utils::delete_shadow_copies();
	utils::reset_verifier();
	utils::log_message(10, "[>] done, should be good to go. \n");


	freeze;
	return 0;
}