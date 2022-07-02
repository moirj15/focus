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

    Handle() = default;
    explicit Handle(const uint64_t value) : _value(value) {}

    explicit operator uint64_t() const { return _value; }

    friend bool operator==(Handle a, Handle b) { return a._value == b._value; }
    friend bool operator!=(Handle a, Handle b) { return a._value != b._value; }
    bool operator==(const uint64_t b) const { return _value == b; }
    bool operator!=(const uint64_t b) const { return _value != b; }
    uint64_t operator++(int) { return _value++; }
    [[nodiscard]] size_t Hash() const { return std::hash<uint64_t>()(_value); }
};

} // namespace focus

namespace std
{

template<typename Tag>
struct hash<focus::Handle<Tag>> {
    // template <typename Tag, typename T, T default_value>
    size_t operator()(const focus::Handle<Tag> &k) const { return k.Hash(); }
};

} // namespace std
namespace focus
{

#define MAKE_HANDLE(name)                                                                                              \
    struct name##Tag {                                                                                                 \
    };                                                                                                                 \
    using name = Handle<name##Tag>

MAKE_HANDLE(VertexBuffer);
MAKE_HANDLE(DynamicVertexBuffer);
MAKE_HANDLE(IndexBuffer);
MAKE_HANDLE(DynamicIndexBuffer);
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

// TODO: rename?
enum class InputClassification {
    Normal,
    Instanced,
};

// Lovingly borrowed from oryol-gfx
template<typename Tag>
struct BufferLayout {
    struct Attribute {
        std::string name;
        VarType type;
        uint32_t offset; // Calculated on a call to VertexBufferLayout::Add()
        uint32_t attrib_divisor;
    };
    std::vector<Attribute> attributes;
    uint32_t binding_point = 0;
    BufferUsage usage = BufferUsage::Default;
    // InputClassification input_classification = InputClassification::Normal;
    std::string debug_name;
    uint32_t stride = 0;

    BufferLayout() = default;
    explicit BufferLayout(const std::string &db_name) : debug_name(db_name) {}
    explicit BufferLayout(const uint32_t bp, const BufferUsage buffer_usage,
        /*const InputClassification input_classification,*/ const std::string &db_name = "") :
            binding_point(bp),
            usage(buffer_usage), /*input_classification(input_classification),*/ debug_name(db_name)
    {
    }
    BufferLayout &Add(const std::string &name, VarType type, const uint32_t attrib_divisor = 0)
    {
        attributes.push_back(BufferLayout::Attribute{
            .name = std::string(name),
            .type = type,
            .offset = attributes.empty() ? 0 : VarTypeByteSize(type) + attributes.back().offset,
            .attrib_divisor = attrib_divisor,
        });
        stride += VarTypeByteSize(type);
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

    explicit IndexBufferLayout(
        const IndexBufferType buffer_type, BufferUsage const buffer_usage = BufferUsage::Default) :
            type(buffer_type),
            usage(buffer_usage)
    {
    }
    void SetDebugName(const std::string &name) { debug_name = name; }
};

struct InputBufferDescriptor {
    std::string name; // name of the variable, not the semantic used.
    VarType type;
    uint32_t slot; // TODO: I guess for D3D this should be used for the intrinisc number? ie POSITION3, 3 is the slot
    uint32_t byte_offset;
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
    // If true, mFront and mBack will be used for separate stencil tests of front and back facing triangles.
    // Otherwise, the values set in mFront will be used for both.
    bool separate = false;
    bool enabled = false;

    bool operator!=(const StencilTest &o) const
    {
        return front != o.front || back != o.back || separate != o.separate || enabled != o.enabled;
    }
};

// I guess some windows headers still include near and far pointer macros
#if defined near
#undef near
#endif
#if defined far
#undef far
#endif

struct DepthRange {
    double near = 0.0;
    double far = 1.0;

