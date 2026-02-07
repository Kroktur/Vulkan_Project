#include "AllFiles.h"

bool f11_pressed(GLFWwindow* window)
{
	return glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
}

int main()
{
    KGR::_GLFW myWindow;
    KGR::_GLFW::Init();
    KGR::_GLFW::AddHint(GLFW_CLIENT_API, GLFW_NO_API);
    KGR::_GLFW::AddHint(GLFW_RESIZABLE, GLFW_FALSE);
    myWindow.CreateMyWindow({ 1280, 720 }, "GC goes Vulkan", nullptr, nullptr);

    KGR::_Vulkan vulkan;
    vulkan.Init(&myWindow);

    uint32_t currentFrame = 0;

    do
    {
        KGR::_GLFW::PollEvent();

        auto& cb = vulkan.Begin();
        auto& currentImage = vulkan.GetCurrentImage();

        auto clearRange = vk::ImageSubresourceRange
        {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .levelCount = vk::RemainingMipLevels,
            .layerCount = vk::RemainingArrayLayers
        };

        vulkan.TransitionToTransferDst(cb, currentImage);
        cb.clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal,
            vk::ClearColorValue(10.0f, 0.2f, 5.0f, 1.0f), clearRange);
        vulkan.TransitionToPresent(cb, currentImage);

        vulkan.End();

    } while (!myWindow.ShouldClose());

    vulkan.WaitIdle();
    vulkan.Cleanup();

    myWindow.DestroyMyWindow();
    KGR::_GLFW::Destroy();

    return 0;
}