#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct SDL_Window;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace focus
{

/// Implementation of type safe handles based on: https://www.ilikebigbits.com/2014_05_06_type_safe_handles.html
template<typename Tag>
class Handle
{
    uint64_t _value = 0;

  public:
    static Handle Invalid() { return Handle(); }

    Handle() = default; //: _value(default_value) {}
    explicit Handle(uint64_t value) : _value(value) {}

    explicit operator uint64_t() const { return _value; }

    friend bool operator==(Handle a, Handle b) { return a._value == b._value; }
    friend bool operator!=(Handle a, Handle b) { return a._value != b._value; }
    bool operator==(uint64_t b) { return _value == b; }
    bool operator!=(uint64_t b) { return _value != b; }
    uint64_t operator++(int) { return _value++; }
    size_t Hash() const { return std::hash<uint64_t>()(_value); }
};

#define MAKE_HANDLE(name)                                                                                              \
    struct name##Tag {                                                                                                 \
    };                                                                                                                 \
    using name = Handle<name##Tag>

MAKE_HANDLE(VertexBuffer);
MAKE_HANDLE(IndexBuffer);
MAKE_HANDLE(ConstantBuffer);
MAKE_HANDLE(ShaderBuffer);
MAKE_HANDLE(Shader);
MAKE_HANDLE(Pipeline);

#undef MAKE_HANDLE

enum class AccessMode {
    ReadOnly,
    WriteOnly,
    ReadWrite,
};

enum class VarType {
    Float,
    Int,
    UInt,
    Float2,
    Float3,
    Float4,
    Int2,
    Int3,
    Int4,
    UInt2,
    UInt3,
    UInt4,
    Float2x2,
    Float3x3,
    Float4x4,
    Sampler1D,
    Sampler2D,
    Sampler3D,

    Invalid
};

uint32_t VarTypeByteSize(VarType type);

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

// Lovingly borrowed from oryol-gfx
template<typename Tag>
struct BufferLayout {
    std::string debug_name;
    struct Attribute {
        std::string name;
        VarType type;
        uint32_t offset; // Calculated on a call to VertexBufferLayout::Add()
    };
    std::vector<Attribute> _attributes;
    BufferUsage _usage = BufferUsage::Default;
    uint32_t binding_point = 0;

    BufferLayout() = default;
    explicit BufferLayout(uint32_t bp, BufferUsage buffer_usage) : binding_point(bp), _usage(buffer_usage) {}
    BufferLayout &Add(const std::string &name, VarType type)
    {
        _attributes.push_back(BufferLayout::Attribute{
            .name = std::string(name),
            .type = type,
            .offset = _attributes.empty() ? 0 : VarTypeByteSize(type) + _attributes.back().offset,
        });
        return *this;
    }
    void SetDebugName(const std::string &name) { debug_name = name; }
};

#define MAKE_BUFFER_LAYOUT(name)                                                                                       \
    struct name##Tag {                                                                                                 \
    };                                                                                                                 \
    using name = BufferLayout<name##Tag>

MAKE_BUFFER_LAYOUT(VertexBufferLayout);
MAKE_BUFFER_LAYOUT(ConstantBufferLayout);
MAKE_BUFFER_LAYOUT(ShaderBufferLayout);

#undef MAKE_BUFFER_LAYOUT

struct IndexBufferLayout {
    std::string debug_name;
    IndexBufferType type;
    BufferUsage usage;

    IndexBufferLayout() = default;
    IndexBufferLayout(IndexBufferType buffer_type, BufferUsage buffer_usage = BufferUsage::Default) :
            type(buffer_type), usage(buffer_usage)
    {
    }
    void SetDebugName(const std::string &name) { debug_name = name; }
};

struct VertexBufferDescriptor {
    std::vector<std::string> input_descriptor_name;
    BufferType buffer_type;
    std::vector<VarType> types;
    uint32_t size_in_bytes;
    uint32_t element_size_in_bytes;
    BufferUsage usage;
};

struct IndexBufferDescriptor {
    IndexBufferType type;
    uint32_t size_in_bytes;
    BufferUsage usage;
};

struct ConstantBufferDescriptor {
    std::string name;
    std::vector<VarType> types; // TODO: consider if this is necessary
    uint32_t slot;
    BufferUsage usage;
    uint32_t size_in_bytes;
};

struct ShaderBufferDescriptor {
    std::string name;
    uint32_t slot;
    AccessMode accessMode;
    std::vector<VarType> types;
    BufferUsage usage;
    uint32_t size_in_bytes;
};

struct InputBufferDescriptor {
    std::string name; // name of the variable, not the semantic used.
    VarType type;
    uint32_t slot; // TODO: I guess for D3D this should be used for the intrinisc number? ie POSITION3, 3 is the slot
    uint32_t byteOffset;
};

