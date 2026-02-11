#pragma once
#include <fstream>
#include "Core/RessourcesManager.h"

inline std::unique_ptr<std::ifstream> LoadFile(const std::string& filePath)
{
	auto  file = std::make_unique<std::ifstream>(filePath, std::ios::ate | std::ios::binary);
	if (!file->is_open()) {
		throw std::runtime_error("failed to open file!");
	}
	return std::move(file);
}
using fileManager = KGR::ResourceManager<std::ifstream, KGR::TypeWrapper<>, LoadFile>;
