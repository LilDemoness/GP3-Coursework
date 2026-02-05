#pragma once

#include <GL/glew.h>
#include <iostream>
#include "Vertex.h"

struct VertexAttribute
{
    enum VertexAttributeElement { kPosition = 0, kNormal = 1, kTextureCoordinate = 2 };
    static const VertexAttribute VertexAttributes[3];


	GLuint index;           // Shader layout location (0 = position, 1 = normal, etc.)
    GLint size;             // Number of components (e.g., vec3 = 3)
    GLenum type;            // Data type (GL_FLOAT, etc.)
    GLboolean is_normalized;// Normalize fixed-point data (usually GL_FALSE)
    GLsizei stride;         // Byte offset between consecutive attributes
    size_t offset;          // Byte offset of this attribute in the vertex struct

    VertexAttribute(GLuint index, GLint component_size, GLenum data_type, GLboolean is_normalized, GLsizei stride, size_t offset)
        : index(index),
          size(component_size),
          type(data_type),
          is_normalized(is_normalized),
          stride(stride),
          offset(offset)
    {}


    static VertexAttribute get_position_attribute() { return VertexAttributes[VertexAttributeElement::kPosition]; }
    static VertexAttribute get_normal_attribute() { return VertexAttributes[VertexAttributeElement::kNormal]; }
    static VertexAttribute get_texture_coordinate_attribute() { return VertexAttributes[VertexAttributeElement::kTextureCoordinate]; }

    static size_t get_total_stride()
    {
        size_t stride = 0;
        for each (VertexAttribute var in VertexAttributes)
        {
            stride += var.stride;
            std::cout << stride << std::endl;
        }
        return stride;
    }
};