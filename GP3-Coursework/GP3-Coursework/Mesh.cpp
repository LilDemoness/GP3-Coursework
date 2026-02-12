#include "Mesh.h"

Mesh::Mesh(Vertex* vertices, unsigned int num_vertices, unsigned int* indices, unsigned int num_indices)
{
    IndexedModel model;

    for (unsigned int i = 0; i < num_vertices; i++)
    {
        model.positions.push_back(*vertices[i].get_pos());
        model.normals.push_back(*vertices[i].get_normal());
        model.texCoords.push_back(*vertices[i].get_texture_coordinate());
    }

    for (unsigned int i = 0; i < num_indices; i++)
    {
        model.indices.push_back(indices[i]);
    }

    InitModel(model);
}
Mesh::Mesh(const std::string& file_name)
{
    IndexedModel model = OBJModel(file_name).ToIndexedModel();
    InitModel(model);
}
Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertex_buffer_object_);
    glDeleteBuffers(1, &element_buffer_object_);
    glDeleteVertexArrays(1, &vertex_array_object_);
}


Mesh Mesh::CreateTriangleMesh()
{
    Vertex vertices[3] = {
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),  // Bottom Left
        Vertex(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 1.0f)),  // Top Middle
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),  // Bottom Right
    };
    unsigned int indices[3] = { 0, 1, 2 };

    // Size calcuated by number of bytes of an array / no bytes of one element.
    unsigned int numVertices = sizeof(vertices) / sizeof(vertices[0]);
    unsigned int numIndices = sizeof(indices) / sizeof(indices[0]);

    return Mesh(vertices, numVertices, indices, numIndices);
}


void Mesh::InitModel(const IndexedModel& model)
{
    draw_count_ = model.indices.size();

    // Check for empty model.
    if (model.positions.empty() || model.texCoords.empty() || model.normals.empty())
    {
        std::cerr << "ERROR: Model data is missing!" << std::endl;
        return;
    }

    // Create Buffers.
    glGenVertexArrays(1, &vertex_array_object_);
    glGenBuffers(1, &vertex_buffer_object_);
    glGenBuffers(1, &element_buffer_object_);

    // Create Interleaved Data.
    std::vector<float> interleavedData;
    for (size_t i = 0; i < model.positions.size(); i++)
    {
        interleavedData.push_back(model.positions[i].x);
        interleavedData.push_back(model.positions[i].y);
        interleavedData.push_back(model.positions[i].z);

        interleavedData.push_back(model.normals[i].x);
        interleavedData.push_back(model.normals[i].y);
        interleavedData.push_back(model.normals[i].z);

        interleavedData.push_back(model.texCoords[i].x);
        interleavedData.push_back(model.texCoords[i].y);
    }

    glBindVertexArray(vertex_array_object_);

    // Interleaved Data to Vertex Buffer Object.
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    // Index Data to Element Buffer Object.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.indices.size() * sizeof(unsigned int), model.indices.data(), GL_STATIC_DRAW);


    // Dynamic Vertex Attributes.
    size_t stride = sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);

    // Position.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)));

    // Texture Coordinates.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec3)));

    glBindVertexArray(0);
}

void Mesh::Draw()
{
    glBindVertexArray(vertex_array_object_);
    glDrawElements(GL_TRIANGLES, draw_count_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
