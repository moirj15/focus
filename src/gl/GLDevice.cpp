#include "GLDevice.hpp"

#include "ShaderManager.hpp"
#include "Utils.hpp"
#include "glad.h"

#include <SDL2/SDL.h>

namespace focus
{

static void DBCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message,
    void const *userParam)
{
    printf("DBG: %s\n", message);
}

GLDevice::GLDevice()
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        assert(0 && "Failed to Init video");
    }
    SDL_GL_LoadLibrary(nullptr);
}

GLDevice::~GLDevice()
{
    // TODO: proper shutdown
}

Window GLDevice::MakeWindow(int32_t width, int32_t height)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    SDL_Window *window =
        SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL);

    assert(window != nullptr);

    // Don't resize window so it doesn't mess with tiling window managers
    SDL_SetWindowResizable(window, SDL_FALSE);

    auto *context = SDL_GL_CreateContext(window);
    assert(context);
    assert(gladLoadGL());

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DBCallBack, nullptr);
#endif

    printf("%s", glGetString(GL_VERSION));

    glGenVertexArrays(1, &mVAO);
    return {
        .width = width,
        .height = height,
        .sdl_window = window,
    };
}

Shader GLDevice::CreateShaderFromBinary(const std::vector<uint8_t> &vBinary, const std::vector<uint8_t> &fBinary)
{
    assert(0);
    return {};
}

Shader GLDevice::CreateShaderFromSource(const char *name, const std::string &vSource, const std::string &fSource)
{
    return mShaderManager.AddShader(name, vSource, fSource);
}

Shader GLDevice::CreateComputeShaderFromSource(const char *name, const std::string &source)
{
    return mShaderManager.AddComputeShader(name, source);
}

VertexBuffer GLDevice::CreateVertexBuffer(
    const VertexBufferLayout &vertex_buffer_layout, void *data, uint32_t data_size)
{
    return mVBManager.Create(vertex_buffer_layout, data, data_size);
}

IndexBuffer GLDevice::CreateIndexBuffer(const IndexBufferLayout &index_buffer_layout, void *data, uint32_t data_size)
{
    return mIBManager.Create(index_buffer_layout, data, data_size);
}

ConstantBuffer GLDevice::CreateConstantBuffer(
    const ConstantBufferLayout &constant_buffer_layout, void *data, uint32_t data_size)
{
    return mCBManager.Create(constant_buffer_layout, data, data_size);
}

ShaderBuffer GLDevice::CreateShaderBuffer(
    const ShaderBufferLayout &shader_buffer_layout, void *data, uint32_t data_size)
{
    return mSBManager.Create(shader_buffer_layout, data, data_size);
}

Pipeline GLDevice::CreatePipeline(PipelineState state)
{
    return _pipeline_manager.InsertNew(state);
}

// void GLDevice::UpdateVertexBuffer(VertexBuffer handle, void *data, uint32_t size)
//{
//     mVBManager.WriteTo(data, size, handle);
// }
// void GLDevice::UpdateIndexBuffer(IndexBuffer handle, void *data, uint32_t size)
//{
//     mIBManager.WriteTo(data, size, handle);
// }
// void GLDevice::UpdateConstantBuffer(ConstantBuffer handle, void *data, uint32_t size)
//{
//     mCBManager.WriteTo(data, size, handle);
// }
// void GLDevice::UpdateShaderBuffer(ShaderBuffer handle, void *data, uint32_t size)
//{
//     assert(0);
// }

void *GLDevice::MapBuffer(ShaderBuffer handle, AccessMode accessMode)
{
    return glMapNamedBuffer(mSBManager.Get(handle), glUtils::AccessModeToGL(accessMode));
}
void GLDevice::UnmapBuffer(ShaderBuffer handle)
{
    glUnmapNamedBuffer(mSBManager.Get(handle));
}

void GLDevice::DestroyVertexBuffer(VertexBuffer handle)
{
    mVBManager.Destroy(handle);
}

void GLDevice::DestroyIndexBuffer(IndexBuffer handle)
{
    mIBManager.Destroy(handle);
}

void GLDevice::DestroyShaderBuffer(ShaderBuffer handle)
{
    mSBManager.Destroy(handle);
}
void GLDevice::DestroyConstantBuffer(ConstantBuffer handle)
{
    mCBManager.Destroy(handle);
}

void GLDevice::BeginPass(const std::string &name)
{
    _pass_name = name;
}

void GLDevice::BindSceneState(const SceneState &scene_state)
{
    _bound_scene_state = scene_state;
}

