#pragma once

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include "obj_loader.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Vertex.h"

#define DEFAULT_MESH_INSTANCE_COUNT 20

class Mesh {
public:
	~Mesh();
    static void clear();


    template <size_t MaxCount>
    static Mesh* create_mesh(const std::string& file_name)
    {
        if (file_to_mesh_map_.find(file_name) != file_to_mesh_map_.end())
            return file_to_mesh_map_[file_name];

        std::cout << "Creating New Mesh for File: " << file_name << std::endl;

        // Create, cache, and return a new mesh.
        Mesh* mesh = new Mesh(file_name);
        Mesh::initialise_mesh_instancing<MaxCount>(mesh);
        file_to_mesh_map_.emplace(file_name, mesh);
        return mesh;
    }
    static Mesh* create_mesh(const std::string& file_name) { return create_mesh<DEFAULT_MESH_INSTANCE_COUNT>(file_name); }


	void draw();
	void draw_instanced(GLsizei instance_count);
	void bind_vao();

	void set_instance_matrix(const unsigned int index, const glm::mat4& value);
	
	const std::vector<glm::vec3>& get_vertex_positions() const;

private:
	Mesh(Vertex* vertices, unsigned int num_vertices, unsigned int* indices, unsigned int num_indices);
	Mesh(const std::string& file_name);

    // Delete copy constructors.
	Mesh(Mesh& other) = delete;
	Mesh& operator= (const Mesh& other) = delete;


    std::string file_path_;


	void init_model();
	std::shared_ptr<IndexedModel> model_;

	GLuint vertex_array_object_ = 0;	// Vertex Array Object.
	GLuint vertex_buffer_object_ = 0;	// Interleaved Vertex Buffer Object.
	GLuint element_buffer_object_ = 0;  // Index Buffer Object.
	unsigned int draw_count_ = 0;		// Number of Indices.


	// Mesh Instancing.
    struct MeshInstanceData
    {
        unsigned int buffer;
        glm::mat4* instance_matrices;
        unsigned int count;

        /*MeshInstanceData() = default;
        template <size_t MaxCount>
        MeshInstanceData(unsigned int buffer, glm::mat4 instance_matrices[MaxCount]) :
            buffer(buffer),
            instance_matrices(&instance_matrices[0]),
            count(MaxCount)
        { }*/
        MeshInstanceData(unsigned int buffer, glm::mat4* instance_matrices, unsigned int count) :
            buffer(buffer),
            instance_matrices(instance_matrices),
            count(count)
        { }
    };

	static std::unordered_map<std::string, Mesh*> file_to_mesh_map_;
	static std::unordered_map<Mesh*, Mesh::MeshInstanceData*> instance_buffers_;
    template <size_t MaxCount>
    static void initialise_mesh_instancing(Mesh* mesh)
    {
        if (instance_buffers_.find(mesh) != instance_buffers_.end())
            return; // We've already initialised an instance buffer for this mesh type.

        glm::mat4 model_matrices[MaxCount] = { glm::mat4(1.0f) };


        // Configure Instanced Array.
        unsigned int buffer_;
        glGenBuffers(1, &buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_);
        glBufferData(GL_ARRAY_BUFFER, MaxCount * sizeof(glm::mat4), &model_matrices[0], GL_STATIC_DRAW);

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
        instance_buffers_.emplace(mesh, new MeshInstanceData(buffer_, &model_matrices[0], MaxCount));
    }
};
