#include "Buffer.h"
#include "Device.h"
#include "CommandBuffers.h"
#include "Queue.h"
#include "PhysicalDevice.h"
KGR::_Vulkan::Buffer::Buffer(Device* device, Queue* queue, CommandBuffers* buffers, PhysicalDevice* phDevice, vk::BufferUsageFlags usage, size_t size, const void* data)
{


	vk::raii::Buffer       stagingBuffer({});
	vk::raii::DeviceMemory stagingBufferMemory({});
	createBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory,device,phDevice);
	void* mData = stagingBufferMemory.mapMemory(0, size);
	memcpy(mData, data, (size_t)size);
	stagingBufferMemory.unmapMemory();

	createBuffer(size, vk::BufferUsageFlagBits::eTransferDst | usage, vk::MemoryPropertyFlagBits::eDeviceLocal, m_buffer, m_bufferMemory, device, phDevice);

	copyBuffer(stagingBuffer, m_buffer, size,device,queue, buffers);
}

void KGR::_Vulkan::Buffer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                                        vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory,Device*device, PhysicalDevice* phDevice)
{
	vk::BufferCreateInfo bufferInfo{ .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };
	buffer = vk::raii::Buffer(device->Get(), bufferInfo);
	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties,phDevice) };
	bufferMemory = vk::raii::DeviceMemory(device->Get(), allocInfo);
	buffer.bindMemory(bufferMemory, 0);
}


 void KGR::_Vulkan::Buffer::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, Device* device, Queue* queue, CommandBuffers* commandBuffer)
{
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = commandBuffer->GetPool(), .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
	vk::raii::CommandBuffer       commandCopyBuffer = std::move(device->Get().allocateCommandBuffers(allocInfo).front());
	commandCopyBuffer.begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
	commandCopyBuffer.end();
	queue->Get().submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr);
	queue->Get().waitIdle();
}

 uint32_t KGR::_Vulkan::Buffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, PhysicalDevice* phDevice)
{
	vk::PhysicalDeviceMemoryProperties memProperties = phDevice->Get().getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}