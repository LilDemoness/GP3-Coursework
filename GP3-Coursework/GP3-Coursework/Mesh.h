#pragma once

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <string>
#include "obj_loader.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Vertex.h"

constexpr int kDefaultMeshInstanceCount = 20;
#define CREATION_DEBUG_LOGS true

class Mesh {
public:
    static std::shared_ptr<Mesh> create_mesh(const std::string& file_name, size_t max_count = kDefaultMeshInstanceCount);

	Mesh(const std::string& file_name);
	~Mesh();
    static void clear();


    // ----- Rendering -----

	void draw();
	void draw_instanced(GLsizei instance_count);
	const void bind_vao() const;

    // ----- Mesh Instancing -----

	void set_instance_matrix(const unsigned int index, const glm::mat4& value);
    void return_instance();
	
    // ----- Other -----
    
    const std::vector<glm::vec3>& get_vertex_positions() const;

private:
    // Delete copy constructors.
	Mesh(Mesh& other) = delete;
	Mesh& operator= (const Mesh& other) = delete;


	void init_model();
	std::shared_ptr<IndexedModel> model_;
    std::string file_path_;

    // ----- Rendering -----

	GLuint vertex_array_object_ = 0;	// Vertex Array Object.
	GLuint vertex_buffer_object_ = 0;	// Interleaved Vertex Buffer Object.
	GLuint element_buffer_object_ = 0;  // Index Buffer Object.
	unsigned int draw_count_ = 0;		// Number of Indices.

	// ----- Mesh Instancing -----

    struct MeshInstanceData
    {
        const unsigned int buffer;
        std::vector<glm::mat4> instance_matrices;   // DO NOT RESIZE.
        unsigned int existing_instance_count;

        /*MeshInstanceData() = default;
        template <size_t MaxCount>
        MeshInstanceData(unsigned int buffer, glm::mat4 instance_matrices[MaxCount]) :
            buffer(buffer),
            instance_matrices(&instance_matrices[0]),
            count(MaxCount)
        { }*/
        MeshInstanceData(unsigned int buffer, std::vector<glm::mat4> instance_matrices) :
            buffer(buffer),
            instance_matrices(instance_matrices),
            existing_instance_count(1)
        { }


        const void* get_data() { return &(instance_matrices.data())[0]; }
    };

	static std::unordered_map<std::string, std::shared_ptr<Mesh>> file_to_mesh_map_;
	static std::unordered_map<Mesh*, Mesh::MeshInstanceData*> instance_buffers_;
    static void initialise_mesh_instancing(std::shared_ptr<Mesh> mesh, size_t max_count);
    static void on_existing_mesh_retrieved(std::shared_ptr<Mesh> mesh);
};