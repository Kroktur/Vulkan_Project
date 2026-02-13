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
			std::uint32_t AcquireNextImage();
			std::uint32_t PresentImage();


			void recordCommandBuffer(uint32_t imageIndex, vk::raii::CommandBuffer& buffer);

			void transition_image_layout(
				uint32_t                imageIndex,
				vk::ImageLayout         old_layout,
				vk::ImageLayout         new_layout,
				vk::AccessFlags2        src_access_mask,
				vk::AccessFlags2        dst_access_mask,
				vk::PipelineStageFlags2 src_stage_mask,
				vk::PipelineStageFlags2 dst_stage_mask, vk::raii::CommandBuffer& buffer);

			void createSyncObjects();
	
			

			void drawFrame();
			


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
			Pipeline               graphicsPipeline;

			Buffer vertexBuffer;
			Buffer indexBuffer;

			CommandBuffers         commandBuffers;


			std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
			std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
			std::vector<vk::raii::Fence>     inFlightFences;
			uint32_t                         frameIndex = 0;



			std::uint32_t imageIndex;

			std::vector<const char*> requiredDeviceExtension = {
				vk::KHRSwapchainExtensionName };
		};
	}
}
