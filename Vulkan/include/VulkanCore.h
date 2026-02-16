#pragma once

#include <vector>

#include "Buffer.h"
#include "Global.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Surface.h"
#include "Device.h"
#include "ImagesViews.h"
#include "Pipeline.h"
#include "Queue.h"
#include "SwapChain.h"
#include "CommandBuffers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <stb_image.h>

#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "SyncObject.h"

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};



namespace KGR
{
	namespace _Vulkan
	{
		class VulkanCore
		{
		public:
			VulkanCore(GLFWwindow* window);
			void initVulkan();
			void mainLoop();
			void recreateSwapChain();
			std::uint32_t PresentImage();


			void recordCommandBuffer(uint32_t imageIndex, vk::raii::CommandBuffer& buffer);

			void transition_image_layout(
				vk::Image               image,
				vk::ImageLayout         old_layout,
				vk::ImageLayout         new_layout,
				vk::AccessFlags2        src_access_mask,
				vk::AccessFlags2        dst_access_mask,
				vk::PipelineStageFlags2 src_stage_mask,
				vk::PipelineStageFlags2 dst_stage_mask, vk::ImageAspectFlags    image_aspect_flags, vk::raii::CommandBuffer& buffer);

			void drawFrame();

			
	
			

		
			
			


			


			void createTextureImage();
			void createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image& image, vk::raii::DeviceMemory& imageMemory);
			void transitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
			void copyBufferToImage(const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);
			void createTextureImageView();
			void createTextureSampler();
			vk::raii::ImageView createImageView(vk::raii::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);

			void createDepthResources();


			static vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features,PhysicalDevice* phDevice);

			bool hasStencilComponent(vk::Format format);

			static vk::Format findDepthFormat(PhysicalDevice* phDevice);

			void updateUniformBuffer(uint32_t currentImage);
			// callBack for instance
			static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
		private:
			// window
			GLFWwindow* window = nullptr;

			Instance               instance;
			Surface                surface;
			PhysicalDevice         physicalDevice ;
			Device				   device;
			Queue				   queue ;
			SwapChain              swapChain;
			ImagesViews            swapChainImageViews;

			DescriptorLayouts descriptorSetLayout;
			DescriptorPool descriptorPool;
			std::vector<DescriptorSet> descriptorSets;
			Pipeline               graphicsPipeline;

			Buffer vertexBuffer;
			Buffer indexBuffer;
			CommandBuffers         commandBuffers;


			std::vector<Buffer> uniformBuffers;

			SyncObject syncObject;


			vk::raii::Image textureImage = nullptr;
			vk::raii::DeviceMemory textureImageMemory = nullptr;
			vk::raii::ImageView    textureImageView = nullptr;
			vk::raii::Sampler      textureSampler = nullptr;

			vk::raii::Image        depthImage = nullptr;
			vk::raii::DeviceMemory depthImageMemory = nullptr;
			vk::raii::ImageView    depthImageView = nullptr;

			std::vector<const char*> requiredDeviceExtension = {
				vk::KHRSwapchainExtensionName };
		};
	}
}