struct ShaderInfo {
    // TODO: maybe use the position in the shader for the key?
    std::unordered_map<std::string, ConstantBufferDescriptor> mConstantBufferDescriptors;
    std::unordered_map<std::string, ShaderBufferDescriptor> mShaderBufferDescriptors;
    std::unordered_map<std::string, InputBufferDescriptor> mInputBufferDescriptors;
};

struct Color {
    float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 0.0f;
    bool operator!=(const Color &o) const
    {
        return red != o.red || green != o.green || blue != o.blue || alpha != o.alpha;
    }
};

struct DepthTest {
    ComparisonFunction function = ComparisonFunction::Less;
    bool enabled = true;
    bool write_to_depth_buffer = true;

    bool operator!=(const DepthTest &o) const
    {
        return function != o.function || enabled != o.enabled || write_to_depth_buffer != o.write_to_depth_buffer;
    }
};

struct CullingState {
    TriangleFace face = TriangleFace::Back;
    WindingOrder front_face = WindingOrder::CounterClockwise;
    bool enabled = true;

    bool operator!=(const CullingState &o) const
    {
        return face != o.face || front_face != o.front_face || enabled != o.enabled;
    }
};

enum class RasterizationMode {
    Point,
    Line,
    Fill,
};

struct StencilTest {
    struct StencilFace {
        ComparisonFunction func = ComparisonFunction::Always;
        TriangleFace face;
        uint32_t mask;
        StencilOp op;

        bool operator!=(const StencilFace &o) const
        {
            return func != o.func || face != o.face || mask != o.mask || op != o.op;
        }
    } front, back;
    // If true, mFront and mBack will be used for seperate stencil tests of front and back facing triangles.
    // Otherwise, the values set in mFront will be used for both.
    bool seperate = false;
    bool enabled = false;

    bool operator!=(const StencilTest &o) const
    {
        return front != o.front || back != o.back || seperate != o.seperate || enabled != o.enabled;
    }
};

struct DepthRange {
    double mNear = 0.0;
    double mFar = 1.0;

    bool operator!=(const DepthRange &o) const { return mNear != o.mNear || mFar != o.mFar; }
};

struct BlendState {
    struct BlendingFace {
        // TODO: not sure if the is the correct default behavior
        Color mBlendColor;
        BlendEquation mEquation = BlendEquation::Add;
        BlendFunction mFunction = BlendFunction::Zero;

        bool operator!=(const BlendingFace &o) const
        {
            return mBlendColor != o.mBlendColor || mEquation != o.mEquation || mFunction != o.mFunction;
        }
    } mColor, mAlpha;
    bool mSeperate = false;
    bool mEnabled = false;
    bool operator!=(const BlendState &o) const
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
    ClearBuffer to_clear = ClearBuffer::DepthStencilColor;
    Color clear_color;
};

struct PipelineState {
    Shader shader = Shader::Invalid();
    DepthTest depth_test;
    CullingState cull_state;
    RasterizationMode rasterization_mode = RasterizationMode::Fill;
    StencilTest stencil_test;
    DepthRange depth_range;
    BlendState blend_state;
};

struct SceneState {
    std::vector<VertexBuffer> vb_handles;
    std::vector<ConstantBuffer> cb_handles;
    IndexBuffer ib_handle = IndexBuffer::Invalid();
    bool indexed = false;
};

struct ComputeState {
    std::vector<ShaderBuffer> buffer_handles;
    std::vector<ConstantBuffer> cb_handles;
};

struct Window {
    int32_t width = 0;
    int32_t height = 0;

    SDL_Window *sdl_window = nullptr;
};

enum class RendererAPI {
    OpenGL,
    Vulkan,
    DX11,
    DX12,
    Invalid,
};

class Device
{
  public:
    static Device *Init(RendererAPI api);
    // Window creation
    virtual Window MakeWindow(int32_t width, int32_t height) = 0;

    // Shader creation
    virtual Shader CreateShaderFromBinary(const std::vector<uint8_t> &vBinary, const std::vector<uint8_t> &fBinary) = 0;
    virtual Shader CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource) = 0;
    virtual Shader CreateComputeShaderFromSource(const char *name, const std::string &source) = 0;

    // Buffer Creation
    virtual VertexBuffer CreateVertexBuffer(
        const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size) = 0;
    virtual IndexBuffer CreateIndexBuffer(
        const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size) = 0;
    virtual ConstantBuffer CreateConstantBuffer(
        const ConstantBufferLayout &constant_buffer_layout, void *data, uint32_t data_size) = 0;
    virtual ShaderBuffer CreateShaderBuffer(const ShaderBufferLayout &shader_buffer_layout, void *data, uint32_t data_size) = 0;

    virtual Pipeline CreatePipeline(PipelineState state) = 0;

