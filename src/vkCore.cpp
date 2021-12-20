#include "vkCore.hpp"

#include "common.h"

#include <GLFW/glfw3.h>
#include <cassert>
#include <fmt/core.h>
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk
{

// TODO: put this in a class?
VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagBitsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *callBackData,
    void *userData)
{
//  fmt::print("Validation layer: {}\n", callBackData->pMessage);
  return VK_FALSE;
}

static VkResult CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *createInfo,
    const VkAllocationCallbacks *allocator, VkDebugUtilsMessengerEXT *debugMessenger)
{
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, createInfo, allocator, debugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

Core::Core(bool debugEnabled) : mDebugEnabled(debugEnabled)
{
  Init();
}

void Core::Init()
{
  // Create the glfw window
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  mWindow = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
  glfwSetWindowUserPointer(mWindow, this);

  // TODO: resizing
  //  glfwSetFramebufferSizeCallback(mWindow, FramebufferResizeCallback);
  // Create the instance, debug messenger, and window surface
  CreateInstance();
  // Pick the physical device, currently just picking one, need to add an actual rating system
  FindDevices();

  // Create the logical device, and the graphics and present queues
  CreateLogicalDevice();

  CreateSwapChain();

  // Create the default render pass
  // TODO: move into utility function
  CreateRenderPass();
}

bool Core::CheckForValidationLayer(const char *layerName)
{
  u32 layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  bool layerFound = false;
  for (const auto &layerProperties : availableLayers) {
    if (strcmp(layerName, layerProperties.layerName) == 0) {
      layerFound = true;
      break;
    }
  }

  return layerFound;
}

void Core::CreateInstance()
{
  if (!CheckForValidationLayer(sValidationLayer)) {
    passert("Validation layers not available\n", 0);
  }
  VkApplicationInfo appInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = nullptr,
      .pApplicationName = "CHANGE NAME LATER",
      .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(0, 0, 1),
      .apiVersion = VK_API_VERSION_1_2,
  };

  // get the required glfw extensions and add the debug extension if we're in debug mode
  u32 glfwExtensionCount = 0;
  const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  if (mDebugEnabled) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  mDebugCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugCallback,
      .pUserData = nullptr,
  };

  VkInstanceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&mDebugCreateInfo,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = mDebugEnabled ? 1u : 0u,
      .ppEnabledLayerNames = mDebugEnabled ? &sValidationLayer : nullptr,
      .enabledExtensionCount = (u32)extensions.size(),
      .ppEnabledExtensionNames = extensions.data(),
  };
  // create the vulkan instance
  passert("Failed to create instance", vkCreateInstance(&createInfo, nullptr, &mInstance) == VK_SUCCESS);
  // create the debug messenger
  auto tmp = CreateDebugCreateInfo();
  passert("Failed to setup the debug messenger",
      CreateDebugUtilsMessenger(mInstance, &mDebugCreateInfo, nullptr, &mDebugMessenger) == VK_SUCCESS);

  // create a surface for the window
  passert("failed to create window surface",
      glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mWindowSurface) == VK_SUCCESS);
}

VkDebugUtilsMessengerCreateInfoEXT Core::CreateDebugCreateInfo()
{
  return {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)DebugCallback,
      .pUserData = nullptr,
  };
}
void Core::FindDevices()
{
  u32 deviceCount = 0;
  passert("failed to find vulkan physical devices",
      vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr) == VK_SUCCESS);
  mPhysicalDevices.resize(deviceCount);
  vkEnumeratePhysicalDevices(mInstance, &deviceCount, mPhysicalDevices.data());
  for (const auto &device : mPhysicalDevices) {
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    u32 extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceExtensions.data());
    // Just pick the last device for now
    mPhysicalDevice = device;
  }
}
void Core::CreateLogicalDevice()
{
  mFamilyPair = FindQueueFamily();
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::unordered_set<u32> uniqueQueueFamilies = {(u32)mFamilyPair.first, (u32)mFamilyPair.second};

  // TODO: add some sort of queue priority
  f32 queuePriorite = 1.0f;
  for (auto queueFamily : uniqueQueueFamilies) {
    queueCreateInfos.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorite,
    });
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.samplerAnisotropy = VK_TRUE;
  VkDeviceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = (u32)queueCreateInfos.size(),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = mDebugEnabled ? 1u : 0u,
      .ppEnabledLayerNames = mDebugEnabled ? &sValidationLayer : nullptr,
      .enabledExtensionCount = (u32)sRequiredExtensions.size(),
      .ppEnabledExtensionNames = sRequiredExtensions.data(),
      .pEnabledFeatures = &deviceFeatures,
  };
  passert("Failed to create logical device",
      vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice) == VK_SUCCESS);

  vkGetDeviceQueue(mLogicalDevice, mFamilyPair.first, 0, &mGraphicsQueue);
  vkGetDeviceQueue(mLogicalDevice, mFamilyPair.first, 0, &mPresentQueue);
}

