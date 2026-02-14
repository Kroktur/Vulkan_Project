#define STB_IMAGE_IMPLEMENTATION
#include "Core/ManagerImple.h"


std::unique_ptr<stbi_uc*> LoadFile2(const std::string& filePath, int* texWidth, int* texHeight, int* texChannels)
{

	return std::make_unique<stbi_uc*>(stbi_load(filePath.c_str(), texWidth, texHeight, texChannels, STBI_rgb_alpha));
}
