#include "glinit.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <iostream>

static void error_callback(int error, const char* description)
{
    std::cerr << "Error " << error << ": " << description << std::endl;
}

GLInit::GLInit() : m_window(0) {}

GLInit::~GLInit() { deinit(); }

void GLInit::init(const std::string& window_name, int width, int height,
                  bool useImgui)
{
    m_useImgui = useImgui;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        std::cerr << "Error glfwInit()" << std::endl;
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    gl3wInit();

    if (m_useImgui)
    {
        ImGui::CreateContext();
        ImGui::GetIO();
        ImGui_ImplGlfwGL3_Init(m_window, true);
    }
}

bool GLInit::shouldClose() { return glfwWindowShouldClose(m_window); }

void GLInit::beginFrame()
{
    glfwPollEvents();
    if (m_useImgui)
    {
        ImGui_ImplGlfwGL3_NewFrame();
    }

    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLInit::endFrame()
{
    if (m_useImgui)
    {
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glfwSwapBuffers(m_window);
}

void GLInit::deinit()
{
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