//    virtual void UpdateVertexBuffer(VertexBuffer handle, void *data, uint32_t size) = 0;
//    virtual void UpdateIndexBuffer(IndexBuffer handle, void *data, uint32_t size) = 0;
//    virtual void UpdateConstantBuffer(ConstantBuffer handle, void *data, uint32_t size) = 0;
//    virtual void UpdateShaderBuffer(ShaderBuffer handle, void *data, uint32_t size) = 0;

    // Buffer Access
    // TODO: add partial buffer access too
    // TODO: consider adding a scoped pointer for mapped memory
    virtual void *MapBuffer(ShaderBuffer handle, AccessMode access_mode) = 0;
    virtual void UnmapBuffer(ShaderBuffer handle) = 0;

    // Buffer Destruction

    virtual void DestroyVertexBuffer(VertexBuffer handle) = 0;
    virtual void DestroyIndexBuffer(IndexBuffer handle) = 0;
    virtual void DestroyShaderBuffer(ShaderBuffer handle) = 0;
    virtual void DestroyConstantBuffer(ConstantBuffer handle) = 0;

    virtual void BeginPass(const std::string &name) = 0;

    virtual void BindSceneState(const SceneState &scene_state) = 0;
    virtual void BindPipeline(Pipeline pipeline) = 0;

    virtual void Draw(Primitive primitive) = 0;

    virtual void EndPass() = 0;

    // draw call submission

    // Compute shader dispatch
    virtual void DispatchCompute(
        uint32_t x_groups, uint32_t y_groups, uint32_t z_groups, Shader shader, const ComputeState &compute_state) = 0;

    // TODO: better naming
    virtual void WaitForMemory(uint64_t flags) = 0;

    // Screen clearing
    virtual void ClearBackBuffer(ClearState clear_state) = 0;

    virtual void SwapBuffers(const Window &window) = 0;
};

/*
// Window creation
Window MakeWindow(int32_t width, int32_t height);

// Shader creation
ShaderHandle CreateShaderFromBinary(const std::vector<uint8_t> &vBinary, const std::vector<uint8_t> &fBinary);
ShaderHandle CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource);
ShaderHandle CreateComputeShaderFromSource(const char *name, const std::string &source);

// Buffer Creation
VertexBufferHandle CreateVertexBuffer(void *data, VertexBufferDescriptor descriptor);
IndexBufferHandle CreateIndexBuffer(void *data, IndexBufferDescriptor descriptor);
ConstantBufferHandle CreateConstantBuffer(void *data, ConstantBufferDescriptor descriptor);
ShaderBufferHandle CreateShaderBuffer(void *data, ShaderBufferDescriptor descriptor);

void UpdateVertexBuffer(VertexBufferHandle handle, void *data, uint32_t size);
void UpdateIndexBuffer(IndexBufferHandle handle, void *data, uint32_t size);
void UpdateConstantBuffer(ConstantBufferHandle handle, void *data, uint32_t size);
void UpdateShaderBuffer(ShaderBufferHandle handle, void *data, uint32_t size);

// Buffer Access
// TODO: add partial buffer access too
// TODO: consider adding a scoped pointer for mapped memory
void *MapBuffer(ShaderBufferHandle handle, AccessMode access_mode);
void UnmapBuffer(ShaderBufferHandle handle);

// Buffer Destruction

void DestroyVertexBuffer(VertexBufferHandle handle);
void DestroyIndexBuffer(IndexBufferHandle handle);
void DestroyShaderBuffer(ShaderBufferHandle handle);
void DestroyConstantBuffer(ConstantBufferHandle handle);

// draw call submission
void Draw(Primitive primitive, RenderState render_state, ShaderHandle shader, const SceneState &scene_state);

// Compute shader dispatch
void DispatchCompute(uint32_t x_groups, uint32_t y_groups, uint32_t z_groups, ShaderHandle shader,
        const ComputeState &compute_state);

// TODO: better naming
void WaitForMemory(uint64_t flags);

// Screen clearing
void ClearBackBuffer(ClearState clear_state);

void swap_buffers(const Window &window);
*/

#ifdef _WIN32
/**
 * @brief Returns the Device and DeviceContext as a pair. Will fail if the api used isn't dx11.
 *
 * @return std::pair<ID3D11Device*, ID3D11DeviceContext*>
 */
std::pair<ID3D11Device *, ID3D11DeviceContext *> GetDeviceAndContext();
#endif

} // namespace focus
