#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>

class GLInit
{
public:
    GLInit();
    ~GLInit();
    void init(const std::string &window_name, int width, int height, bool useImgui);
    bool shouldClose();
    void beginFrame();
    void endFrame();
    void deinit();

private:
    GLFWwindow *m_window;
    bool m_useImgui;
};
