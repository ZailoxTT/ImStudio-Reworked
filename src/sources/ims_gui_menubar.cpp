#include "ims_gui.h"
#include "../utils/filesystem_utils.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <iterator>

void ImStudio::GUI::ShowMenubar()
{
    ImGui::SetNextWindowPos(mb_P);
    ImGui::SetNextWindowSize(mb_S);
    ImGui::Begin("Menubar", NULL,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    static std::string error_message = "";

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {

          if (ImGui::MenuItem("Open Project...")) {
            open_dialog_open = true;
          }

    if (ImGui::MenuItem("Import project from clipboard"))
    {
        const char* text = ImGui::GetClipboardText();
        if (text && strlen(text) > 0)
        {
            try {
                bw.load_from_json(text);
                error_message = "";
            } catch (const std::exception& e) {
                error_message = "Parse error: " + std::string(e.what());
                ImGui::OpenPopup("Import Error");
            }
        }
    }

            ImGui::Separator();

#ifndef __EMSCRIPTEN__
            if (ImGui::MenuItem("Export C++ code to clipboard"))
            {
                ImGui::LogToClipboard();
                ImGui::LogText("%s", output.c_str());
                ImGui::LogFinish();
            }
#endif

            ImGui::Separator();

            if (ImGui::MenuItem("Export JSON to clipboard"))
            {
                std::string json_str = bw.to_json_string();
#ifdef __EMSCRIPTEN__
                JsClipboard_SetClipboardText(json_str.c_str());
#else
                ImGui::SetClipboardText(json_str.c_str());
#endif
            }

            // === SAVE PROJECT ===
            if (ImGui::MenuItem("Save Project As...")) {
              project_name = "MyProject";
              save_dialog_open = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit")) state = false;
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::BeginMenu("Layout"))
            {
                ImGui::MenuItem("Compact", NULL, &compact);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Behavior"))
            {
                ImGui::MenuItem("Static Mode", NULL, &bw.staticlayout);
                ImGui::SameLine();
                utils::HelpMarker("Toggle between static/linear layout and fixed/manual layout");

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Reset"))
            {
                bw.objects.clear();
                bw.selected_obj_id = -1;
                bw.open_child_id = -1;
                bw.open_child = false;
                bw.idgen = 0;
            }

            ImGui::EndMenu();
        }

        /// menu-tools
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Style Editor", NULL, &child_style);
            ImGui::MenuItem("Demo Window", NULL, &child_demo);
            ImGui::MenuItem("Metrics", NULL, &child_metrics);
            ImGui::MenuItem("Stack Tool", NULL, &child_stack);
            ImGui::MenuItem("Color Export", NULL, &child_color);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Resources")) child_resources = true;
            if (ImGui::MenuItem("About ImStudio")) child_about = true;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (open_dialog_open) {
        ImGui::OpenPopup("Open Project");
    }

    if (ImGui::BeginPopupModal("Open Project", &open_dialog_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::vector<std::string> projects;
        static bool loaded = false;

        if (ImGui::IsWindowAppearing()) {
            projects.clear();
            std::string dir = ImStudio::FileSystem::GetProjectDirectory();
            if (fs::exists(dir) && fs::is_directory(dir)) {
                for (const auto& entry : fs::directory_iterator(dir)) {
                    if (entry.path().extension() == ".json" && entry.path().filename() != "last_open.json") {
                        projects.push_back(entry.path().filename().string());
                    }
                }
            }
            loaded = true;
        }

        ImGui::Text("Select project to open:");
        ImGui::BeginChild("ProjectsList", ImVec2(300, 200), true);
        for (const auto& proj : projects) {
            if (ImGui::Selectable(proj.c_str())) {
                try {
                    std::string dir = ImStudio::FileSystem::GetProjectDirectory();
                    std::ifstream file(dir + "\\" + proj);
                    if (file.is_open()) {
                        std::string json((std::istreambuf_iterator<char>(file)), {});
                        file.close();
                        bw.load_from_json(json);
                        project_name = proj.substr(0, proj.size() - 5);
                        ImGui::CloseCurrentPopup();
                        open_dialog_open = false;
                    }
                } catch (...) {
                    error_message = "Failed to load: " + proj;
                    ImGui::OpenPopup("Load Error");
                }
            }
        }
        ImGui::EndChild();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
            open_dialog_open = false;
        }
        ImGui::EndPopup();
        }

    // === SAVE DIALOG ===
    if (save_dialog_open) {
      ImGui::OpenPopup("Save Project");
    }

    if (ImGui::BeginPopupModal("Save Project", &save_dialog_open, ImGuiWindowFlags_AlwaysAutoResize))
    {

      static char buf[256] = "";
      if (ImGui::IsWindowAppearing()) {
          strcpy_s(buf, project_name.c_str());
      }

      ImGui::Text("Project name:");
      ImGui::InputText("##name", buf, IM_ARRAYSIZE(buf));

      if (ImGui::Button("Save"))
      {
        std::string name = buf;
        if (!name.empty())
        {
          try {
            std::string json = bw.to_json_string();
            std::string path = ImStudio::FileSystem::GetProjectFilePath(name);

            std::ofstream file(path);
            if (file.is_open()) {
              std::cout << "isopen 1"<<"\n";
              file << json;
              file.close();
              std::string last_path = ImStudio::FileSystem::GetProjectDirectory() + "\\last_open.json";
              std::ofstream last_file(last_path);
              if (last_file.is_open()) {
                std::cout << "isopen 2"<<"\n";
                last_file << json;
                last_file.close();
              }
              project_name = name;
              save_dialog_open = false;
            } else {
              error_message = "Cannot write to: " + path;
              ImGui::OpenPopup("Save Error");
            }
          } catch (const std::exception& e) {
            error_message = std::string("Save error: ") + e.what();
            ImGui::OpenPopup("Save Error");
          }
        }
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel")) {
        save_dialog_open = false;
      }
    ImGui::EndPopup();
    }

    // === ERRORS ===
    if (ImGui::BeginPopupModal("Import Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Error: %s", error_message.c_str());
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Save Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Error: %s", error_message.c_str());
        if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (!compact)
    {
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
        {
            // tab-create
            if (ImGui::BeginTabItem("Create"))
            {
                wksp_output = false;
                wksp_create = true;
                ImGui::EndTabItem();
            }

            // tab-output
            if (ImGui::BeginTabItem("Output"))
            {
                wksp_create = false;
                wksp_output = true;
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    ImGui::End();
}
