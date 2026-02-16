#include "Buffer.h"
#include "Device.h"
#include "CommandBuffers.h"
#include "Queue.h"
#include "PhysicalDevice.h"

KGR::_Vulkan::Buffer::vkBuffer& KGR::_Vulkan::Buffer::Get()
{
	return m_buffer;
}

const KGR::_Vulkan::Buffer::vkBuffer& KGR::_Vulkan::Buffer::Get() const
{
	return m_buffer;
}

KGR::_Vulkan::Buffer::Buffer(Device* device, PhysicalDevice* phDevice,
	vk::BufferUsageFlags usage, vk::MemoryPropertyFlags MemoryProperties,size_t size)
{
	m_size = size;
	createBuffer(m_size,  usage, MemoryProperties, m_buffer, m_bufferMemory, device, phDevice);
}

void KGR::_Vulkan::Buffer::Upload(const void* data, size_t size)
{	
	if (!dest)
		throw std::runtime_error("Buffer not mapped");
	if (size > m_size)
		throw std::out_of_range("impossible to upload");
	std::memcpy(dest, data, (size_t)size);
}


void KGR::_Vulkan::Buffer::Copy(Buffer* other, Device* device, Queue* queue, CommandBuffers* buffers)
{
	if (other->m_size > m_size)
		throw std::out_of_range("impossible to copy");
	copyBuffer(other->m_buffer, m_buffer, other->m_size , device, queue, buffers);
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

void KGR::_Vulkan::Buffer::MapMemory(size_t size)
{
	if (size > m_size)
		throw std::out_of_range("impossible to upload");
	dest = m_bufferMemory.mapMemory(0, size);

}

void KGR::_Vulkan::Buffer::UnMapMemory()
{
	m_bufferMemory.unmapMemory();
	dest = nullptr;
}