std::pair<s32, s32> Core::FindQueueFamily()
{
  // TODO: refractor into a more useful struct
  std::pair<s32, s32> ret{-1, -1};

  u32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());
  for (u32 i = 0; i < queueFamilyCount; i++) {
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mWindowSurface, &presentSupport);
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      ret.first = i;
    }
    if (presentSupport) {
      ret.second = i;
    }
    if (ret.first != -1 && ret.second != -1) {
      break;
    }
  }
  return ret;
}

void Core::CreateSwapChain()
{
  // populate mSwapChainSupportDetails with the information about what the current physical device supports
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mWindowSurface, &mSwapChainSupportDetails.mCapabilities);

  u32 formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mWindowSurface, &formatCount, nullptr);

  if (formatCount != 0) {
    mSwapChainSupportDetails.mFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        mPhysicalDevice, mWindowSurface, &formatCount, mSwapChainSupportDetails.mFormats.data());
  }

  u32 presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, mWindowSurface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    mSwapChainSupportDetails.mPresentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        mPhysicalDevice, mWindowSurface, &presentModeCount, mSwapChainSupportDetails.mPresentModes.data());
  }

  // attempt to find the "best" format for the swap chain
  mSwapChainSurfaceFormat = mSwapChainSupportDetails.mFormats[0];
  for (const auto &availableFormat : mSwapChainSupportDetails.mFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
        && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      mSwapChainSurfaceFormat = availableFormat;
      break;
    }
  }

  // find the the "best" present mode for the swap chain
  mSwapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  for (const auto &availablePresentMode : mSwapChainSupportDetails.mPresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      mSwapChainPresentMode = availablePresentMode;
      break;
    }
  }

  // find the extent for the swap chain
  if (mSwapChainSupportDetails.mCapabilities.currentExtent.width != UINT32_MAX) {
    mSwapChainExtent = mSwapChainSupportDetails.mCapabilities.currentExtent;
  } else {
    s32 width = 0;
    s32 height = 0;
    glfwGetFramebufferSize(mWindow, &width, &height);
    VkExtent2D actualExtent = {(u32)width, (u32)height};
    actualExtent.width = std::max(mSwapChainSupportDetails.mCapabilities.minImageExtent.width,
        std::min(mSwapChainSupportDetails.mCapabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(mSwapChainSupportDetails.mCapabilities.minImageExtent.height,
        std::min(mSwapChainSupportDetails.mCapabilities.maxImageExtent.height, actualExtent.height));
    mSwapChainExtent = actualExtent;
  }
  // create the swap chain
  u32 imageCount = mSwapChainSupportDetails.mCapabilities.minImageCount + 1;
  if (mSwapChainSupportDetails.mCapabilities.maxImageCount > 0
      && imageCount > mSwapChainSupportDetails.mCapabilities.maxImageCount) {
    imageCount = mSwapChainSupportDetails.mCapabilities.maxImageCount;
  }

  // TODO: use c-style construction
  u32 queueFamilyIndices[] = {(u32)mFamilyPair.first, (u32)mFamilyPair.second};
  VkSwapchainCreateInfoKHR createInfo = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = mWindowSurface,
      .minImageCount = imageCount,
      .imageFormat = mSwapChainSurfaceFormat.format,
      .imageColorSpace = mSwapChainSurfaceFormat.colorSpace,
      .imageExtent = mSwapChainExtent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .preTransform = mSwapChainSupportDetails.mCapabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = mSwapChainPresentMode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
  };

  if (mFamilyPair.first != mFamilyPair.second) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  passert("Failed to create swap chain",
      vkCreateSwapchainKHR(mLogicalDevice, &createInfo, nullptr, &mSwapChain) == VK_SUCCESS);
  vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount, nullptr);
  mSwapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount, mSwapChainImages.data());

  mSwapChainImageFormat = mSwapChainSurfaceFormat.format;

  // Create the swap chain image views
  mSwapChainImageViews.resize(mSwapChainImages.size());
  for (u64 i = 0; i < mSwapChainImages.size(); i++) {
    mSwapChainImageViews[i] = CreateImageView(mSwapChainImages[i], mSwapChainImageFormat);
  }
}

VkImageView Core::CreateImageView(VkImage image, VkFormat format)
{
  VkImageViewCreateInfo viewInfo{};

  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;

  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;

  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  passert(
      "Faled to create image view", vkCreateImageView(mLogicalDevice, &viewInfo, nullptr, &imageView) == VK_SUCCESS);

  return imageView;
}
void Core::CreateRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = mSwapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;

  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;

  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  passert("failed to create render pass",
      vkCreateRenderPass(mLogicalDevice, &renderPassInfo, nullptr, &mDefaultRenderPass) == VK_SUCCESS)
}

} // namespace vk
