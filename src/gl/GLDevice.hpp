#pragma once

//#include "../../common.h"
#include "../Interface/focus.hpp"
#include "ShaderManager.hpp"

#include <cassert>
#include <optional>

struct GLFWwindow;

namespace focus
{

template<typename Handle, typename Data>
class HandleManager
{
    std::unordered_map<Handle, Data> _map;
    Handle _current_key;

  public:
    [[nodiscard]] Handle InsertNew(Data data)
    {
        _current_key++;
        _map.insert({_current_key, data});
        return _current_key;
    }
    [[nodiscard]] std::optional<Data> Get(Handle handle)
    {
        return _map.contains(handle) ? _map[handle] : std::optional<Data>{};
    }
};

template<typename Handle, typename BufferLayout>
struct BufferManager {
    Handle mCurrHandle{0};
    std::unordered_map<Handle, GLuint> mHandles;
    std::unordered_map<Handle, BufferLayout> mDescriptors;

    Handle Create(BufferLayout buffer_layout, void *data, uint32_t size_in_bytes, GLenum usage)
    {
        // Create the buffer for OpenGL
        GLuint handle;
        glGenBuffers(1, &handle);
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        glBufferData(GL_ARRAY_BUFFER, size_in_bytes, data, usage);
        // Do the actual management of the buffer handle
        mCurrHandle++;
        mDescriptors[mCurrHandle] = buffer_layout;
        mHandles[mCurrHandle] = handle;
        return mCurrHandle;
    }

    GLuint Get(Handle handle) { return mHandles[handle]; }

    void Destroy(Handle handle)
    {
        auto bufferHandle = mHandles[handle];
        mHandles.erase(handle);
        mDescriptors.erase(handle);
        glDeleteBuffers(1, &bufferHandle);
    }
};

class GLDevice final : public Device
{
    BufferManager<VertexBuffer, VertexBufferLayout> mVBManager;
    BufferManager<DynamicVertexBuffer, VertexBufferLayout> _dynamic_vb_manager;
    BufferManager<IndexBuffer, IndexBufferLayout> mIBManager;
    BufferManager<DynamicIndexBuffer, IndexBufferLayout> m_dynamic_ib_manager;
    BufferManager<ConstantBuffer, ConstantBufferLayout> mCBManager;
    BufferManager<ShaderBuffer, ShaderBufferLayout> mSBManager;
    ShaderManager mShaderManager;
    PipelineState mCachedRenderState;
    GLuint mVAO;
    SceneState _bound_scene_state;
    std::optional<Pipeline> _bound_pipeline;
    std::string _pass_name;

    // TODO: Extract and maybe use with other managers
    HandleManager<Pipeline, PipelineState> _pipeline_manager;

  public:
    GLDevice();
    ~GLDevice() override;
    // Window creation
    Window MakeWindow(int32_t width, int32_t height) override;

    // Shader creation

    // TODO: need to create a shader compile tool
    Shader CreateShaderFromBinary(const std::vector<uint8_t> &vBinary, const std::vector<uint8_t> &fBinary) override;
    Shader CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource) override;
    Shader CreateComputeShaderFromSource(const char *name, const std::string &source) override;

    // Buffer Creation
    VertexBuffer CreateVertexBuffer(
        const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size) override;

    DynamicVertexBuffer CreateDynamicVertexBuffer(
        const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size) override;

    IndexBuffer CreateIndexBuffer(
        const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size) override;

    DynamicIndexBuffer CreateDynamicIndexBuffer(
        const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size) override;

    ConstantBuffer CreateConstantBuffer(
        const ConstantBufferLayout &constant_buffer_layout, void *data, uint32_t data_size) override;

    ShaderBuffer CreateShaderBuffer(
        const ShaderBufferLayout &shader_buffer_layout, void *data, uint32_t data_size) override;

    Pipeline CreatePipeline(PipelineState state) override;

    void UpdateDynamicVertexBuffer(
        DynamicVertexBuffer handle, void *data, uint32_t data_size, uint32_t offset) override;

    void UpdateDynamicIndexBuffer(
        DynamicIndexBuffer handle, void *data, uint32_t data_size, uint32_t offset = 0) override;
    void UpdateConstantBuffer(ConstantBuffer handle, void *data, uint32_t data_size, uint32_t offset) override;

    void *MapBuffer(ShaderBuffer handle, AccessMode accessMode) override;
    void UnmapBuffer(ShaderBuffer handle) override;

    // Buffer Destruction
    void DestroyVertexBuffer(VertexBuffer handle) override;
    void DestroyDynamicVertexBuffer(DynamicVertexBuffer handle) override;
    void DestroyDynamicIndexBuffer(DynamicIndexBuffer handle) override;
    void DestroyIndexBuffer(IndexBuffer handle) override;
    void DestroyShaderBuffer(ShaderBuffer handle) override;
    void DestroyConstantBuffer(ConstantBuffer handle) override;

    void BeginPass(const std::string &name) override;

    void BindSceneState(const SceneState &scene_state) override;
    void BindPipeline(Pipeline pipeline) override;
    // draw call submission
    void Draw(Primitive primitive, uint32_t starting_vertex, uint32_t point_count) override;
    void DrawInstanced(
        Primitive primitive, uint32_t starting_vertex, uint32_t point_count, uint32_t instance_count) override;

    void EndPass() override;
    // Compute shader dispatch
    void DispatchCompute(
        uint32_t xGroups, uint32_t yGroups, uint32_t zGroups, Shader shader, const ComputeState &computeState) override;
    void WaitForMemory(uint64_t flags) override;

    // Screen clearing
    void ClearBackBuffer(ClearState clearState = {}) override;

    void SwapBuffers(const Window &window) override;

  private:
    void BindAttributes();
};

} // namespace focus
