cmake_minimum_required(VERSION 3.10)

project(cells)

add_executable(cells main.cpp)

add_subdirectory(Thread-Pool)
target_include_directories(cells PRIVATE Thread-Pool)

SET(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(cells ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} thread_pool)