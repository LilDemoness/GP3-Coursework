#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <unordered_map>

class DLLManager
{
public:
	static DLLManager& get_instance();

	// Load a DLL by its filename.
	bool load_dll(const std::string& dll_name);

	template <typename T>
	T get_function(const std::string& dll_name, const std::string& function_name)
	{
		// Find our desired dll.
		auto iterator = loaded_dlls_.find(dll_name);
		// Check for DLL Detection errors.
		if (iterator == loaded_dlls_.end())
		{
			std::cerr << "DLL not found: " << dll_name << std::endl;
			return nullptr;
		}

		// Load the desired function.
		FARPROC func = GetProcAddress(iterator->second, function_name.c_str());
		// Check for loading errors.
		if (!func)
		{
			std::cerr << "Failed to retrieve function: \"" << function_name << "\" from: " << dll_name << std::endl;
			return nullptr;
		}

		// Correct our raw function pointer to the desired type and return it.
		return reinterpret_cast<T>(func);
	}

	// Unload a specific DLL.
	void unload_dll(const std::string& dll_name);
	
	// Unload all loaded DLLs.
	void unload_all_dlls();

private:
	DLLManager() = default;
	~DLLManager();

	// Prevent copy & move.
	DLLManager(const DLLManager&) = delete;
	DLLManager& operator=(const DLLManager&) = delete;

	// Stores loaded DLLs (ID > DLL)
	std::unordered_map<std::string, HINSTANCE> loaded_dlls_;
};