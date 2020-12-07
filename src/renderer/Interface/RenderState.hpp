#pragma once

#include "Types.hpp"
#include "Handles.hpp"

#include <vector>

namespace renderer
{
struct Color {
  f32 red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
  bool operator!=(const Color &o) { return red != o.red || green != o.green || blue != o.blue || alpha != o.alpha; }
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

enum class BlendEquation { Add, Subtract, ReverseSubtract, Min, Max };

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

} // namespace renderer
