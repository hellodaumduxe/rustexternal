#include "stdafx.h"

void* driver_base = nullptr;
NTSTATUS __fastcall io_control(__int64 ecx, DEVICE_OBJECT* device_object, IRP* irp)
{
	UNREFERENCED_PARAMETER(ecx);
	UNREFERENCED_PARAMETER(device_object);

	kprintf("[>][io_control] recieved call. \n");
	irp->IoStatus.Information = 0;

	c_kernel_basic_request* basic_request = reinterpret_cast<c_kernel_basic_request*>(irp->AssociatedIrp.SystemBuffer);
	if (basic_request == nullptr)
	{
		irp->IoStatus.Status = STATUS_ACCESS_DENIED;
		IofCompleteRequest(irp, IO_NO_INCREMENT);
		return STATUS_ACCESS_DENIED;
	}

	switch (basic_request->control_code)
	{
	case clean:
	{
		kprintf("[>][io_control] performing \"clean\" operation. \n");
		c_cleaning_request* cleaning_request = reinterpret_cast<c_cleaning_request*>(basic_request);
		cleaning_request->success = (cleaning::invalidate_driver(s_w(L"\\Driver\\Capcom")) && cleaning::clear_piddb_cache(s_w(L"capcom.sys"), 0x57cd1415));
		irp->IoStatus.Information = sizeof(c_cleaning_request);
	}
	break;

	case read:
		kprintf("[>][io_control] performing \"read\" operation. \n");
		process::reference_and_read(basic_request->pid, basic_request->address, basic_request->buffer, basic_request->size);
		break;

	case write:
		kprintf("[>][io_control] performing \"write\" operation. \n");
		process::reference_and_write(basic_request->pid, basic_request->address, basic_request->buffer, basic_request->size);
		break;

	case find_program:
	{
		kprintf("[>][io_control] performing \"find_program\" operation. \n");
		c_find_program_request* find_program_request = reinterpret_cast<c_find_program_request*>(basic_request);
		find_program_request->pid = process::find(find_program_request->buffer);
		irp->IoStatus.Information = sizeof(c_find_program_request);
	}
	break;

	case find_module:
	{
		kprintf("[>][io_control] performing \"find_module\" operation. \n");
		c_find_module_request* find_module_request = reinterpret_cast<c_find_module_request*>(basic_request);
		find_module_request->module = process::find_module(find_module_request->pid, find_module_request->buffer);
		irp->IoStatus.Information = sizeof(c_find_module_request);
	}
	break;

	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	IofCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS __stdcall driver_entry(DRIVER_OBJECT* driver_object, UNICODE_STRING* registry_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	eprocess::completed = false;
	void* nxusbf = memory::find_module(s_w(L"nxusbf.sys"));
	if (nxusbf == nullptr)
	{
		kprintf("[-][main] nxusbf is not loaded. \n");
		return STATUS_UNSUCCESSFUL;
	}
	kprintf("[>][main] found nxusbf @ 0x%p \n", nxusbf);

	uintptr_t sig = reinterpret_cast<uintptr_t>(memory::pattern_scan(nxusbf, s("48 8D 05 ? ? ? ? 8B D7 48 89 03")));
	uintptr_t* table = reinterpret_cast<uintptr_t*>(sig + *reinterpret_cast<int32_t*>(sig + 3) + 7);

	memory::disable_wp();
	table[4] = reinterpret_cast<uintptr_t>(&io_control);
	memory::enable_wp();

	kprintf("[>][main] hook placed. \n");
	return STATUS_SUCCESS;
}