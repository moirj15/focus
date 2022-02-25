#pragma once
#include "../../common.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

struct UniformBufferObject
{
  alignas(16) glm::mat4 mModel;
  alignas(16) glm::mat4 mView;
  alignas(16) glm::mat4 mProj;
};

struct Vertex
{
  glm::vec2 mPos;
  glm::vec3 mColor;
  glm::vec2 mTexCoord;

  static VkVertexInputBindingDescription GetBindingDescription()
  {
    VkVertexInputBindingDescription bindingDescription{};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }

  static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
  {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, mPos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, mColor);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, mTexCoord);

    return attributeDescriptions;
  }
};

class TriangleApp
{
  GLFWwindow *mWindow{nullptr};
  VkInstance mInstance{};
  VkDebugUtilsMessengerEXT mDebugMessenger{};
  VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
  VkDevice mDevice{};
  VkQueue mGraphicsQueue{};
  VkSurfaceKHR mSurface{};
  VkQueue mPresentQueue{};
  VkSwapchainKHR mSwapChain{};
  std::vector<VkImage> mSwapChainImages;
  VkFormat mSwapChainImageFormat;
  VkExtent2D mSwapChainExtent;
  std::vector<VkImageView> mSwapChainImageViews;
  VkRenderPass mRenderPass;
  VkPipelineLayout mPipelineLayout;
  VkPipeline mGraphicsPipeline;
  std::vector<VkFramebuffer> mSwapChainFramebuffers;
  VkCommandPool mCommandPool;
  std::vector<VkCommandBuffer> mCommandBuffers;
  std::vector<VkSemaphore> mImageAvailableSemaphores;
  std::vector<VkSemaphore> mRenderFinishedSemaphores;
  std::vector<VkFence> mInFlightFences;
  std::vector<VkFence> mImagesInFlight;
  u64 mCurrentFrame = 0;
  VkBuffer mVertexBuffer;
  VkDeviceMemory mVertexBufferMemory;
  VkBuffer mIndexBuffer;
  VkDeviceMemory mIndexBufferMemory;
  VkDescriptorSetLayout mDescriptorSetLayout;
  std::vector<VkBuffer> mUniformBuffers;
  std::vector<VkDeviceMemory> mUniformBuffersMemory;
  VkDescriptorPool mDescriptorPool;
  std::vector<VkDescriptorSet> mDescriptorSets;
  VkImage mTextureImage;
  VkDeviceMemory mTextureImageMemory;
  VkImageView mTextureImageView;
  VkSampler mTextureSampler;

  const u32 WIDTH = 600;
  const u32 HEIGHT = 800;
  const s32 MAX_FRAMES_IN_FLIGHT = 2;

  const std::vector<const char *> mValidationLayers = {"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> mDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  const bool mEnableValidationLayers = true;

  const std::vector<Vertex> vertices = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
  };
  const std::vector<u16> indices = {0, 1, 2, 2, 3, 0};

  struct QueueFamilyIndices
  {
    std::optional<u32> mGraphicsFamily;
    std::optional<u32> mPresentFamily;

    bool IsComplete() const
    {
      return mGraphicsFamily.has_value() && mPresentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails
  {
    VkSurfaceCapabilitiesKHR mCapabilities;
    std::vector<VkSurfaceFormatKHR> mFormats;
    std::vector<VkPresentModeKHR> mPresentModes;
    bool IsAdequate() const
    {
      return !mFormats.empty() && !mPresentModes.empty();
    }
  };

public:
  bool mFramebufferResized = false;
  void Run();

private:
  void InitWindow();
  void InitVulkan();

  void CreateInstance();

  bool CheckValidationLayerSupport();

  std::vector<const char *> GetRequiredExtensions();

  void SetupDebugMessenger();

  void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo);

  void PickPhysicalDevice();

  bool IsDeviceSuitable(VkPhysicalDevice device);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

  bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

  QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
  void CreateLogicalDevice();

  void CreateSurface();

  VkSurfaceFormatKHR
  ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR
  ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

  void CreateSwapChain();

  void CreateRenderPass();
  void CreateGraphicsPipeline();
  void CreateImageViews();

  void CreateImage(
      u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
      VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory);
  void TransitionImageLayout(
      VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
  void CopyBufferToImage(VkBuffer buffer, VkImage image, u32 width, u32 height);
  VkShaderModule CreateShaderModule(const std::vector<char> &code);

  void CreateFrameBuffers();

  void CreateCommandBuffers();

  void CreateSyncObjects();

  void CreateCommandPool();

  void RecreateSwapChain();
  void MainLoop();

  void DrawFrame();

  void CleanupSwapChain();
  void CleanUp();
  void CreateVertexBuffer();
  u32 FindMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);
  void CreateBuffer(
      VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
      VkBuffer *buffer, VkDeviceMemory *bufferMemory);

  void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  VkCommandBuffer BeginSingleTimeCommands();
  void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
  void CreateIndexBuffer();
  void CreateDescriptorSetLayout();
  void CreateUniformBuffers();
  void UpdateUniformBuffer(u32 imageIndex);
  void CreateDescriptorPool();
  void CreateDescriptorSets();
  void CreateTextureImage();
  void CreateTextureImageView();
  VkImageView CreateImageView(VkImage image, VkFormat format);
  void CreateTextureSampler();
};
