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

#include "stb_image.h"

std::unique_ptr<stbi_uc*> LoadFile2(const std::string& filePath, int* texWidth, int* texHeight, int* texChannels);
using STBManager = KGR::ResourceManager<stbi_uc*, KGR::TypeWrapper<int*,int*,int*>, LoadFile2>;
