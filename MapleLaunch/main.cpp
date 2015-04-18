#pragma comment(lib, "advapi32.lib")

#include <iostream>
#include <cstdlib>
#include <algorithm>

#include <Windows.h>
#include <Psapi.h>

#include "process.hpp"

ULONG get_string_reg_key(HKEY hKey, const std::string &key_name, std::string &key_value);

int main(int argc, char *argv[]) {
	// Read the maple path from the registry
	HKEY hKey;
	LONG error_code;
	std::string maple_reg_path = "";
	
	error_code = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wizet\\MapleStory", 0, KEY_READ, &hKey);

	if(error_code == ERROR_SUCCESS) {
		get_string_reg_key(hKey, TEXT("Executable"), maple_reg_path);
	} else if(error_code == ERROR_FILE_NOT_FOUND) {
		std::cout << "MapleStory registry key not found" << std::endl;
		std::system("PAUSE");
		return 0;
	}

	if(maple_reg_path.empty()) {
		std::cout << "Failed to find the path to MapleStory" << std::endl;
		std::system("PAUSE");
		return 0;
	} else {
		std::cout << "Maplestory path: " << maple_reg_path << std::endl;
	}


	std::string maple_cmdline = "\"" + maple_reg_path + "\"" + " GameLaunching";
	bool found_process = false;

	// Launch MapleStory
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);

	if(!CreateProcessA( NULL, LPSTR(maple_cmdline.c_str()), NULL, NULL, false, 0, NULL, NULL, &si, &pi )) {
		std::cout << "Failed to launch MapleStory: " << GetLastError() << std::endl;
		std::system("PAUSE");
		return 0;
	}

	std::cout << "Waiting for process..." << std::endl;

	while(!found_process) {
		DWORD process_list[1024], size_needed, process_count;
		unsigned int i;

		if(!EnumProcesses(process_list, sizeof(process_list), &size_needed)) {
			return 0;
		}

		process_count = size_needed / sizeof(DWORD);

		for(i = 0; i < process_count; i++) {
			if(process_list[i] != 0) {
				std::string process_name = get_process_name(process_list[i]);
				std::transform(std::begin(process_name), std::end(process_name), std::begin(process_name), ::tolower);

				if(process_name != "<unknown>") {
					if(process_name == "blackxchg.aes") {
						std::cout << "Found process " << process_name << std::endl;
						std::cout << "Ending process " << process_name << std::endl;

						HANDLE process_handle = ::OpenProcess(PROCESS_TERMINATE, FALSE, process_list[i]);
						if(process_handle == nullptr) {
							std::cout << "Failed to get the process handle" << std::endl;
							std::system("PAUSE");

							return 0;
						}

						if(::TerminateProcess(process_handle, 0) == FALSE) {
							std::cout << "Failed to terminate the process" << std::endl;
							std::system("PAUSE");
							return 0;
						}

						std::cout << "Process ended" << std::endl;

						::CloseHandle(process_handle);

						found_process = true;
						break;
					}
				}
			}
		}
	}

	return 0;
}

ULONG get_string_reg_key(HKEY hKey, const std::string &key_name, std::string &key_value) {
	key_value = "";

	char szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG error;
	error = RegQueryValueEx(hKey, key_name.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if(error == ERROR_SUCCESS) {
		key_value = szBuffer;
	}

	return error;
}