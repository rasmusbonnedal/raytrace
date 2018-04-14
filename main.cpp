#define ENABLE_GL_ERROR_CHECKING 1
#include "glerror.h"
#include "glinit.h"
#include "gltextureblitter.h"
#include "raytracer.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

int main(int, char**)
{
    GLInit glState;
    glState.init("Window", 720, 480, true);
    RayTracer raytracer;

    raytracer.init(720, 480);
    raytracer.start();

    GLTextureBlitter texBlit;
    texBlit.init(720, 480);

    while (!glState.shouldClose())
    {
        texBlit.update(raytracer.lockFramebuffer());
        raytracer.unlockFramebuffer();

        glState.beginFrame();
        texBlit.blit();

        ImGui::Begin("ImGui Window");
        ImGui::Button("Button");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Text("Render average %.1f ms/frame (%.1f FPS)",
                    1000.0 / raytracer.getFPS(), raytracer.getFPS());
        ImGui::End();
        glState.endFrame();
    }
    raytracer.stop();
    return 0;
}
