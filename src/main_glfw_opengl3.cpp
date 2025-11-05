#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <iterator>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "main_window.h"

#include "utils/filesystem_utils.h"

#ifndef PROJECT_VERSION_STRING
#define PROJECT_VERSION_STRING "0.0.0"
#endif
#ifndef GIT_SHA1
#define GIT_SHA1 "0000000"
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
        {
            printf("%s\n", PROJECT_VERSION_STRING);
            return 0;
        }
        if (strcmp(argv[1], "--hash") == 0)
        {
            printf("%s\n", GIT_SHA1);
            return 0;
        }
    }

    std::cout << "Project dir: " << ImStudio::FileSystem::GetProjectDirectory() << std::endl;

    ImStudio::GUI gui;

    ImStudio::FileSystem::EnsureProjectDirectory();
    std::string last_path = ImStudio::FileSystem::GetProjectDirectory() + "\\last_open.json";

    std::ifstream file(last_path);
    if (file.is_open()) {
      std::string json((std::istreambuf_iterator<char>(file)), {});
      file.close();
      try {
        gui.bw.load_from_json(json);
      } catch (...) {
        //
      }
    }

    gui.bw.objects.reserve(2048);

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

#if defined(IMGUI_IMPL_OPENGL_ES2)
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER,
                   GLFW_TRUE);

    GLFWwindow *glwindow = glfwCreateWindow(1280, 720, "ImStudio", NULL, NULL);

    if (glwindow == NULL)
        return 1;

    glfwMakeContextCurrent(glwindow);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    MainWindowStyle();

    ImGui_ImplGlfw_InitForOpenGL(glwindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while ((!glfwWindowShouldClose(glwindow)) && (gui.state))
    {
      gui.autosave_timer += ImGui::GetIO().DeltaTime;
      if (gui.autosave_timer >= 3.0f && gui.last_write != true) {
        try {
          std::string json = gui.bw.to_json_string();
          std::string path = ImStudio::FileSystem::GetProjectDirectory() + "\\last_open.json";
          std::ofstream file(path);
          if (file.is_open()) {
              file << json;
              file.close();
              gui.last_write = true;
            }
          } catch (...) {}
          //gui.autosave_timer = 0.0f;
        }
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        MainWindowGUI(gui);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(glwindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        static ImVec4 bg = ImVec4(0.123f, 0.123f, 0.123, 1.00f);
        glClearColor(bg.x * bg.w, bg.y * bg.w, bg.z * bg.w, bg.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glwindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(glwindow);
    glfwTerminate();

    return 0;
}
