#pragma once
#include "common.h"

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace vk
{
class Core
{
  const char *sValidationLayer = "VK_LAYER_KHRONOS_validation";
  //  const char *sDeviceExtensions = ;
  static const inline std::vector<const char *> sRequiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  VkInstance mInstance = {};
  VkDebugUtilsMessengerCreateInfoEXT mDebugCreateInfo = {};
  VkDebugUtilsMessengerEXT mDebugMessenger = {};
  GLFWwindow *mWindow;
  VkSurfaceKHR mWindowSurface;

  std::vector<VkPhysicalDevice> mPhysicalDevices;
  VkPhysicalDevice mPhysicalDevice;

  VkDevice mLogicalDevice;

  VkQueue mGraphicsQueue;
  VkQueue mPresentQueue;

  std::pair<s32, s32> mFamilyPair;

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR mCapabilities;
    std::vector<VkSurfaceFormatKHR> mFormats;
    std::vector<VkPresentModeKHR> mPresentModes;
    bool IsAdequate() const { return !mFormats.empty() && !mPresentModes.empty(); }
  } mSwapChainSupportDetails;

  VkSurfaceFormatKHR mSwapChainSurfaceFormat;
  VkPresentModeKHR mSwapChainPresentMode;
  VkExtent2D mSwapChainExtent;
  std::vector<VkImage> mSwapChainImages;
  std::vector<VkImageView> mSwapChainImageViews;
  VkFormat mSwapChainImageFormat;

  VkSwapchainKHR mSwapChain;
  VkRenderPass mDefaultRenderPass;

  bool mDebugEnabled;

public:
  explicit Core(bool debugEnabled);

private:
  void Init();
  bool CheckForValidationLayer(const char *layerName);
  void CreateInstance();
  VkDebugUtilsMessengerCreateInfoEXT CreateDebugCreateInfo();
  void FindDevices();
  void CreateLogicalDevice();

  // the first will be the graphics family, second present family
  std::pair<s32, s32> FindQueueFamily();

  void CreateSwapChain();
  VkImageView CreateImageView(VkImage image, VkFormat format);
  void CreateRenderPass();
};
} // namespace vk
