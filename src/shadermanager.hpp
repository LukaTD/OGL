#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "types.hpp"

// Contains information for createion of shader modules
struct ShaderModuleInfo 
{
	std::string name; 			// name used to reference the created module
	std::string pathToShader; 	// path to file containing shader source code
	u32 		type; 			// shader stage (vertex, fragment, ...) 
};

// Contains information for createion of shader programs
struct ShaderProgramInfo
{
	std::string 			 name; 			// name used to reference the created program
	std::vector<std::string> moduleNames;	// names that specify the shader stages to be linked
	bool 					 deleteModules; // if set to true, used modules will be deleted after creation of shader program
};

// Used for setting variables(uniforms) in spectified shader program 
template<typename T>
struct ShaderVariable
{
	std::string programName;
	std::string variableName;
	T 			newValue;
};

// Manages shader programs and their creation 
struct ShaderManager
{
	std::unordered_map<std::string,u32> shaderModules;
	std::unordered_map<std::string,u32> shaderPrograms;

	~ShaderManager();
	
	void CreateShaderModule(const ShaderModuleInfo &moduleInfo);
	void CreateShaderProgram(const ShaderProgramInfo &programInfo);
    void UseShaderProgram(const std::string &programName);

	void DeleteSelectedShaderModules(const std::vector<std::string> &moduleNames);
	void DeleteSelectedShaderPrograms(const std::vector<std::string> &programNames);
	void DeleteAllShaderModules();
	void DeleteAllShaderPrograms();

	void SetVariable(const ShaderVariable<i32> &submission);
	void SetVariable(const ShaderVariable<f32> &submission);
	void SetVariable(const ShaderVariable<glm::vec2> &submission);
	void SetVariable(const ShaderVariable<glm::vec3> &submission);
	void SetVariable(const ShaderVariable<glm::vec4> &submission);
	void SetVariable(const ShaderVariable<glm::mat4> &submission);

	i32 GetUniformLocation(const std::string &programName,const std::string &variableName);
};