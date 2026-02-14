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
		

			vkBuffer& Get();
			const vkBuffer& Get() const;
			Buffer(Device* device, PhysicalDevice* phDevice, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags MemoryProperties,size_t size);
			template<typename elemType, size_t elemSize>
			void Upload(const std::array<elemType, elemSize>& data);
			template<typename elemType>
			void Upload(const std::vector<elemType>& data);
			void Upload(const void* data,size_t size);



			void Copy(Buffer* other, Device* device, Queue* queue, CommandBuffers* buffers);
			static void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
				vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory, Device* device, PhysicalDevice* phDevice);
			static void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, Device* device, Queue* queue, CommandBuffers* commandBuffer);
			static uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, PhysicalDevice* phDevice);
			void MapMemory(size_t size);
			void UnMapMemory();
		private:
			void* dest = nullptr;
			size_t m_size = 0;
			vkBuffer m_buffer = nullptr;
			vkBufferMemory m_bufferMemory = nullptr;
			bool m_isMapped = false;
		};

		
		

		template <typename elemType, size_t elemSize>
		void Buffer::Upload(const std::array<elemType, elemSize>& data)
		{
			Upload(data.data(), data.size() * (data.empty() ? 0 : sizeof(data[0])));
		}

		template <typename elemType>
		void Buffer::Upload(const std::vector<elemType>& data)
		{
			Upload(data.data(), data.size() * (data.empty() ? 0 : sizeof(data[0])));
		}

	}
}