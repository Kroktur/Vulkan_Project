#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Instance;
		class Surface
		{
			using vkSurface = vk::raii::SurfaceKHR;
		public:
			Surface() = default;
			Surface(Instance* instance, GLFWwindow* window );

			vkSurface& Get();
			const vkSurface& Get() const;



		private:
			vkSurface m_surface = nullptr;
		};
	}
}