#pragma once

#include "../../common.h"
#include "../Interface/FocusBackend.hpp"
#include "BufferManager.hpp"
#include "ShaderManager.hpp"

struct GLFWwindow;

namespace focus
{

class GLDevice final : public Device
{
    BufferManager<VertexBuffer, VertexBufferLayout> mVBManager;
    BufferManager<IndexBuffer, IndexBufferLayout> mIBManager;
    BufferManager<ConstantBuffer, ConstantBufferLayout> mCBManager;
    BufferManager<ShaderBuffer, ShaderBufferLayout> mSBManager;
    ShaderManager mShaderManager;
    PipelineState mCachedRenderState;
    GLuint mVAO;
    SceneState _bound_scene_state;
    std::optional<Pipeline> _bound_pipeline;
    std::string _pass_name;

    // TODO: Extract and maybe use with other managers
    template <typename Handle, typename Data>
    class HandleManager
    {
        std::unordered_map<Handle, Data> _map;
        Handle _current_key;
      public:
        [[nodiscard]] Handle InsertNew(Data data) {
            _current_key++;
            _map.insert({_current_key, data});
            return _current_key;
        }
        [[nodiscard]] std::optional<Data> Get(Handle handle) {
            return _map.contains(handle) ? _map[handle] : std::optional<Data>{};
        }
    } ;
    HandleManager<Pipeline, PipelineState> _pipeline_manager;


public:
    GLDevice();
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
    IndexBuffer CreateIndexBuffer(
        const IndexBufferLayout &index_buffer_descriptor, void *data, uint32_t data_size) override;
    ConstantBuffer CreateConstantBuffer(
        const ConstantBufferLayout &constant_buffer_layout, void *data, uint32_t data_size) override;
    ShaderBuffer CreateShaderBuffer(const ShaderBufferLayout &shader_buffer_layout, void *data, uint32_t data_size) override;
    Pipeline CreatePipeline(PipelineState state) override;

    /*
    // Buffer Updates
    void UpdateVertexBuffer(VertexBuffer handle, void *data, uint32_t size) override;
    void UpdateIndexBuffer(IndexBuffer handle, void *data, uint32_t size) override;
    void UpdateConstantBuffer(ConstantBuffer handle, void *data, uint32_t size) override;
    void UpdateShaderBuffer(ShaderBuffer handle, void *data, uint32_t size) override;
     */

    void *MapBuffer(ShaderBuffer handle, AccessMode accessMode) override;
    void UnmapBuffer(ShaderBuffer handle) override;

    // Buffer Destruction
    void DestroyVertexBuffer(VertexBuffer handle) override;
    void DestroyIndexBuffer(IndexBuffer handle) override;
    void DestroyShaderBuffer(ShaderBuffer handle) override;
    void DestroyConstantBuffer(ConstantBuffer handle) override;

    void BeginPass(const std::string &name) override;

    void BindSceneState(const SceneState &scene_state) override;
    void BindPipeline(Pipeline pipeline) override;
    // draw call submission
    void Draw(Primitive primitive, uint32_t starting_vertex, uint32_t point_count) override;

    void EndPass() override;
    // Compute shader dispatch
    void DispatchCompute(
        uint32_t xGroups, uint32_t yGroups, uint32_t zGroups, Shader shader, const ComputeState &computeState) override;
    void WaitForMemory(uint64_t flags) override;

    // Screen clearing
    void ClearBackBuffer(ClearState clearState = {}) override;

    void SwapBuffers(const Window &window) override;
};

} // namespace focus
