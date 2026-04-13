#include "Mesh.h"

Mesh::Mesh(Vertex* vertices, unsigned int num_vertices, unsigned int* indices, unsigned int num_indices)
{
    std::shared_ptr<IndexedModel> model;

    for (unsigned int i = 0; i < num_vertices; i++)
    {
        model->positions.push_back(*vertices[i].get_pos());
        model->normals.push_back(*vertices[i].get_normal());
        model->texCoords.push_back(*vertices[i].get_texture_coordinate());
    }

    for (unsigned int i = 0; i < num_indices; i++)
    {
        model->indices.push_back(indices[i]);
    }

    init_model();
}
Mesh::Mesh(const std::string& file_name)
{
    model_ = OBJModel(file_name).ToIndexedModel();
    init_model();
}
Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertex_buffer_object_);
    glDeleteBuffers(1, &element_buffer_object_);
    glDeleteVertexArrays(1, &vertex_array_object_);
}


Mesh* Mesh::create_triangle_mesh()
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

    return new Mesh(vertices, numVertices, indices, numIndices);
}
Mesh* Mesh::create_quad_mesh()
{
    Vertex vertices[4] = {
        Vertex(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),    // Top Left
        Vertex(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),  // Bottom Left
        Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),   // Top Right
        Vertex(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),   // Bottom Right
    };
    unsigned int indices[4] = { 0, 1, 2, 3 };

    // Size calcuated by number of bytes of an array / no bytes of one element.
    unsigned int numVertices = sizeof(vertices) / sizeof(vertices[0]);
    unsigned int numIndices = sizeof(indices) / sizeof(indices[0]);

    return new Mesh(vertices, numVertices, indices, numIndices);
}


void Mesh::init_model()
{
    draw_count_ = model_->indices.size();

    // Check for empty model.
    if (model_->positions.empty() || model_->texCoords.empty() || model_->normals.empty())
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
    for (size_t i = 0; i < model_->positions.size(); i++)
    {
        interleavedData.push_back(model_->positions[i].x);
        interleavedData.push_back(model_->positions[i].y);
        interleavedData.push_back(model_->positions[i].z);

        interleavedData.push_back(model_->normals[i].x);
        interleavedData.push_back(model_->normals[i].y);
        interleavedData.push_back(model_->normals[i].z);

        interleavedData.push_back(model_->texCoords[i].x);
        interleavedData.push_back(model_->texCoords[i].y);
    }

    glBindVertexArray(vertex_array_object_);

    // Interleaved Data to Vertex Buffer Object.
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    // Index Data to Element Buffer Object.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model_->indices.size() * sizeof(unsigned int), model_->indices.data(), GL_STATIC_DRAW);


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

void Mesh::draw()
{
    glBindVertexArray(vertex_array_object_);
    glDrawElements(GL_TRIANGLES, draw_count_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


const std::vector<glm::vec3>& Mesh::get_vertex_positions() const { return model_->positions; }