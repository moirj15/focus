#pragma once

#include <focus.hpp>

#include <optional>
#include <stdio.h>
#include <string>

class Example
{
  protected:
    std::string _name = "";
    focus::Device *_device;
    focus::Window _window;

  public:
    explicit Example(focus::Device *device, focus::Window window, const std::string name) :
            _device(device), _window(window), _name(name)
    {
    }
    virtual ~Example() {}
    virtual void DoFrame() = 0;
};

namespace utils
{
enum class FilePermissions {
    Read,
    Write,
    ReadWrite,
    BinaryRead,
    BinaryWrite,
    BinaryReadWrite,
};

inline FILE *OpenFile(const char *file, FilePermissions permissions)
{
    const char *cPermissions[] = {"r", "w", "w+", "rb", "wb", "wb+"};
    FILE *ret = nullptr;
    ret = fopen(file, cPermissions[(uint32_t)permissions]);
    if (!ret) {
        // TODO: better error handling
        printf("FAILED TO OPEN FILE: %s\n", file);
        exit(EXIT_FAILURE);
    }
    return ret;
}

inline std::string ReadEntireFileAsString(const char *file)
{
    auto *fp = OpenFile(file, FilePermissions::Read);
    fseek(fp, 0, SEEK_END);
    uint64_t length = ftell(fp);
    rewind(fp);
    if (length == 0) {
        printf("Failed to read file size\n");
    }
    std::string data(length, 0);
    fread(data.data(), sizeof(uint8_t), length, fp);
    fclose(fp);
    return data;
}

inline std::vector<uint8_t> ReadEntireFileAsVector(const char *file)
{
    auto *fp = OpenFile(file, FilePermissions::BinaryRead);
    fseek(fp, 0, SEEK_END);
    uint64_t length = ftell(fp);
    rewind(fp);
    if (length == 0) {
        printf("Failed to read file size\n");
    }
    std::vector<uint8_t> data(length, 0);
    fread(data.data(), sizeof(uint8_t), data.size(), fp);
    fclose(fp);
    return data;
}

struct Mesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texture_coords;
    std::vector<uint32_t> indices;
};

Mesh ReadObjectFile(const std::string &file);

} // namespace utils

template<typename ErrorType, typename ReturnType>
class MayFail
{
    std::optional<ErrorType> _error_code;
    std::optional<ReturnType> _return_value;

  public:
};
