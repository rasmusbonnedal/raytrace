cmake_minimum_required(VERSION 2.8)

SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "")
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "")
ADD_SUBDIRECTORY(glfw)
SET(GLFW_INCLUDE_DIRS glfw/include)
LIST(APPEND GLFW_LIBRARIES glfw)

FIND_PACKAGE(OpenGL REQUIRED)

INCLUDE_DIRECTORIES(${GLFW_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(imgui imgui/examples/opengl3_example imgui/examples/libs/gl3w)
LINK_DIRECTORIES(${GLFW_LIBRARY_DIRS})

IF (APPLE)
  	SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
ENDIF ()

ADD_EXECUTABLE(raytrace
	main.cpp
	color.h
	glerror.h
	glinit.cpp
	glinit.h
	gltextureblitter.cpp
	gltextureblitter.h
	intersection.h
	ray.h
	raytracer.cpp
	raytracer.h
	vecmath.h
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/examples/libs/gl3w/GL/gl3w.c
	imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp
)

IF (APPLE)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit)
   TARGET_LINK_LIBRARIES(raytrace ${COCOA_LIBRARY} ${IOKIT_LIBRARY})
ENDIF ()

TARGET_LINK_LIBRARIES(raytrace ${OPENGL_LIBRARY} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES})
