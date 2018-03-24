#define ENABLE_GL_ERROR_CHECKING 1
#include "glerror.h"
#include "glinit.h"
#include "gltextureblitter.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

int main(int, char**)
{
    GLInit glState;
    glState.init("Window", 640, 480, true);

    while (!glState.shouldClose())
    {  
        glState.beginFrame();

        ImGui::Begin("ImGui Window");
        ImGui::Button("Button");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        glState.endFrame();
    }
    return 0;
}
