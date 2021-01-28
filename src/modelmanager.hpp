#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <stdexcept>

#include <GLEW/glew.h>
#include <glm/glm.hpp>

#include "types.hpp"
#include "misc.hpp"

struct ModelInfo
{
	std::string modelName; 		// name used to reference the loaded model data
	std::string pathToModel;	// path to .obj file containing the model data
};

enum ModelStructure : u32
{
	VERTICES_ONLY,								// contains vetices
	VERTICES_AND_NORMALS,						// contains vetices and normals
	VERTICES_AND_TEXTURE_COORDINATES,			// contains vetices and texture coordinates
	VERTICES_TEXTURE_COORDINATES_AND_NORMALS	// contains vetices, texture coordinates and normals
};

struct Model
{
	ModelStructure structure;			// specifies the the structure of each vertex 
	u32 		   vertexArrayID;		// name of the OpenGL object required for submitting a draw call 
	u32 		   vertexBufferID;		// name of the OpenGL object specifying vertex structure
	u32			   numberOfVertices;	// specifies the number of vertices the model contains
};

// Manages the model (mesh) data loaded from .obj files
struct ModelManager
{
	std::unordered_map<std::string,Model> models;

	~ModelManager();
	
	void LoadModel(const ModelInfo &modelInfo);

	void DeleteModel(const std::string &modelName);
	void DeleteSelectedModels(const std::vector<std::string> &modelNames);
	void DeleteAllModels();
}; 

