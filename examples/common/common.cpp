#include "common.hpp"

#include "tiny_obj_loader.h"

namespace utils
{
Mesh ReadObjectFile(const std::string &file)
{
    auto obj_file = ReadEntireFileAsString(file.c_str());
    tinyobj::ObjReader obj_reader;
    obj_reader.ParseFromString(obj_file, "");
    auto attrib = obj_reader.GetAttrib();
    auto shapes = obj_reader.GetShapes();
    // TODO: try implementing vertex pulling using tinyobj::index_t to index into buffers for a fun example
    Mesh mesh;
    // TODO: optimize someday
    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            mesh.vertices.push_back(attrib.vertices[(index.vertex_index) * 3]);
            mesh.vertices.push_back(attrib.vertices[((index.vertex_index) * 3) + 1]);
            mesh.vertices.push_back(attrib.vertices[((index.vertex_index) * 3) + 2]);

            if (index.normal_index != -1) {
                mesh.normals.push_back(attrib.normals[(index.normal_index) * 3]);
                mesh.normals.push_back(attrib.normals[((index.normal_index) * 3) + 1]);
                mesh.normals.push_back(attrib.normals[((index.normal_index) * 3) + 2]);
            }

            if (index.texcoord_index != -1) {
                mesh.texture_coords.push_back(attrib.texcoords[(index.normal_index) * 2]);
                mesh.texture_coords.push_back(attrib.texcoords[((index.normal_index) * 2) + 1]);
            }
        }
    }

    return mesh;
}

} // namespace utils
