PCH_FILE = include/pch.hpp.gch
SRCS =  include/glm/detail/glm.cpp  $(wildcard jsoncpp-1.9.5/lib_json/*.cpp) $(wildcard src/*.cpp) $(wildcard imgui-1.90/*.cpp) imgui-1.90/backends/imgui_impl_glfw.cpp imgui-1.90/backends/imgui_impl_opengl3.cpp 
OBJS = $(SRCS:%.cpp=%.o)
HEADERS = -Iinclude -Iimgui-1.90/  -Iglad/include/ $(shell pkg-config --cflags glfw3) $(shell pkg-config --cflags opengl)    -I./jsoncpp-1.9.5/include/ -I./jsoncpp-1.9.5/include/json/
INCLUDES = $(HEADERS)   $(shell pkg-config --libs glfw3) $(shell pkg-config --libs opengl) 

SHADERS = shader.frag shader.vert

all: main
	./build/main
	# rm build/main
	

main: $(OBJS) build/glad.o 
	g++ main.cpp -g $(INCLUDES) $(OBJS) glad/src/glad.o -o build/main

$(PCH_FILE): include/pch.hpp
	g++ $(HEADERS) include/pch.hpp -o $(PCH_FILE)

build/glad.o:
	gcc $(HEADERS) -c ./glad/src/glad.c -o build/glad.o  

%.o: %.cpp $(PCH_FILE)
	g++ $(INCLUDES) -include include/pch.hpp -c $< -o $@

