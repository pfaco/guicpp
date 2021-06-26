#include "gui/backend.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ees::gui
{

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
static void teardown(GLFWwindow *window);

BackendContext backend_init(int width, int height, const char *title, const uint8_t *font_data, size_t font_data_size, float font_size)
{
    if (!glfwInit())
    {
        throw -1;
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER , 1);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    float highDPIscaleFactor = 1.0;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    float xscale, yscale;
    glfwGetMonitorContentScale(monitor, &xscale, &yscale);
    if (xscale > 1 || yscale > 1)
    {
        highDPIscaleFactor = xscale;
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    }

    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        teardown(NULL);
        throw -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync

    if (gl3wInit())
    {
        throw -1;
    }

    int actualWindowWidth, actualWindowHeight;
    glfwGetWindowSize(window, &actualWindowWidth, &actualWindowHeight);
    glViewport(0, 0, actualWindowWidth, actualWindowHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    if (font_data == nullptr) {
        ImGui::GetIO().Fonts->AddFontDefault();
    } else {
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        ImGui::GetIO().Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(const_cast<uint8_t*>(font_data)), font_data_size, font_size, &fontConfig);
    }
    return BackendContext{window};
}

bool backend_should_close(BackendContext ctx)
{
    return glfwWindowShouldClose((GLFWwindow *)ctx.window);
}

void backend_set_frame(BackendContext ctx)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void backend_render(BackendContext ctx)
{
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize((GLFWwindow *)ctx.window, &display_w, &display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers((GLFWwindow *)ctx.window);
}

void backend_teardown(BackendContext ctx)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    teardown((GLFWwindow *)ctx.window);
}

static auto backend_load_texture(unsigned char* image_data, int *width, int *height) -> BackendTexture
{
    if (image_data == NULL)
        throw -1;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    return BackendTexture{image_texture};
}

auto backend_load_texture(const char *path, int *width, int *height) -> BackendTexture
{
    return backend_load_texture(stbi_load(path, width, height, NULL, 4), width, height);
}

auto backend_load_texture(const uint8_t *data, size_t size, int *width, int *height) -> BackendTexture
{
    return backend_load_texture(stbi_load_from_memory(data, size, width, height, NULL, 4), width, height);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void teardown(GLFWwindow *window)
{
    if (window != NULL) { glfwDestroyWindow(window); }
    glfwTerminate();
}

}
