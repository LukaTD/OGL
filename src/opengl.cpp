#include <iostream>
#include <string>
#include <array>
#include <cstdlib>
#include <fstream>
#include <chrono>

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/constants.hpp>

#include "types.hpp"
#include "timer.hpp"
#include "modelmanager.hpp"
#include "shadermanager.hpp"
#include "texturemanager.hpp"
#include "camera.hpp"
#include "mouse.hpp"
#include "misc.hpp"

namespace Modes
{
	u32 currentMouseMode = 0;
	const std::array mouseModes = {
		GLFW_CURSOR_NORMAL,
		GLFW_CURSOR_DISABLED
	};
	bool mouseModeChanged = false;

	u32 currentDrawMode = 0;
	const std::array drawModes = {
		GL_FILL,
		GL_LINE,
		GL_POINT
	};
}

u32 windowWidth = 1280,windowHeight = 720;

void FramebufferResizeCB(GLFWwindow *window, i32 width, i32 height);
void KeyPressedCB(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods);
void GetInput(GLFWwindow *window, Camera *camera, Mouse *mouse);

// Controls //
// W 	 - move forward
// A 	 - move left
// S 	 - move backward
// D 	 - move right
// SPACE - ascend
// LCRTL - descend
// R 	 - toggle camera look
// TAB 	 - change draw mode
// ESC	 - exit


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	//Toggle resizable window 
	//glfwWindowHint(GLFW_RESIZABLE,false);

	GLFWwindow *window = glfwCreateWindow(windowWidth,windowHeight,"OpenGLWin",0,0);
	
	if(!window)
	{
		std::cout << "Window couldn't be created!" << std::endl;
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window,FramebufferResizeCB);
	glfwSetKeyCallback(window,KeyPressedCB);

  	glewInit();

  	glViewport(0,0,1280,720);
  	glEnable(GL_DEPTH_TEST);  

	ModelManager modelManager;

	const std::array modelInfos = {
		ModelInfo{
			.modelName   = "house",
			.pathToModel = "assets/models/houseVT.obj"
		},
		ModelInfo{
			.modelName   = "skyboxCube",
			.pathToModel = "assets/models/cubeV.obj"
		},
		ModelInfo{
			.modelName   = "cube",
			.pathToModel = "assets/models/cubeVN.obj"
		}
	};
	
	for(const auto &modelInfo : modelInfos)
		modelManager.LoadModel(modelInfo);
	
  	TextureManager textureManager;

	const TextureInfo textureInfo = {
		.name 		 = "house",
		.pathToImage = "assets/textures/house.jpg"
	};
  	textureManager.CreateTextureFromImage(textureInfo);

	const CubemapTextureInfo cubemapInfo = {
		.name = "skybox",
		.pathsToImages = {
			"assets/textures/skybox/posx.jpg",
			"assets/textures/skybox/negx.jpg",
			"assets/textures/skybox/posy.jpg",
			"assets/textures/skybox/negy.jpg",
			"assets/textures/skybox/posz.jpg",
			"assets/textures/skybox/negz.jpg"
		}
	};
	textureManager.CreateCubemapFromImages(cubemapInfo);


	ShaderManager shaderManager;

	const std::array shaderModuleInfos = {
		ShaderModuleInfo{
			.name 		  = "texturedmodelVert",
			.pathToShader = "assets/shaders/texturedmodel.vert",				
			.type 		  = GL_VERTEX_SHADER
		},
		ShaderModuleInfo{
			.name 		  = "texturedmodelFrag",
			.pathToShader = "assets/shaders/texturedmodel.frag",
			.type 		  = GL_FRAGMENT_SHADER
		},
		ShaderModuleInfo{
			.name 		  = "coloredmodelVert",
			.pathToShader = "assets/shaders/coloredmodel.vert",				
			.type 		  = GL_VERTEX_SHADER
		},
		ShaderModuleInfo{
			.name 		  = "coloredmodelFrag",
			.pathToShader = "assets/shaders/coloredmodel.frag",
			.type 		  = GL_FRAGMENT_SHADER
		},
		ShaderModuleInfo{
			.name 		  = "skyboxVert",
			.pathToShader = "assets/shaders/skybox.vert",
			.type 		  = GL_VERTEX_SHADER
		},
		ShaderModuleInfo{
			.name 		  = "skyboxFrag",
			.pathToShader = "assets/shaders/skybox.frag",
			.type 		  = GL_FRAGMENT_SHADER
		}
	};
	for(const auto &shaderModuleInfo : shaderModuleInfos)
		shaderManager.CreateShaderModule(shaderModuleInfo);
	
	const std::array shaderProgramInfos = {
		ShaderProgramInfo{
			.name          = "texturedmodel",
			.moduleNames   = {
				"texturedmodelVert",
				"texturedmodelFrag"
			},
			.deleteModules = true
		},
		ShaderProgramInfo{
			.name          = "coloredmodel",
			.moduleNames   = {
				"coloredmodelVert",
				"coloredmodelFrag"
			},
			.deleteModules = true
		},
		ShaderProgramInfo{
			.name          = "skybox",
			.moduleNames   = {
				"skyboxVert",
				"skyboxFrag"
			},
			.deleteModules = true
		}
	};
	for(const auto &shaderProgramInfo : shaderProgramInfos)
		shaderManager.CreateShaderProgram(shaderProgramInfo);

	const std::array textureSamplerVariables = {
		ShaderVariable<i32>{
			.programName  = "texturedmodel",
			.variableName = "textureSampler",
			.newValue     = 0 // texture sampler is linked to GL_TEXTURE0 binding
		},
		ShaderVariable<i32>{
			.programName  = "skybox",
			.variableName = "cubemapSampler",
			.newValue     = 0
		}
	};
	shaderManager.UseShaderProgram("texturedmodel");
	shaderManager.SetVariable(textureSamplerVariables[0]);

	shaderManager.UseShaderProgram("skybox");
	shaderManager.SetVariable(textureSamplerVariables[1]);

  	Camera camera(glm::vec3(0.f,0.f,3.f),glm::vec3(0.f,0.f,0.f));

  	if(!glfwRawMouseMotionSupported())
  	{
    	std::cout << "Raw mouse input not supported!" << std::endl;
    	exit(-1);
  	}

	glfwSetInputMode(window,GLFW_RAW_MOUSE_MOTION,true);

	Mouse mouse(window);

	// Main loop
 	while(!glfwWindowShouldClose(window))
  	{
    	// Input
    	GetInput(window,&camera,&mouse);

    	// Render
    	glClearColor(.3f,.3f,.3f,2.f);
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//f32 time = static_cast<f32>(glfwGetTime());
    
    	//model = glm::translate(model,glm::vec3(glm::sin(time),0.0f,0.0f));
    	//model = glm::rotate(model,5*glm::sin(time),glm::vec3(0.5f,0.3f,0.0f));

		const std::array MVP = {
			ShaderVariable<glm::mat4>{ // model
				.programName  = "texturedmodel",
				.variableName = "model",
				.newValue 	  = glm::translate(glm::mat4(1.f),glm::vec3(.1f,0.f,0.f))
			},
			ShaderVariable<glm::mat4>{ // view
				.programName  = "texturedmodel",
				.variableName = "view",
				.newValue 	  = glm::lookAt(camera.position,camera.position+camera.direction,glm::vec3(0.f,1.f,0.f))
			},
			ShaderVariable<glm::mat4>{ // projection
				.programName  = "texturedmodel",
				.variableName = "projection",
				.newValue 	  = glm::perspective(glm::radians(45.0f),windowWidth/static_cast<f32>(windowHeight),.1f,100.f)
			}
		};

		const std::array MVPcube = {
			ShaderVariable<glm::mat4>{ // model
				.programName  = "coloredmodel",
				.variableName = "model",
				.newValue 	  = glm::translate(glm::mat4(1.f),glm::vec3(-2.f,0.f,3.f))
			},
			ShaderVariable<glm::mat4>{ // view
				.programName  = "coloredmodel",
				.variableName = "view",
				.newValue 	  = glm::lookAt(camera.position,camera.position+camera.direction,glm::vec3(0.f,1.f,0.f))
			},
			ShaderVariable<glm::mat4>{ // projection
				.programName  = "coloredmodel",
				.variableName = "projection",
				.newValue 	  = glm::perspective(glm::radians(45.0f),windowWidth/static_cast<f32>(windowHeight),.1f,100.f)
			}
		};

		const std::array VPskybox = {
			ShaderVariable<glm::mat4>{ // view
				.programName  = "skybox",
				.variableName = "view",
				.newValue 	  = glm::mat4(glm::mat3(glm::lookAt(camera.position,camera.position+camera.direction,glm::vec3(0.0f,1.0f,0.0f))))
			},
			ShaderVariable<glm::mat4>{ // projection
				.programName  = "skybox",
				.variableName = "projection",
				.newValue 	  = glm::perspective(glm::radians(45.0f),windowWidth/static_cast<f32>(windowHeight),0.1f,100.0f)
			}
		};

		//house
		const Model &house = modelManager.models["house"];

    	glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D,textureManager.textures["house"]);
    
		shaderManager.UseShaderProgram("texturedmodel");
		for(const auto &variable : MVP)
			shaderManager.SetVariable(variable);

		glBindVertexArray(house.vertexArrayID);
		glDrawArrays(GL_TRIANGLES,0,house.numberOfVertices);
		glBindVertexArray(0);
		
		//cube
		const Model &cube = modelManager.models["cube"];

		shaderManager.UseShaderProgram("coloredmodel");
		for(const auto &variable : MVPcube)
			shaderManager.SetVariable(variable);

		glBindVertexArray(cube.vertexArrayID);
		glDrawArrays(GL_TRIANGLES,0,cube.numberOfVertices);
		glBindVertexArray(0);

		// skybox
		glDepthFunc(GL_LEQUAL); 

		const Model &skyboxCube = modelManager.models["skyboxCube"];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP,textureManager.textures["skybox"]);

		shaderManager.UseShaderProgram("skybox");
		for(const auto &variable : VPskybox)
			shaderManager.SetVariable(variable);

		glBindVertexArray(skyboxCube.vertexArrayID);
		glDrawArrays(GL_TRIANGLES,0,skyboxCube.numberOfVertices);
		glDepthFunc(GL_LESS); 
		
    	// Reset for next frame
    	glfwSwapBuffers(window);
    	glfwPollEvents();
  	}

	shaderManager.DeleteAllShaderPrograms();
	textureManager.DeleteAllTextures();
	modelManager.DeleteAllModels();

  	glfwTerminate();
  	return 0;
}

