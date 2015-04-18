#include <string>
#include <Windows.h>
#include <Psapi.h>
#include "process.hpp"

#pragma comment(lib, "psapi.lib")

std::string get_process_name(DWORD process_id) {
	char process_name[MAX_PATH] = TEXT("<unknown>");

	HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, process_id);

	if(process_handle != nullptr) {
		HMODULE module_handle;
		DWORD size_needed;

		if(::EnumProcessModules(process_handle, &module_handle, sizeof(module_handle), &size_needed)) {
			::GetModuleBaseNameA(process_handle, module_handle, process_name, sizeof(process_name) / sizeof(char));
		}
	}

	::CloseHandle(process_handle);

	return std::string(process_name);
}