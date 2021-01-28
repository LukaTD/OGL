#include "modelmanager.hpp"

ModelManager::~ModelManager()
{
	if(!this->models.empty())
	{
		std::cout << "Not all models have been manually deleted. " 
					 "Automatically deleting:" << std::endl;
		for(const auto &[modelName,model] : this->models)
			std::cout << '\t' << modelName << std::endl;
		
		DeleteAllModels();
	}
}

// Only handles triangulized meshes 
void ModelManager::LoadModel(const ModelInfo &modelInfo)
{
	std::ifstream modelFile(modelInfo.pathToModel,std::ios::in);
    
    if(!modelFile.is_open())
    {
        std::cout << "Model file at location: \"" << modelInfo.pathToModel
				  << "\" could not be opened!" << std::endl;
        exit(-1);
    }

	Model model;
	std::vector<glm::vec3> vertices; 			// describes only vertex coordinates
	std::vector<glm::vec2> textureCoords;		// describes only vertex texture coordinates
	std::vector<glm::vec3> normals; 			// describes only vertex normals
	std::vector<f32>       vertexComponent; 	// contains complete description of vertices
	u32 				   lineNumber = 1;  	// keeps track of the line number in the parsed file for error reporting
	bool				   firstTime = true;
	
	try
	{
		for(std::string line;std::getline(modelFile,line);lineNumber++)
		{
			std::string_view inputLine = line;

			if(inputLine.length() == 0 || line.starts_with('#')) // line is empty or is a comment
				continue;
			else if(inputLine.starts_with("vt")) 	// texture coordinate record found
			{
				inputLine.remove_prefix(3);

				const u32 space = inputLine.find(' ');
				
				glm::vec2 textureCoordinate;
				textureCoordinate.s = StrToNum<f32>(inputLine.substr(0,space));
				textureCoordinate.t = StrToNum<f32>(inputLine.substr(space + 1));
				
				textureCoords.push_back(textureCoordinate);
			}
			else if(inputLine.starts_with("vn")) 	// normal record found
			{
				inputLine.remove_prefix(3);

				const u32 spaceFirst = inputLine.find(' ');
				const u32 spaceLast  = inputLine.rfind(' ');
				
				glm::vec3 normal;
				normal.x = StrToNum<f32>(inputLine.substr(0,spaceFirst));
				normal.y = StrToNum<f32>(inputLine.substr(spaceFirst + 1,spaceLast - spaceFirst - 1));
				normal.z = StrToNum<f32>(inputLine.substr(spaceLast + 1));
				
				normals.push_back(normal);
			}
			else if(inputLine.starts_with('v'))		// vertex record found
			{
				inputLine.remove_prefix(2);

				const u32 spaceFirst = inputLine.find(' ');
				const u32 spaceLast  = inputLine.rfind(' ');
				
				glm::vec3 vertex;
				vertex.x = StrToNum<f32>(inputLine.substr(0,spaceFirst));
				vertex.y = StrToNum<f32>(inputLine.substr(spaceFirst + 1,spaceLast - spaceFirst - 1));
				vertex.z = StrToNum<f32>(inputLine.substr(spaceLast + 1));
				
				vertices.push_back(vertex);
			}
			else if(inputLine.starts_with('f'))		// face record found
			{
				if(firstTime)
				{
					if(textureCoords.empty() && normals.empty())
						model.structure = VERTICES_ONLY;
					else if(textureCoords.empty())
						model.structure = VERTICES_AND_NORMALS; 
					else if(normals.empty())
						model.structure = VERTICES_AND_TEXTURE_COORDINATES;
					else
						model.structure = VERTICES_TEXTURE_COORDINATES_AND_NORMALS;
					
					model.numberOfVertices = 0;

					firstTime = false;
				}
				
				inputLine.remove_prefix(2);

				const std::array<u64,4> delimiters = {
					0, 							// 1st character of first section
					inputLine.find(' ') + 1,    // 1st character of second section
					inputLine.rfind(' ') + 1,   // 1st character of third section
					inputLine.length() + 1		// 2 characters after the last character in the string
				};
				
				for(u32 i = 0;i < delimiters.size() - 1;i++)
				{
					auto faceVertex = inputLine.substr(delimiters[i],delimiters[i + 1] - delimiters[i] - 1);

					switch(model.structure)
					{
						case VERTICES_ONLY: 							// structure: v_x v_y v_z
						{		
							const glm::vec3 vertex = vertices[StrToNum<u32>(faceVertex) - 1];

							vertexComponent.insert(vertexComponent.end(),{
								vertex.x,vertex.y,vertex.z
							});
							break;
						}           
						case VERTICES_AND_NORMALS: 						// structure: v_x//n_x v_y//n_y v_z//n_z
						{
							const u32 slashFirst = faceVertex.find('/');
							const u32 slashLast  = faceVertex.rfind('/');

							const glm::vec3 vertex = vertices[StrToNum<u32>(faceVertex.substr(0,slashFirst)) - 1];
							const glm::vec3 normal = normals[StrToNum<u32>(faceVertex.substr(slashLast + 1)) - 1];

							vertexComponent.insert(vertexComponent.end(),{
								vertex.x,vertex.y,vertex.z,
								normal.x,normal.y,normal.z
							});
							break;
						}           
						case VERTICES_AND_TEXTURE_COORDINATES:			// structure: v_x/t_x v_y/t_y v_z/t_z
						{
							const u32 slash = faceVertex.find('/');

							const glm::vec3 vertex 		 = vertices[StrToNum<u32>(faceVertex.substr(0,slash)) - 1];
							const glm::vec2 textureCoord = textureCoords[StrToNum<u32>(faceVertex.substr(slash + 1)) - 1];
							
							vertexComponent.insert(vertexComponent.end(),{
								vertex.x,vertex.y,vertex.z,
								textureCoord.s,textureCoord.t
							});
							break;
						}
						case VERTICES_TEXTURE_COORDINATES_AND_NORMALS:	// structure: v_x/t_x/n_x v_y/t_y/n_y v_z/t_z/n_z
						{
							const u32 slashFirst = faceVertex.find('/');
							const u32 slashLast  = faceVertex.rfind('/');

							const glm::vec3 vertex 	     = vertices[StrToNum<u32>(faceVertex.substr(0,slashFirst)) - 1];
							const glm::vec2 textureCoord = textureCoords[StrToNum<u32>(faceVertex.substr(slashFirst + 1,slashLast - slashFirst - 1)) - 1];
							const glm::vec3 normal	     = normals[StrToNum<u32>(faceVertex.substr(slashLast + 1)) - 1];

							vertexComponent.insert(vertexComponent.end(),{
								vertex.x,vertex.y,vertex.z,
								textureCoord.s,textureCoord.t,
								normal.x,normal.y,normal.z
							});
						}
					}
					model.numberOfVertices++;
				}
			}
		}
	}
	catch(const std::exception &e) // catches the exceptions thrown by lexical_cast, when the cast isn't succsessful
	{
		std::cout << "Error occured while parsing the file \"" << modelInfo.pathToModel << "\" on line " << lineNumber << ":\n"
				  << e.what() << '\n';
		return;
	}

	model.numberOfVertices *= 3; // each face is composed of 3 vertices

	glGenVertexArrays(1,&model.vertexArrayID);
	glGenBuffers(1,&model.vertexBufferID);

	glBindVertexArray(model.vertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER,model.vertexBufferID);

	glBufferData(GL_ARRAY_BUFFER,vertexComponent.size()*sizeof(f32),vertexComponent.data(),GL_STATIC_DRAW);

	switch(model.structure)
	{
		case VERTICES_ONLY:
			// vertex position
			glVertexAttribPointer(0,3,GL_FLOAT,false,3*sizeof(f32),reinterpret_cast<void*>(0));
			glEnableVertexAttribArray(0);
			break;	

		case VERTICES_AND_NORMALS:
			// vertex position
			glVertexAttribPointer(0,3,GL_FLOAT,false,6*sizeof(f32),reinterpret_cast<void*>(0));
			glEnableVertexAttribArray(0);

			// vertex normal
			glVertexAttribPointer(1,3,GL_FLOAT,false,6*sizeof(f32),reinterpret_cast<void*>(3*sizeof(f32)));
			glEnableVertexAttribArray(1);
			break;

		case VERTICES_AND_TEXTURE_COORDINATES:
			// vertex position
			glVertexAttribPointer(0,3,GL_FLOAT,false,5*sizeof(f32),reinterpret_cast<void*>(0));
			glEnableVertexAttribArray(0);

			// texture coordinate
			glVertexAttribPointer(1,2,GL_FLOAT,false,5*sizeof(f32),reinterpret_cast<void*>(3*sizeof(f32)));
			glEnableVertexAttribArray(1);
			break;

		case VERTICES_TEXTURE_COORDINATES_AND_NORMALS:
			// vertex posiiotn
			glVertexAttribPointer(0,3,GL_FLOAT,false,8*sizeof(f32),reinterpret_cast<void*>(0));
			glEnableVertexAttribArray(0);

			// texture coordinate
			glVertexAttribPointer(1,2,GL_FLOAT,false,8*sizeof(f32),reinterpret_cast<void*>(3*sizeof(f32)));
			glEnableVertexAttribArray(1);

			// vertex normal
			glVertexAttribPointer(2,3,GL_FLOAT,false,8*sizeof(f32),reinterpret_cast<void*>(5*sizeof(f32)));
			glEnableVertexAttribArray(2);
	}

	this->models.insert(std::make_pair(modelInfo.modelName,model));
}

