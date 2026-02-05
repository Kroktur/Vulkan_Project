#include "VKFiles.h"
#include "VK.h"

vk::raii::Instance KGR::_Vulkan::InitInstance()
{
	m_appInfo = 
	{
		.pApplicationName = "GCVulkan",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "None",
		.engineVersion = VK_MAKE_VERSION(0, 1, 0),
		.apiVersion = vk::ApiVersion14
	};

	m_requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());

	CheckLayerProperties();
	GetExtensions();
	AddExtensions();
	VerifyExtensions();

	m_instanceCreateInfo =
	{
		.pApplicationInfo = &m_appInfo,
		.enabledLayerCount = static_cast<glm::uint32_t>(m_requiredLayers.size()),
		.ppEnabledLayerNames = m_requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(m_activeExtensions.size()),
		.ppEnabledExtensionNames = m_activeExtensions.data()
	};

	return { m_vkContext, m_instanceCreateInfo };
}

vk::raii::PhysicalDevice KGR::_Vulkan::CreatePhysicalDevice(vk::raii::Instance& instance)
{
	vk::raii::PhysicalDevice result = nullptr;

	return result;
}

void KGR::_Vulkan::CheckLayerProperties()
{
	const auto layerProperties = m_vkContext.enumerateInstanceLayerProperties();
	if (std::ranges::any_of(m_requiredLayers, [&layerProperties](auto const& requiredLayer)
		{
			return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
				{
					return strcmp(layerProperty.layerName, requiredLayer) == 0;
				});
		}))
		throw std::runtime_error("One or more required layers are not supported !");
}

void KGR::_Vulkan::GetExtensions()
{
	m_glfwExtensions = glfwGetRequiredInstanceExtensions(&m_glfwExtensionCount);
	m_extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();
}

void KGR::_Vulkan::AddExtensions()
{
	for (uint32_t i = 0; i < m_glfwExtensionCount; ++i)
		m_activeExtensions.push_back(m_glfwExtensions[i]);
}

void KGR::_Vulkan::VerifyExtensions()
{
	for (uint32_t i = 0; i < m_glfwExtensionCount; ++i)
		if (std::ranges::none_of(m_extensionProperties, [glfwExtension = m_glfwExtensions[i]](auto const& extensionProperty)
			{
				return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
			}))
			throw std::runtime_error("Required GLFW extensions not supported");
}