    bool operator!=(const DepthRange &o) const { return near != o.near || far != o.far; }
};

struct BlendState {
    struct BlendingFace {
        // TODO: not sure if the is the correct default behavior
        Color blend_color;
        BlendEquation equation = BlendEquation::Add;
        BlendFunction function = BlendFunction::Zero;

        bool operator!=(const BlendingFace &o) const
        {
            return blend_color != o.blend_color || equation != o.equation || function != o.function;
        }
    } color, alpha;
    bool seperate = false;
    bool enabled = false;
    bool operator!=(const BlendState &o) const
    {
        return color != o.color || alpha != o.alpha || seperate != o.seperate || enabled != o.enabled;
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
    float line_width = 1.0;
};

struct SceneState {
    std::vector<VertexBuffer> vb_handles;
    std::vector<DynamicVertexBuffer> dynamic_vb_handles;
    std::vector<ConstantBuffer> cb_handles;
    IndexBuffer ib_handle = IndexBuffer::Invalid();
    DynamicIndexBuffer dynamic_ib_handle = DynamicIndexBuffer::Invalid();
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

    virtual ~Device() {}
    // Window creation
    virtual Window MakeWindow(int32_t width, int32_t height) = 0;

    // Shader creation
    virtual Shader CreateShaderFromBinary(const std::vector<uint8_t> &vBinary, const std::vector<uint8_t> &fBinary) = 0;
    virtual Shader CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource) = 0;
    virtual Shader CreateComputeShaderFromSource(const char *name, const std::string &source) = 0;

    // Buffer Creation
    virtual VertexBuffer CreateVertexBuffer(
        const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size) = 0;

    virtual DynamicVertexBuffer CreateDynamicVertexBuffer(
        const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size) = 0;

    virtual IndexBuffer CreateIndexBuffer(
        const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size) = 0;

    virtual DynamicIndexBuffer CreateDynamicIndexBuffer(
        const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size) = 0;

    virtual ConstantBuffer CreateConstantBuffer(
        const ConstantBufferLayout &constant_buffer_layout, void *data, uint32_t data_size) = 0;

    virtual ShaderBuffer CreateShaderBuffer(
        const ShaderBufferLayout &shader_buffer_layout, void *data, uint32_t data_size) = 0;


    virtual Pipeline CreatePipeline(PipelineState state) = 0;


    virtual void UpdateDynamicVertexBuffer(DynamicVertexBuffer handle, void *data, uint32_t data_size, uint32_t offset) = 0;

    virtual void UpdateDynamicIndexBuffer(DynamicIndexBuffer handle, void *data, uint32_t data_size, uint32_t offset) = 0;

    virtual void UpdateConstantBuffer(ConstantBuffer handle, void *data, uint32_t data_size, uint32_t offset) = 0;

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
    virtual void DestroyDynamicVertexBuffer(DynamicVertexBuffer handle) = 0;
    virtual void DestroyIndexBuffer(IndexBuffer handle) = 0;
    virtual void DestroyDynamicIndexBuffer(DynamicIndexBuffer handle) = 0;
    virtual void DestroyShaderBuffer(ShaderBuffer handle) = 0;
    virtual void DestroyConstantBuffer(ConstantBuffer handle) = 0;

    virtual void BeginPass(const std::string &name) = 0;

    virtual void BindSceneState(const SceneState &scene_state) = 0;
    virtual void BindPipeline(Pipeline pipeline) = 0;

    virtual void Draw(Primitive primitive, uint32_t starting_vertex, uint32_t point_count) = 0;
    virtual void DrawInstanced(
        Primitive primitive, uint32_t starting_vertex, uint32_t point_count, uint32_t instance_count) = 0;

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

#ifdef _WIN32
/**
 * @brief Returns the Device and DeviceContext as a pair. Will fail if the api used isn't dx11.
 *
 * @return std::pair<ID3D11Device*, ID3D11DeviceContext*>
 */
std::pair<ID3D11Device *, ID3D11DeviceContext *> GetDeviceAndContext();
#endif

} // namespace focus
