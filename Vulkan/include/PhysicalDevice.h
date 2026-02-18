#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Instance;
		class PhysicalDevice
		{
		public:
			enum class DeviceType
			{
				Best,
				Integrated,
				Discrete,
				Virtual,
				Cpu,
				Other
			};
			using vkFormat = vk::Format;
			using vkPhysicDevice = vk::raii::PhysicalDevice;
			PhysicalDevice() = default;
			PhysicalDevice(Instance* instance,DeviceType wanted) ;

			vkPhysicDevice& Get();
			const vkPhysicDevice& Get() const;

			vkFormat findSupportedFormat(const std::vector<vk::Format>& candidates,
				vk::ImageTiling tiling, vk::FormatFeatureFlags features);

		private: 
			static bool IsMatchingDeviceType(vk::PhysicalDeviceType get, DeviceType wanted);
			vkPhysicDevice m_device = nullptr;
		};
	}
}