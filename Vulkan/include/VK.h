#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vma/vk_mem_alloc.h>

namespace KGR
{
	struct Extensions
	{

	};

	struct LayerProperty
	{
		
	};

	class _Vulkan
	{
	public:
		vk::raii::Instance InitInstance();
		vk::raii::PhysicalDevice CreatePhysicalDevice(vk::raii::Instance& instance);

	private:

		void CheckLayerProperties();
		void GetExtensions();
		void AddExtensions();
		void VerifyExtensions();

	private:
		vk::ApplicationInfo m_appInfo;
		vk::InstanceCreateInfo m_instanceCreateInfo;

		vk::raii::Context m_vkContext;

		std::vector<const char*> m_requiredLayers;
		const std::vector<char const*> m_validationLayers = 
		{ "VK_LAYER_KHRONOS_validation" };

		uint32_t m_glfwExtensionCount = 0;
		const char** m_glfwExtensions;
		std::vector<vk::ExtensionProperties> m_extensionProperties;
		std::vector<const char*> m_activeExtensions;
	};
}