void GLDevice::BindPipeline(Pipeline pipeline)
{
    if (_bound_pipeline) {
        printf("Warning pipeline rebound during pass %s\n", _pass_name.c_str());
    }
    _bound_pipeline = pipeline;
    const auto &pipeline_state = _pipeline_manager.Get(pipeline).value();

    if (pipeline_state.depth_test != mCachedRenderState.depth_test) {
        if (pipeline_state.depth_test.enabled) {
            glEnable(GL_DEPTH_TEST);
            GLenum compFunc = glUtils::ComparisonFunctionToGL(pipeline_state.depth_test.function);
            glDepthFunc(compFunc);
            glDepthMask(pipeline_state.depth_test.write_to_depth_buffer);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }
    if (pipeline_state.cull_state != mCachedRenderState.cull_state) {
        if (pipeline_state.cull_state.enabled) {
            glEnable(GL_CULL_FACE);
            GLenum cullFace = glUtils::TriangleFaceToGL(pipeline_state.cull_state.face);
            glCullFace(cullFace);
            GLenum frontFace = glUtils::WindingOrderToGL(pipeline_state.cull_state.front_face);
            glFrontFace(frontFace);
        } else {
            glDisable(GL_CULL_FACE);
        }
    }
    if (pipeline_state.rasterization_mode != mCachedRenderState.rasterization_mode) {
        GLenum rasterizationMode = glUtils::RasterizationModeToGL(pipeline_state.rasterization_mode);
        // TODO: add the face and rasterization mode to a struct?
        glPolygonMode(GL_FRONT_AND_BACK, rasterizationMode);
    }
    if (pipeline_state.stencil_test != mCachedRenderState.stencil_test) {
        if (pipeline_state.stencil_test.enabled) {
            glEnable(GL_STENCIL_TEST);
        } else {
            glDisable(GL_STENCIL_TEST);
        }
    }
    if (pipeline_state.depth_range != mCachedRenderState.depth_range) {
        glDepthRange(pipeline_state.depth_range.near, pipeline_state.depth_range.near);
    }
    if (pipeline_state.blend_state != mCachedRenderState.blend_state) {
        if (pipeline_state.blend_state.enabled) {
            glEnable(GL_BLEND);
        } else {
            glDisable(GL_BLEND);
        }
    }
    glLineWidth(pipeline_state.line_width);
    //    auto shaderInfo = mShaderManager.GetInfo(pipeline_state.shader);
    GLuint program = mShaderManager.GetProgram(pipeline_state.shader);
    glUseProgram(program);
    glBindVertexArray(mVAO);
}

void GLDevice::BindAttributes()
{
    // TODO: sort based on bit flags?
    // Only doing interleved buffers for now
    // TODO: move a bunch of this stuff into some helpers
    for (const auto &vbHandle : _bound_scene_state.vb_handles) {
        const auto &vb_layout = mVBManager.mDescriptors[vbHandle];
        GLuint gl_vb_handle = mVBManager.Get(vbHandle);
        glBindBuffer(GL_ARRAY_BUFFER, gl_vb_handle);

        for (uint32_t attrib_index = 0; attrib_index < vb_layout.attributes.size(); attrib_index++) {
            const auto &attribute = vb_layout.attributes[attrib_index];
            glEnableVertexAttribArray(attrib_index + vb_layout.binding_point);
            GLint attribute_size_in_bytes = glUtils::VarTypeSizeInBytes(attribute.type);
            // TODO: figure out normalized inputs
            glVertexAttribPointer(attrib_index + vb_layout.binding_point, glUtils::VarTypeToSlotSizeGL(attribute.type),
                glUtils::VarTypeToIndividualTypeGL(attribute.type), false, attribute_size_in_bytes,
                (const void *)(uintptr_t)attribute.offset);
            glVertexAttribDivisor(attrib_index + vb_layout.binding_point, attribute.attrib_divisor);
        }
    }
    // TODO: some error management would be nice
    const PipelineState pipeline_state = _pipeline_manager.Get(_bound_pipeline.value()).value();
    const GLuint gl_shader_handle = mShaderManager.GetProgram(pipeline_state.shader);
    // setup all the uniform buffers
    for (const auto &cbHandle : _bound_scene_state.cb_handles) {
        const auto &cb_layout = mCBManager.mDescriptors[cbHandle];
        const GLuint gl_cb_handle = mCBManager.Get(cbHandle);
        glBindBuffer(GL_UNIFORM_BUFFER, gl_cb_handle);
        // TODO: figure out multiple binding points
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, gl_cb_handle);
        glUniformBlockBinding(gl_shader_handle, cb_layout.binding_point, 0);
    }
}

// draw call submission
void GLDevice::Draw(Primitive primitive, uint32_t starting_vertex, uint32_t point_count)
{
    GLenum glPrimitive = glUtils::PrimitiveToGL(primitive);
    BindAttributes();
    // TODO: pretty ugly, need to re-write. Maybe add an element count to the sceneState or some descriptor?
    glDrawArrays(glPrimitive, (GLint)starting_vertex, (GLint)point_count);
}

void GLDevice::DrawInstanced(
    Primitive primitive, uint32_t starting_vertex, uint32_t point_count, uint32_t instance_count)
{
    BindAttributes();
    GLenum gl_primitive = glUtils::PrimitiveToGL(primitive);
    glDrawArraysInstanced(gl_primitive, (GLint)starting_vertex, (GLint)point_count, (GLint)instance_count);
}

void GLDevice::EndPass()
{
    _bound_pipeline.reset();
}

void GLDevice::ClearBackBuffer(ClearState clearState)
{
    glClearColor(clearState.clear_color.red, clearState.clear_color.green, clearState.clear_color.blue,
        clearState.clear_color.alpha);
    glClear(glUtils::ClearBufferToGL(clearState.to_clear));
}

void GLDevice::DispatchCompute(
    uint32_t xGroups, uint32_t yGroups, uint32_t zGroups, Shader shader, const ComputeState &computeState)
{
    GLuint program = mShaderManager.GetProgram(shader);
    glUseProgram(program);
    for (auto bufferHandle : computeState.buffer_handles) {
        const auto &bufferDesc = mSBManager.mDescriptors[bufferHandle];
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSBManager.Get(bufferHandle));
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bufferDesc.binding_point, mSBManager.Get(bufferHandle));
    }
    for (const auto &cbHandle : computeState.cb_handles) {
        const auto &cbDesc = mCBManager.mDescriptors[cbHandle];
        glBindBuffer(GL_UNIFORM_BUFFER, mCBManager.Get(cbHandle));
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, mCBManager.Get(cbHandle));
    }
    glDispatchCompute(xGroups, yGroups, zGroups);
}

void GLDevice::WaitForMemory(uint64_t flags)
{
    // TODO: actually use input
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void GLDevice::SwapBuffers(const Window &window)
{
    SDL_GL_SwapWindow(window.sdl_window);
}

} // namespace focus
