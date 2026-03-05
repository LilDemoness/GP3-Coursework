#pragma once

#include "DLLManager.h"

DLLManager::~DLLManager()
{
	unload_all_dlls();
}


DLLManager& DLLManager::get_instance()
{
	static DLLManager instance;
	return instance;
}

bool DLLManager::load_dll(const std::string& dll_name)
{
	if (loaded_dlls_.find(dll_name) != loaded_dlls_.end())
	{
		std::cout << "DLL already loaded: " << dll_name << std::endl;
		return true;
	}

	HINSTANCE dll_handle = LoadLibrary(dll_name.c_str());
	if (!dll_handle)
	{
		std::cerr << "Failed to load DLL: " << dll_name << std::endl;
		return false;
	}

	loaded_dlls_[dll_name] = dll_handle;
	std::cout << "Successfully loaded DLL: " << dll_name << std::endl;
	return true;
}


void DLLManager::unload_dll(const std::string& dll_name)
{
	auto iterator = loaded_dlls_.find(dll_name);

	if (iterator == loaded_dlls_.end())
	{
		std::cerr << "DLL not found: " << dll_name << std::endl;
		return;
	}

	FreeLibrary(iterator->second);
	loaded_dlls_.erase(iterator);
	std::cout << "Unloaded DLL: " << dll_name << std::endl;
}
void DLLManager::unload_all_dlls()
{
	for (auto& [dll_name, dll_handle] : loaded_dlls_)
	{
		FreeLibrary(dll_handle);
		std::cout << "Unloaded DLL: " << dll_name << std::endl;
	}

	loaded_dlls_.clear();
}