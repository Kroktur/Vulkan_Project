#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class PhysicalDevice;
		class Surface;


		class Device
		{
		public:
			using vkDevice = vk::raii::Device;
			Device() = default;
			Device(PhysicalDevice* device, Surface* surface, ui32t count = 1);


			vkDevice& Get();

			const vkDevice& Get() const;

			uint32_t GetQueueIndex() const
			{
				return m_queueIndex;
			}
			uint32_t GetQueueCount() const
			{
				return m_queueCount;
			}
		private:
			vkDevice m_device = nullptr;
			uint32_t m_queueIndex;
			uint32_t m_queueCount;
		};
	}
}