#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class SwapChain;
		class Device;
		class ImagesViews
		{
		public:

		/*	e1D = VK_IMAGE_VIEW_TYPE_1D,
				e2D = VK_IMAGE_VIEW_TYPE_2D,
				e3D = VK_IMAGE_VIEW_TYPE_3D,
				eCube = VK_IMAGE_VIEW_TYPE_CUBE,
				e1DArray = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
				e2DArray = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
				eCubeArray = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY*/

			enum class ViewType
			{
				vt1D,
				vt2D,
				vt3D,
				vtCube,
				vt1DArray,
				vt2DArray,
				vtCubeArray
			};



			using vkImageView = vk::raii::ImageView;
			using vkImagesViews = std::vector<vkImageView>;
			ImagesViews() = default;
			ImagesViews(SwapChain* swapChain,Device* device,const ViewType& viewType);

			vkImagesViews& Get();
			const vkImagesViews& Get() const;
		private:
			static vk::ImageViewType KGRToVulkan(const ViewType& viewType);
			vkImagesViews m_views;
		};
	}
}