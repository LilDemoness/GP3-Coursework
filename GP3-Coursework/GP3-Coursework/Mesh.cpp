#include "Mesh.h"

Mesh::Mesh(const std::string& file_name) :
    file_path_(file_name),
    model_(OBJModel(file_name).ToIndexedModel())
{
    init_model();
}
Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertex_buffer_object_);
    glDeleteBuffers(1, &element_buffer_object_);
    glDeleteVertexArrays(1, &vertex_array_object_);
}
void Mesh::clear()
{
    // Cleanup our instancing buffers.
    for (auto it = instance_buffers_.begin(); it != instance_buffers_.end(); ++it)
    {
        MeshInstanceData* instance_data = (it++)->second;
        glDeleteBuffers(1, &instance_data->buffer);
    }
    // Cleanup our maps as their data is now outdated.
    instance_buffers_.clear();
    file_to_mesh_map_.clear();
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
    // Update bound array for this mesh (Note: Assumes this is mesh 0).
    glBindBuffer(GL_ARRAY_BUFFER, instance_buffers_[this]->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), instance_buffers_[this]->instance_matrices, GL_STATIC_DRAW);

    // Draw the mesh.
    glBindVertexArray(vertex_array_object_);
    glDrawElements(GL_TRIANGLES, draw_count_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Mesh::draw_instanced(const GLsizei instance_count)
{
    // Update bound array.
    glBindBuffer(GL_ARRAY_BUFFER, instance_buffers_[this]->buffer);
    glBufferData(GL_ARRAY_BUFFER, instance_count * sizeof(glm::mat4), instance_buffers_[this]->instance_matrices, GL_STATIC_DRAW);

    // Draw the instances.
    glBindVertexArray(vertex_array_object_);
    glDrawElementsInstanced(GL_TRIANGLES, draw_count_, GL_UNSIGNED_INT, 0, instance_count);
    glBindVertexArray(0);
}
void Mesh::bind_vao()
{
    glBindVertexArray(vertex_array_object_);
}


void Mesh::set_instance_matrix(const unsigned int index, const glm::mat4& value)
{
    assert(index < instance_buffers_[this]->count);
    instance_buffers_[this]->instance_matrices[index] = value;
}


const std::vector<glm::vec3>& Mesh::get_vertex_positions() const { return model_->positions; }


// ----- Mesh Instancing -----
std::unordered_map<std::string, Mesh*> Mesh::file_to_mesh_map_;
std::unordered_map<Mesh*, Mesh::MeshInstanceData*> Mesh::instance_buffers_;