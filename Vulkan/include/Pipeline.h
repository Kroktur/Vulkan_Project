#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		struct ShaderInfo
		{
			const char* ShaderPath;
			const char* vertexMain;
			const char* fragmentMain;
		};
		class Device;
		class SwapChain;

		class Pipeline
		{
		public:
			using vkPipeline = vk::raii::Pipeline;
			using vkPipelineLayout = vk::raii::PipelineLayout;
			Pipeline() = default;
			Pipeline(const ShaderInfo& shaderInfo,Device* device,SwapChain* swapChain);

			vkPipelineLayout& GetLayout();

			const vkPipelineLayout& GetLayout() const;

			vkPipeline& Get();

			const vkPipeline& Get() const;

		private:
			vkPipelineLayout m_layout = nullptr;
			vkPipeline m_pipeline = nullptr;
		};
	}
}
