#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

struct SDL_Window;
struct ID3D11Device;
struct ID3D11DeviceContext;
namespace focus
{

using VertexBufferHandle = u32;
using IndexBufferHandle = u32;
using ShaderHandle = u32;
using ConstantBufferHandle = u32;
using BufferHandle = u32;

constexpr u32 INVALID_HANDLE = 0xFFFFFFFF;

enum class AccessMode {
  ReadOnly,
  WriteOnly,
  ReadWrite,
};

enum class VarType {
  Float,
  Int,
  UInt,
  Vec2,
  Vec3,
  Vec4,
  Int2,
  Int3,
  Int4,
  UInt2,
  UInt3,
  UInt4,
  Mat2,
  Mat3,
  Mat4,
  Sampler1D,
  Sampler2D,
  Sampler3D,

  Invalid
};

enum class PrecisionType {
  Half,
  Single,
  Double,
};

enum class IndexType {
  U8,
  U16,
  U32,
};

enum class Primitive {
  Points,
  LineStrip,
  LineLoop,
  Lines,
  LineStripAdjacency,
  LinesAdjacency,
  TriangleStrip,
  TriangleFan,
  Triangles,
  TriangleStripAdjacency,
};

enum class WindingOrder {
  Clockwise,
  CounterClockwise,
};

enum class ComparisonFunction {
  Never,
  Less,
  Equal,
  LessOrEqual,
  Greater,
  GreaterOrEqual,
  NotEqual,
  Always,
};

enum class TriangleFace {
  Front,
  Back,
};

enum class StencilOp {
  Keep,
  Zero,
  Replace,
  Increment,
  IncrementAndWrap,
  Decrement,
  DecrementAndWrap,
  Invert,
};

enum class BlendEquation {
  Add,
  Subtract,
  ReverseSubtract,
  Min,
  Max,
};

enum class BlendFunction {
  Zero,
  One,
  SourceColor,
  OneMinusSourceColor,
  OneMinusDestinationColor,
  SourceAlpha,
  OneMinusSourceAlpha,
  DestinationAlpha,
  OneMinusDestinationAlpha,
  ConstantColor,
  OneMinusConstantColor,
  ConstantAlpha,
  OneMinusConstantAlpha,
  SourceAlphaSaturate,
  SourceOneColor,
  OneMinusSourceOneColor,
  SourceOneAlpha,
  OneMinusSourceOneAlpha,
};

enum class BufferType {
  SingleType,  // just one type i.e (vvvvvvv)
  InterLeaved, // (vntvntvnt)
  Combined,    // (vvvnnnttt)
};

enum class IndexBufferType {
  U8,
  U16,
  U32,
};

enum class BufferUsage {
  Default,
  Dynamic,
  Static,
  Staging,
};

// TODO: consider different ways of defining these types since they are fairly similar
// TODO: handle various buffer types (offsets, types, etc)
struct VertexBufferDescriptor {
  std::string inputDescriptorName;
  BufferType bufferType;
  VarType type;
  u32 sizeInBytes;
  BufferUsage usage;
};

struct IndexBufferDescriptor {
  IndexBufferType type;
  u32 sizeInBytes;
  BufferUsage usage;
};

struct ConstantBufferDescriptor {
  std::string name;
  std::vector<VarType> types; // TODO: consider if this is necessary
  u32 slot;
  BufferUsage usage;
  u32 sizeInBytes;
};

struct ShaderBufferDescriptor {
  std::string name;
  u32 slot;
  AccessMode accessMode;
  std::vector<VarType> types;
  BufferUsage usage;
  u32 sizeInBytes;
};

struct InputBufferDescriptor {
  std::string name; // name of the variable, not the semantic used.
  VarType type;
  u32 slot; // TODO: I guess for D3D this should be used for the intrinisc number? ie POSITION3, 3 is the slot
  u32 byteOffset;
};

struct ShaderInfo {
  // TODO: maybe use the position in the shader for the key?
  std::unordered_map<std::string, ConstantBufferDescriptor> mConstantBufferDescriptors;
  std::unordered_map<std::string, ShaderBufferDescriptor> mShaderBufferDescriptors;
  std::unordered_map<std::string, InputBufferDescriptor> mInputBufferDescriptors;
};
struct Color {
  f32 red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
  bool operator!=(const Color &o) { return red != o.red || green != o.green || blue != o.blue || alpha != o.alpha; }
};

struct DepthTest {
  ComparisonFunction mFunction = ComparisonFunction::Less;
  bool mEnabled = true;
  bool mWriteToDepthBuffer = true;

