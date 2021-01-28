#include "shadermanager.hpp"

ShaderManager::~ShaderManager()
{
	if(!this->shaderModules.empty())
	{
		std::cout << "Not all shader mouldes have been manually deleted. "
					 "Automatically deleting:" << std::endl;
		for(const auto &[shaderName,shaderID] : this->shaderModules)
		{
			std::cout << '\t' << shaderName << std::endl;
			glDeleteShader(shaderID);
		}
	}
	if(!this->shaderPrograms.empty())
	{
		std::cout << "Not all shader programs have been manually deleted. " 
					 "Automatically deleting:" << std::endl;
		for(const auto &[programName,programID] : this->shaderPrograms)
		{
			std::cout << '\t' << programName << std::endl;
			glDeleteShader(programID);
		}
	}
}

void ShaderManager::CreateShaderModule(const ShaderModuleInfo &moduleInfo)
{
	std::ifstream shaderFile(moduleInfo.pathToShader,std::ios::in | std::ios::binary);
    
    if(!shaderFile.is_open())
    {
        std::cout << "Shader file at location: \"" << moduleInfo.pathToShader 
				  << "\" could not be opened!" << std::endl;
        exit(-1);
    }

    shaderFile.seekg(0,std::ios::end);
    u32 fileLength = shaderFile.tellg();
    shaderFile.seekg(0,std::ios::beg);
    
    std::string fileContent;
	fileContent.reserve(fileLength + 1); // for '\0' character 
    shaderFile.read(const_cast<char*>(fileContent.data()),fileLength);
	fileContent[fileLength] = '\0';

    shaderFile.close();
  
    const char* contentLocation = fileContent.c_str();

    u32 shaderID = glCreateShader(moduleInfo.type);
    glShaderSource(shaderID,1,&contentLocation,NULL);
    glCompileShader(shaderID);
    
    i32 valid;
    glGetShaderiv(shaderID,GL_COMPILE_STATUS,&valid);

    if(!valid)
    {
        i32 logLength;
        glGetShaderiv(shaderID,GL_INFO_LOG_LENGTH,&logLength);

        std::string log;
		log.resize(logLength);
        glGetShaderInfoLog(shaderID,logLength,nullptr,const_cast<char*>(log.data()));
        
        std::cout << "Invalid \"" << moduleInfo.pathToShader 
				  << "\" contents:\n" << log << std::endl;
        exit(-1);
	}

	this->shaderModules.insert(std::make_pair(moduleInfo.name,shaderID));
}

void ShaderManager::CreateShaderProgram(const ShaderProgramInfo &programInfo)
{
	u32 programID = glCreateProgram();

	for(const std::string &moduleName : programInfo.moduleNames)
		glAttachShader(programID,shaderModules[moduleName]);

	glLinkProgram(programID);

	i32 valid;
    glGetProgramiv(programID,GL_LINK_STATUS,&valid);

    if(!valid)
    {
        i32 logLength;
        glGetProgramiv(programID,GL_INFO_LOG_LENGTH,&logLength);
        
        std::string log;
		log.resize(logLength);
        glGetProgramInfoLog(programID,logLength,nullptr,const_cast<char*>(log.data()));
        
        std::cout << "Unsucsessful shader program linking:\n" << log << std::endl;
        exit(-1);
    }

	if(programInfo.deleteModules)
		for(const std::string &moduleName : programInfo.moduleNames)
		{
			u32 shaderID = this->shaderModules[moduleName];
			glDetachShader(programID,shaderID);
			glDeleteShader(shaderID);
			this->shaderModules.erase(moduleName);
		}
	else
		for(const std::string &moduleName : programInfo.moduleNames)
			glDetachShader(programID,this->shaderModules[moduleName]);
	
	this->shaderPrograms.insert(std::make_pair(programInfo.name,programID));
}

void ShaderManager::UseShaderProgram(const std::string &programName)
{
    glUseProgram(this->shaderPrograms[programName]);
}

void ShaderManager::DeleteSelectedShaderModules(const std::vector<std::string> &moduleNames)
{
	for(const std::string &moduleName : moduleNames)
	{
		glDeleteShader(this->shaderModules[moduleName]);
		this->shaderModules.erase(moduleName);
	}
}
void ShaderManager::DeleteSelectedShaderPrograms(const std::vector<std::string> &programNames)
{
	for(const std::string &programName : programNames)
	{
		glDeleteShader(this->shaderPrograms[programName]);
		this->shaderPrograms.erase(programName);
	}
}
void ShaderManager::DeleteAllShaderModules()
{
	for(const auto &[shaderName,shaderID] : this->shaderModules)
		glDeleteShader(shaderID);

	this->shaderModules.clear();
}
void ShaderManager::DeleteAllShaderPrograms()
{
	for(const auto &[programName,programID] : this->shaderPrograms)
		glDeleteProgram(programID);

	this->shaderPrograms.clear();
}

i32 ShaderManager::GetUniformLocation(const std::string &programName,const std::string &variableName)
{
    i32 location = glGetUniformLocation(this->shaderPrograms[programName],variableName.c_str());

    if(location == -1)
    {
        std::cout << "Variable \"" << variableName << "\" can't be found in the shader program \"" 
				  << programName << "\"!" << std::endl;
        exit(-1);
    }

    return location;
}

void ShaderManager::SetVariable(const ShaderVariable<i32> &submission)
{
	glUniform1i(GetUniformLocation(submission.programName,submission.variableName),submission.newValue);
}

void ShaderManager::SetVariable(const ShaderVariable<f32> &submission)
{
	glUniform1f(GetUniformLocation(submission.programName,submission.variableName),submission.newValue);
}

void ShaderManager::SetVariable(const ShaderVariable<glm::vec2> &submission)
{
	glUniform2fv(GetUniformLocation(submission.programName,submission.variableName),1,glm::value_ptr(submission.newValue));
}

void ShaderManager::SetVariable(const ShaderVariable<glm::vec3> &submission)
{
	glUniform3fv(GetUniformLocation(submission.programName,submission.variableName),1,glm::value_ptr(submission.newValue));
}

void ShaderManager::SetVariable(const ShaderVariable<glm::vec4> &submission)
{
	glUniform4fv(GetUniformLocation(submission.programName,submission.variableName),1,glm::value_ptr(submission.newValue));
}

void ShaderManager::SetVariable(const ShaderVariable<glm::mat4> &submission)
{
	glUniformMatrix4fv(GetUniformLocation(submission.programName,submission.variableName),1,false,glm::value_ptr(submission.newValue));
}