void FramebufferResizeCB([[maybe_unused]] GLFWwindow *window, i32 width, i32 height)
{
	glViewport(0,0,width,height);
	windowWidth = static_cast<u32>(width);
	windowHeight = static_cast<u32>(height);
	std::cout << "Window resized to: " << windowWidth << " x " << windowHeight << std::endl;
}

void GetInput(GLFWwindow *window, Camera *camera, Mouse *mouse)
{
  	const f32 speed = 0.05f;

	if(Modes::mouseModeChanged)
	{
		if(Modes::mouseModes[Modes::currentMouseMode] == GLFW_CURSOR_DISABLED)
		{
			mouse->InitialCursorPosition();
			mouse->mouseOn = true;
		}
		else
			mouse->mouseOn = false;

		Modes::mouseModeChanged = false;
	}
	
	if(mouse->mouseOn)
	{
		mouse->CalculateDelta();

		const f32 degPerPixel = 0.2f; // amount of rotation applied per pixel of mouse movement
		if(mouse->deltaX != 0)
		{
			const f32 deltaDeg = -mouse->deltaX*degPerPixel;

			camera->direction = glm::rotate(camera->direction,glm::radians(deltaDeg),camera->up);
			camera->left 	  = glm::rotate(camera->left,glm::radians(deltaDeg),camera->up);
		}
		if(mouse->deltaY != 0)
		{
			const f32 deltaDeg = -mouse->deltaY*degPerPixel;
			const f32 degUpAndDirection = glm::degrees(glm::acos(glm::dot(camera->up,
																		  camera->direction)));

			if(deltaDeg > 0)
			{
				if(degUpAndDirection - deltaDeg > 15.f)
					camera->direction = glm::rotate(camera->direction,glm::radians(deltaDeg),
													camera->left);
				else
					camera->direction = glm::rotate(camera->direction,
													glm::radians(degUpAndDirection - 15.f),
													camera->left);
			}
			else
			{
				if(degUpAndDirection - deltaDeg < 165.f)
					camera->direction = glm::rotate(camera->direction,glm::radians(deltaDeg),
													camera->left);
				else
					camera->direction = glm::rotate(camera->direction,
													glm::radians(degUpAndDirection - 165.f),
													camera->left);
			}	
		}
	}

  	if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
    	camera->position += camera->direction*speed; 
	if(glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
    	camera->position -= camera->direction*speed;

  	if(glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
    	camera->position += camera->left*speed;
  	if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
    	camera->position -= camera->left*speed;

	if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS)
		camera->position += camera->up*speed;
	if(glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera->position -= camera->up*speed;
}

void KeyPressedCB(GLFWwindow *window, i32 key, [[maybe_unused]] i32 scancode, i32 action, [[maybe_unused]] i32 mods)
{
	switch(key)
	{
		//exit 
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window,true);
			break;

		//window mouse caputre mode and raw mouse 
		case GLFW_KEY_R:
			if(action == GLFW_PRESS)
			{
				Modes::mouseModeChanged = true;
				if(Modes::currentMouseMode == Modes::mouseModes.size()-1)
					Modes::currentMouseMode = 0;		
				else
					Modes::currentMouseMode++;
			}

			glfwSetInputMode(window,GLFW_CURSOR,Modes::mouseModes[Modes::currentMouseMode]);
			break;

		//render mode switch 
		case GLFW_KEY_TAB:
			if(action == GLFW_PRESS)
			{
				if(Modes::currentDrawMode == Modes::drawModes.size()-1)
					Modes::currentDrawMode = 0;
				else
					Modes::currentDrawMode++;
			}
			
			glPolygonMode(GL_FRONT_AND_BACK,Modes::drawModes[Modes::currentDrawMode]);
			break;
	}
}