void ModelManager::DeleteModel(const std::string &modelName)
{
	const Model model = this->models[modelName];
	this->models.erase(modelName);

	glDeleteBuffers(1,&model.vertexBufferID);
	glDeleteVertexArrays(1,&model.vertexArrayID);
}

void ModelManager::DeleteSelectedModels(const std::vector<std::string> &modelNames)
{
	const u32 numberOfModels = modelNames.size();

	std::vector<u32> sequentialVertexBuffers;
	std::vector<u32> sequentialVertexArrays;
	sequentialVertexBuffers.resize(numberOfModels);
	sequentialVertexArrays.resize(numberOfModels);

	for(u32 i = 0;i < numberOfModels;i++)
	{
		const Model model = this->models[modelNames[i]];
		this->models.erase(modelNames[i]);

		sequentialVertexBuffers[i] = model.vertexBufferID;
		sequentialVertexArrays[i]  = model.vertexArrayID;
	}

	glDeleteBuffers(numberOfModels,sequentialVertexBuffers.data());
	glDeleteVertexArrays(numberOfModels,sequentialVertexArrays.data());
}

void ModelManager::DeleteAllModels()
{
	const u32 numberOfModels = this->models.size();

	std::vector<u32> sequentialVertexBuffers;
	std::vector<u32> sequentialVertexArrays;
	sequentialVertexBuffers.resize(numberOfModels);
	sequentialVertexArrays.resize(numberOfModels);

	u32 i = 0;
	for(const auto &[modelName,model] : this->models)
	{
		sequentialVertexBuffers[i] = model.vertexBufferID;
		sequentialVertexArrays[i]  = model.vertexArrayID;

		i++;
	}
	this->models.clear();

	glDeleteBuffers(numberOfModels,sequentialVertexBuffers.data());
	glDeleteVertexArrays(numberOfModels,sequentialVertexArrays.data());
}