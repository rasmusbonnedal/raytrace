#pragma once
#include <GL/gl3w.h>

#include <iostream>

static const char* gl_error_to_string(GLenum err)
{
    switch (err)
    {
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:
            return "UNKNOWN_ERROR";
    }
}

#if !defined(NDEBUG) || defined(ENABLE_GL_ERROR_CHECKING)
#define CHECK_GL(x)                                               \
    {                                                             \
        x;                                                        \
        if (GLenum e = glGetError())                              \
        {                                                         \
            std::cerr << "Error in GL call " << #x << std::endl   \
                      << gl_error_to_string(e) << "(" << e << ")" \
                      << std::endl;                               \
            exit(e);                                              \
        }                                                         \
    }
#else
#define CHECK_GL(x) x
#endif
