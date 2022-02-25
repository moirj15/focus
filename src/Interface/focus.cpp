#include "focus.hpp"

#include "../gl/GLDevice.hpp"

namespace focus
{

uint32_t VarTypeByteSize(const VarType type)
{
    switch (type) {
    case VarType::Float:
        return sizeof(float);
    case VarType::Int:
        return sizeof(int32_t);
    case VarType::UInt:
        return sizeof(uint32_t);
    case VarType::Float2:
        return sizeof(float) * 2;
    case VarType::Float3:
        return sizeof(float) * 3;
    case VarType::Float4:
        return sizeof(float) * 4;
    case VarType::Int2:
        return sizeof(int32_t) * 2;
    case VarType::Int3:
        return sizeof(int32_t) * 3;
    case VarType::Int4:
        return sizeof(int32_t) * 4;
    case VarType::UInt2:
        return sizeof(uint32_t) * 2;
    case VarType::UInt3:
        return sizeof(uint32_t) * 3;
    case VarType::UInt4:
        return sizeof(uint32_t) * 4;
    case VarType::Float2x2:
        return sizeof(float) * (2 * 2);
    case VarType::Float3x3:
        return sizeof(float) * (3 * 3);
    case VarType::Float4x4:
        return sizeof(float) * (4 * 4);
    default:
        assert(0);
        return -1;
    }
}

Device *Device::Init(RendererAPI api)
{
    if (api == RendererAPI::OpenGL) {
        return new GLDevice();
    } else if (api == RendererAPI::Vulkan) {

    } else if (api == RendererAPI::DX11) {

    } else if (api == RendererAPI::DX12) {

    } else if (api == RendererAPI::Invalid) {
    }
}

} // namespace focus
