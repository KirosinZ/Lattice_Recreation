cmake_minimum_required(VERSION 3.22)
project(R3DS_Lattice)

set(CMAKE_CXX_STANDARD 23)

add_executable(R3DS_Lattice camera/Camera.h camera/Camera.cpp mesh/Mesh.cpp mesh/Mesh.h lattice/Lattice.cpp lattice/Lattice.h multipointer/multipointer.h lattice/kernels/kernels.h program/Program.cpp program/Program.h program/Shader.cpp program/Shader.h parser/parser.cpp parser/parser.h render/RenderObject.cpp render/RenderObject.h render/Scene.cpp render/Scene.h main.cpp mesh/SparsePoint.cpp mesh/SparsePoint.h lattice/kernels/kernels.cpp)


target_link_libraries(R3DS_Lattice opengl32.lib)
target_link_libraries(R3DS_Lattice glew32.lib)
target_link_libraries(R3DS_Lattice glfw3.lib)