  bool operator!=(const DepthTest &o)
  {
    return mFunction != o.mFunction || mEnabled != o.mEnabled || mWriteToDepthBuffer != o.mWriteToDepthBuffer;
  }
};

struct CullingState {
  TriangleFace mFace = TriangleFace::Back;
  WindingOrder mFrontFace = WindingOrder::CounterClockwise;
  bool mEnabled = true;

  bool operator!=(const CullingState &o)
  {
    return mFace != o.mFace || mFrontFace != o.mFrontFace || mEnabled != o.mEnabled;
  }
};

enum class RasterizationMode {
  Point,
  Line,
  Fill,
};

struct StencilTest {
  struct StencilFace {
    ComparisonFunction mFunc = ComparisonFunction::Always;
    TriangleFace mFace;
    u32 mMask;
    StencilOp mOp;

    bool operator!=(const StencilFace &o)
    {
      return mFunc != o.mFunc || mFace != o.mFace || mMask != o.mMask || mOp != o.mOp;
    }
  } mFront, mBack;
  // If true, mFront and mBack will be used for seperate stencil tests of front and back facing triangles.
  // Otherwise, the values set in mFront will be used for both.
  bool mSeperate = false;
  bool mEnabled = false;

  bool operator!=(const StencilTest &o)
  {
    return mFront != o.mFront || mBack != o.mBack || mSeperate != o.mSeperate || mEnabled != o.mEnabled;
  }
};

struct DepthRange {
  f64 mNear = 0.0;
  f64 mFar = 1.0;

  bool operator!=(const DepthRange &o) { return mNear != o.mNear || mFar != o.mFar; }
};

struct BlendState {
  struct BlendingFace {
    // TODO: not sure if the is the correct default behavior
    Color mBlendColor;
    BlendEquation mEquation = BlendEquation::Add;
    BlendFunction mFunction = BlendFunction::Zero;

    bool operator!=(const BlendingFace &o)
    {
      return mBlendColor != o.mBlendColor || mEquation != o.mEquation || mFunction != o.mFunction;
    }
  } mColor, mAlpha;
  bool mSeperate = false;
  bool mEnabled = false;
  bool operator!=(const BlendState &o)
  {
    return mColor != o.mColor || mAlpha != o.mAlpha || mSeperate != o.mSeperate || mEnabled != o.mEnabled;
  }
};

enum class ClearBuffer {
  Depth = 1,
  Stencil = 1 << 1,
  Color = 1 << 2,
  DepthStencil = Depth | Stencil,
  DepthColor = Depth | Color,
  StencilColor = Stencil | Color,
  DepthStencilColor = Depth | Stencil | Color,
};

struct ClearState {
  ClearBuffer toClear = ClearBuffer::DepthStencilColor;
  Color clearColor;
};

struct RenderState {
  DepthTest depthTest;
  CullingState cullState;
  RasterizationMode rasterizationMode = RasterizationMode::Fill;
  StencilTest stencilTest;
  DepthRange depthRange;
  BlendState blendState;
};

struct SceneState {
  std::vector<VertexBufferHandle> vbHandles;
  std::vector<ConstantBufferHandle> cbHandles;
  IndexBufferHandle ibHandle;
  bool indexed = false;
};

struct ComputeState {
  std::vector<BufferHandle> bufferHandles;
  std::vector<ConstantBufferHandle> cbHandles;
};

struct Window {
  s32 mWidth;
  s32 mHeight;

