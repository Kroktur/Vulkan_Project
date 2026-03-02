#include "Core/Texture.h"
#include "VulkanCore.h"
#include "Core/ManagerImple.h"

Texture::Texture(KGR::_Vulkan::Image&& image, KGR::_Vulkan::DescriptorSet&& set): m_image(std::move(image))
                                                                                  ,m_set(std::move(set))
{}

void Texture::Bind(const vk::raii::CommandBuffer* buffer, const vk::raii::PipelineLayout* layout, int set)
{
	buffer->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *layout, set, *m_set.Get(), nullptr);

}

std::unique_ptr<Texture> LoadTexture(const std::string& filePat, KGR::_Vulkan::VulkanCore* core)
{
	auto image = core->CreateImage(filePat);
	auto set = core->CreateSetForImage(&image);
	std::unique_ptr<Texture> result = std::make_unique<Texture>(std::move(image),std::move(set));
	return result;
}
