#include "Mesh.h"

Mesh::Mesh(const std::string& file_name) :
    file_path_(file_name),
    model_(OBJModel(file_name).ToIndexedModel())
{
    init_model();
}
Mesh* Mesh::create_mesh(const std::string& file_name, size_t max_count)
{
    if (file_to_mesh_map_.find(file_name) != file_to_mesh_map_.end())
    {
        Mesh* existing_mesh = file_to_mesh_map_[file_name];
        on_existing_mesh_retrieved(existing_mesh);
        return existing_mesh;
    }

#if CREATION_DEBUG_LOGS
    std::cout << "Creating New Mesh for File: " << file_name << std::endl;
#endif

    // Create, cache, and return a new mesh.
    Mesh* mesh = new Mesh(file_name);
    Mesh::initialise_mesh_instancing(mesh, max_count);
    file_to_mesh_map_.emplace(file_name, mesh);
    return mesh;
}


Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertex_buffer_object_);
    glDeleteBuffers(1, &element_buffer_object_);
    glDeleteVertexArrays(1, &vertex_array_object_);

    if (instance_buffers_.find(this) != instance_buffers_.end())
        --instance_buffers_[this]->existing_instance_count;
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


// ----- Rendering -----


void Mesh::draw()
{
    // Update bound array for this mesh (Note: Assumes this is mesh 0).
    glBindBuffer(GL_ARRAY_BUFFER, instance_buffers_[this]->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), instance_buffers_[this]->get_data(), GL_STATIC_DRAW);

    // Draw the mesh.
    glBindVertexArray(vertex_array_object_);
    glDrawElements(GL_TRIANGLES, draw_count_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Mesh::draw_instanced(const GLsizei instance_count)
{
    // Update bound array.
    glBindBuffer(GL_ARRAY_BUFFER, instance_buffers_[this]->buffer);
    glBufferData(GL_ARRAY_BUFFER, instance_count * sizeof(glm::mat4), instance_buffers_[this]->get_data(), GL_STATIC_DRAW);

    // Draw the instances.
    glBindVertexArray(vertex_array_object_);
    glDrawElementsInstanced(GL_TRIANGLES, draw_count_, GL_UNSIGNED_INT, 0, instance_count);
    glBindVertexArray(0);
}
const void Mesh::bind_vao() const
{
    glBindVertexArray(vertex_array_object_);
}


const std::vector<glm::vec3>& Mesh::get_vertex_positions() const { return model_->positions; }


// ----- Mesh Instancing -----


std::unordered_map<std::string, Mesh*> Mesh::file_to_mesh_map_;
std::unordered_map<Mesh*, Mesh::MeshInstanceData*> Mesh::instance_buffers_;

void Mesh::initialise_mesh_instancing(Mesh* mesh, size_t max_count)
{
    if (instance_buffers_.find(mesh) != instance_buffers_.end())
        return; // We've already initialised an instance buffer for this mesh type.

    //glm::mat4 model_matrices[MaxCount] = { glm::mat4(1.0f) };
    std::vector<glm::mat4> model_matrices = std::vector<glm::mat4>(max_count, glm::mat4(1.0f));


    // Configure Instanced Array.
    unsigned int buffer_;
    glGenBuffers(1, &buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_);
    glBufferData(GL_ARRAY_BUFFER, max_count * sizeof(glm::mat4), &(model_matrices.data())[0], GL_STATIC_DRAW);

    // Set our transformation matrices as an instance vertex attribute.
    mesh->bind_vao();
    // Set attribute pointers for the matrix (4 vec4s)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    // Unbind array.
    glBindVertexArray(0);

    // Cache instancing values.
    //instance_buffers_.emplace(mesh, new MeshInstanceData<MaxCount>(buffer_, model_matrices));
    instance_buffers_.emplace(mesh, new MeshInstanceData(buffer_, model_matrices));
}

void Mesh::on_existing_mesh_retrieved(Mesh* mesh)
{
    MeshInstanceData* instancing_data = instance_buffers_[mesh];
    ++instancing_data->existing_instance_count;
#if CREATION_DEBUG_LOGS
    std::cout << "Retrieving Existing Mesh for File: " << mesh->file_path_ << " | Instance Count: " << instancing_data->existing_instance_count << std::endl;
#endif

    if (instancing_data->existing_instance_count < instancing_data->instance_matrices.size())
        return; // We don't need to resize our instancing buffer.

    // We need to resize our instancing buffer.
    // Determine required values for our new instancing buffer.
    constexpr float kResizeValue = 1.5f;
    int new_max_size = (int)std::ceil(instancing_data->instance_matrices.size() * kResizeValue);
    //int existing_instance_count = instancing_data->existing_instance_count;

    // Resize the buffer.
    instancing_data->instance_matrices.resize(new_max_size, glm::mat4(1.0f));
    glBindBuffer(GL_ARRAY_BUFFER, instancing_data->buffer);
    glBufferData(GL_ARRAY_BUFFER, new_max_size * sizeof(glm::mat4), instancing_data->get_data(), GL_STATIC_DRAW);

#if CREATION_DEBUG_LOGS
    std::cout << "Resized Instance Buffer for Mesh: " << mesh->file_path_ << " | New Capacity: " << new_max_size << std::endl;
#endif
}


void Mesh::set_instance_matrix(const unsigned int index, const glm::mat4& value)
{
    assert(index < instance_buffers_[this]->instance_matrices.size());
    instance_buffers_[this]->instance_matrices[index] = value;
}