  SDL_Window *mSDLWindow;
};

enum class RendererAPI {
  OpenGL,
  Vulkan,
  DX11,
  DX12,
  Invalid,
};

class Context
{
public:
  // TODO: consider switching to sdl to handle the message loop so this #ifdef isn't necessary
  /**
   * @brief Initialize the global render context (gContext) with the specified Render API.
   *        Should only be called once, calling multiple times will result in an assertion in debug builds.
   * @param api The render API that will the global render context will use.
   * @param messageHandler The windows message handler.
   * @param instanceHandle The windows application's HINSTANCE value that is passed to WinMain.
   */
  static void Init(RendererAPI api);

  // TODO: think of a better way to do this, maybe have creating the window be agnostic but not init?
  virtual void Init(){};

  // Window creation
  virtual Window MakeWindow(s32 width, s32 height) = 0;

  // Shader creation
  virtual ShaderHandle CreateShaderFromBinary(const char *vBinary, const char *fBinary) = 0;
  virtual ShaderHandle CreateShaderFromSource(
      const char *name, const std::string &vSource, const std::string &fSource) = 0;
  virtual ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source) = 0;

  // Buffer Creation
  virtual VertexBufferHandle CreateVertexBuffer(void *data, u32 sizeInBytes, VertexBufferDescriptor descriptor) = 0;
  virtual IndexBufferHandle CreateIndexBuffer(void *data, u32 sizeInBytes, IndexBufferDescriptor descriptor) = 0;
  virtual ConstantBufferHandle CreateConstantBuffer(
      void *data, u32 sizeInBytes, ConstantBufferDescriptor descriptor) = 0;
  virtual BufferHandle CreateShaderBuffer(void *data, u32 sizeInBytes, ShaderBufferDescriptor descriptor) = 0;

  virtual void UpdateVertexBuffer(VertexBufferHandle handle, void *data, u32 size) = 0;
  virtual void UpdateIndexBuffer(IndexBufferHandle handle, void *data, u32 size) = 0;
  virtual void UpdateConstantBuffer(ConstantBufferHandle handle, void *data, u32 size) = 0;
  virtual void UpdateShaderBuffer(BufferHandle handle, void *data, u32 size) = 0;

  virtual std::vector<u8> ReadShaderBuffer(BufferHandle handle) = 0;

  // Buffer Access
  // TODO: add partial buffer access too
  // TODO: consider adding a scoped pointer for mapped memory
  virtual void *MapBufferPtr(BufferHandle handle, AccessMode accessMode) = 0;
  virtual void UnmapBufferPtr(BufferHandle handle) = 0;

  // Buffer Destruction

  virtual void DestroyVertexBuffer(VertexBufferHandle handle) = 0;
  virtual void DestroyIndexBuffer(IndexBufferHandle handle) = 0;
  virtual void DestroyShaderBuffer(BufferHandle handle) = 0;
  virtual void DestroyConstantBuffer(ConstantBufferHandle handle) = 0;

  // Draw call submission
  virtual void Draw(
      Primitive primitive, RenderState renderState, ShaderHandle shader, const SceneState &sceneState) = 0;

  // Compute shader dispatch
  virtual void DispatchCompute(
      u32 xGroups, u32 yGroups, u32 zGroups, ShaderHandle shader, const ComputeState &computeState) = 0;

  // TODO: better naming
  virtual void WaitForMemory(u64 flags) = 0;

  // Screen clearing
  virtual void Clear(ClearState clearState = {}) = 0;

  virtual void SwapBuffers(const Window &window) = 0;
};
extern Context *gContext;

/**
 * @brief Returns the Device and DeviceContext as a pair. Will fail if the api used isn't dx11.
 *
 * @return std::pair<ID3D11Device*, ID3D11DeviceContext*>
 */
std::pair<ID3D11Device *, ID3D11DeviceContext *> GetDeviceAndContext();

} // namespace focus
