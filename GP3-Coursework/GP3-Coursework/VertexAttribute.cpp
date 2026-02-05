#pragma once
#include "VertexAttribute.h"

const VertexAttribute VertexAttribute::VertexAttributes[3] =
{
    {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position)},
    {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)},
    {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texture_coordinate)}
};