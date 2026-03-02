#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Device;
		class Queue
		{
		public:
			using vkQueue = vk::raii::Queue;
			Queue() = default;
			Queue(Device* device,uint32_t id = 0);

			vkQueue& Get();

			const vkQueue& Get() const;

		private:
			vkQueue m_queue = nullptr; 
		};
	}
}