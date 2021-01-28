$CPL = "clang++";
$WRN = "-Wall","-Wextra","-Wno-deprecated-volatile","-Wno-deprecated-declarations";
$INC = "-I.";
$LIBINC = "-LGLFW/lib","-LGLEW/lib";
$LIB = "-lglew32","-lglfw3dll","-lopengl32";
$STD = "-std=c++20";
$OPT = "-O3"; #"-O0"
$REQ = @($STD) + @($WRN) + @($OPT) + @($INC);

$CMD = "-o","obj/Mouse.o","-c","src/mouse.cpp";
& $CPL $CMD $REQ;

$CMD = "-o","obj/Camera.o","-c","src/camera.cpp";
& $CPL $CMD $REQ;

$CMD = "-o","obj/TextureManager.o","-c","src/texturemanager.cpp";
& $CPL $CMD $REQ;

$CMD = "-o","obj/ModelManager.o","-c","src/modelmanager.cpp";
& $CPL $CMD $REQ;

$CMD = "-o","obj/ShaderManager.o","-c","src/shadermanager.cpp";
& $CPL $CMD $REQ;

$CMD = "-o","obj/OpenGl.o", "-c","src/opengl.cpp";
& $CPL $CMD $REQ;


$CMD = "-o","OpenGL.exe","obj/OpenGl.o","obj/ShaderManager.o",
"obj/ModelManager.o","obj/TextureManager.o","obj/Camera.o",
"obj/Mouse.o";
& $CPL $CMD $LIBINC $LIB;

