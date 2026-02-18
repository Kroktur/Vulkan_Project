#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Device;
		class PhysicalDevice;
		class Image
		{
		public:
			
			using vkImage = vk::raii::Image;
			using vkImageView = vk::raii::ImageView;
			using vkDeviceMemory = vk::raii::DeviceMemory;
			Image() = default;
			Image(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
				vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,Device* device,PhysicalDevice* physicalDevice);
			void CreateView(vk::Format format, vk::ImageAspectFlags aspectFlags, Device* device);
			vkImageView& GetView();

			const vkImageView& GetView() const;

			vkImage& Get();

			const vkImage& Get() const;


			static void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
			                        vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image& image,
			                        vk::raii::DeviceMemory& imageMemory,Device* device,PhysicalDevice* physicalDevice);


			static vkImageView createImageView(vk::raii::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags,Device* device);

			uint32_t GetHeight() const 
			{
				return m_height;
			}
			uint32_t GetWidth() const
			{
				return m_width;
			}
		private:
			uint32_t m_width = 0, m_height = 0;
			vkImage m_image = nullptr;
			vkImageView m_imageView = nullptr;
			vkDeviceMemory m_imageMemory = nullptr;
		};
	}
}