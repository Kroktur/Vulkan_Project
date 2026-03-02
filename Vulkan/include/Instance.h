#pragma once
#include "Global.h"

namespace KGR
{
	namespace _Vulkan
	{
		class Instance;

		struct AppInfo
		{
			friend Instance;
			const char* appName = "Basic_Api";
			const char* engineName = "None";
			std::uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
			std::uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
			std::uint32_t version = vk::ApiVersion14;
		private:
			void Create();
			vk::ApplicationInfo m_info ;
		};


		class Instance
		{
		public:
			using validCallBack = VkBool32(*)(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);
			using vkInstance = vk::raii::Instance;
			using vkUtilMessenger = vk::raii::DebugUtilsMessengerEXT;

			Instance() = default;
			Instance(AppInfo&&, std::vector<char const*> validationLayers);

			template<validCallBack cb>
			void setupDebugMessenger();

			vkInstance& Get();
			const vkInstance& Get() const;
			AppInfo GetInfo() const;

		private:
			static std::vector<char const*> getRequiredExtensions();


			AppInfo m_info;
			vkInstance m_instance = nullptr;
			vkUtilMessenger m_debugMessenger = nullptr;
		};

		template <Instance::validCallBack cb>
		void Instance::setupDebugMessenger()
		{
			if (!enableValidationLayers)
				return;

			vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
			vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
			vk::DebugUtilsMessengerCreateInfoEXT  debugUtilsMessengerCreateInfoEXT{
				.messageSeverity = severityFlags,
				.messageType = messageTypeFlags,
				.pfnUserCallback = cb };
			m_debugMessenger = m_instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
		}
	}
}
