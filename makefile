CPL = g++#clang++
WRN = -Wall -Wextra
LIB = -lGLEW -lglfw -lOpenGL
STD = -std=c++20
OPT = -O3#-O0
REQ = $(STD) $(WRN) $(OPT)

all: OpenGL

OpenGL: obj/OpenGl.o obj/ShaderManager.o obj/TextureManager.o obj/ModelManager.o obj/Camera.o obj/Mouse.o
	$(CPL) -o OpenGL obj/OpenGl.o obj/ShaderManager.o obj/ModelManager.o obj/TextureManager.o obj/Camera.o obj/Mouse.o $(LIB)

obj/Mouse.o: src/mouse.cpp
	$(CPL) -o obj/Mouse.o -c src/mouse.cpp $(REQ)

obj/Camera.o: src/camera.cpp
	$(CPL) -o obj/Camera.o -c src/camera.cpp $(REQ)

obj/TextureManager.o: src/texturemanager.cpp
	$(CPL) -o obj/TextureManager.o -c src/texturemanager.cpp $(REQ)

obj/ModelManager.o: src/modelmanager.cpp
	$(CPL) -o obj/ModelManager.o -c src/modelmanager.cpp $(REQ)

obj/ShaderManager.o: src/shadermanager.cpp
	$(CPL) -o obj/ShaderManager.o -c src/shadermanager.cpp $(REQ)

obj/OpenGl.o: src/opengl.cpp
	$(CPL) -o obj/OpenGl.o -c src/opengl.cpp $(REQ)