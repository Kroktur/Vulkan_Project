#pragma once
#include "Global.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>


#include "DescriptorSet.h"
#include "Image.h"
#include "RessourcesManager.h"

namespace KGR::_Vulkan
{
	class VulkanCore;
}


class Texture
{
public:
	Texture(KGR::_Vulkan::Image&& image, KGR::_Vulkan::DescriptorSet&& set);
	void Bind(const vk::raii::CommandBuffer* buffer,const vk::raii::PipelineLayout* layout,int set);
private:
	KGR::_Vulkan::Image m_image;
	KGR::_Vulkan::DescriptorSet m_set;
};

struct TextureComponent
{

	TextureComponent() = default;
	void SetSize(uint32_t size)
	{
		m_textures.resize(size);
	}
	size_t Size() const
	{
		return m_textures.size();
	}
	void AddTexture(uint32_t index, Texture* texture)
	{
		m_textures[index] = texture;
	}
	const Texture* GetTexture(uint32_t index) const 
	{
		return m_textures[index];
	}
	Texture* GetTexture(uint32_t index)
	{
		return m_textures[index];
	}
	std::vector<Texture*>& GetAllTextures()
	{
		return m_textures;
	};
private:
	std::vector<Texture*> m_textures;
};


std::unique_ptr<Texture> LoadTexture(const std::string& filePat, KGR::_Vulkan::VulkanCore* core);

using TextureLoader = KGR::ResourceManager<Texture, KGR::TypeWrapper<KGR::_Vulkan::VulkanCore*>, LoadTexture>;