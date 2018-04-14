cmake_minimum_required(VERSION 2.8)

ADD_SUBDIRECTORY(modules)

FIND_PACKAGE(OpenGL REQUIRED)

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
	settings.cpp
	settings.h
	vecmath.h
)

target_link_libraries(raytrace variant glfw imgui_glfw)
target_link_libraries(raytrace ${OPENGL_LIBRARY})
