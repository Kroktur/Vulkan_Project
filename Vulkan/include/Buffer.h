#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class CommandBuffers;
		class Device;
		class Queue;
		class PhysicalDevice;
		class Buffer
		{
		public:
			using vkBuffer = vk::raii::Buffer;
			using vkBufferMemory = vk::raii::DeviceMemory;
			Buffer() = default;
			template<typename elemType,size_t elemSize>
			Buffer(Device* device, Queue* queue, CommandBuffers* buffers, PhysicalDevice* phDevice, vk::BufferUsageFlags usage,const std::array<elemType,elemSize>& data);
			Buffer(Device* device, Queue* queue, CommandBuffers* buffers, PhysicalDevice* phDevice, vk::BufferUsageFlags usage, size_t size, const void* data);

			vkBuffer& Get()
			{
				return m_buffer;
			}
			const vkBuffer& Get() const
			{
				return m_buffer;
			}
		private:
			static void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
				vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory,Device* device, PhysicalDevice* phDevice);

			static void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size,Device* device,Queue* queue,CommandBuffers* commandBuffer);

			static uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, PhysicalDevice* phDevice);

			vkBuffer m_buffer = nullptr;
			vkBufferMemory m_bufferMemory = nullptr;
		};

		template <typename elemType, size_t elemSize>
		Buffer::Buffer(Device* device, Queue* queue, CommandBuffers* buffers, PhysicalDevice* phDevice, vk::BufferUsageFlags usage, const std::array<elemType, elemSize>& data) : Buffer(device, queue, buffers, phDevice, (data.empty() ? 0 : sizeof(data[0]))* data.size(), data.data())
		{}

		
	}
}