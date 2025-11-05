#include "ims_buffer.h"

void ImStudio::BufferWindow::drawall()
{
    if (state)
    {
        ImVec2 parent_size = ImGui::GetWindowSize();
        ImVec2 init_size   = ImVec2(parent_size.x * 0.8, parent_size.y * 0.7);
        ImVec2 center      = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        if (size.x <= 0 || size.y <= 0) {
          ImGui::SetNextWindowSize(init_size, ImGuiCond_Once);
        } else {
          ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        }
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.09f, 0.09f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.09f, 0.09f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.86f, 0.86f, 0.86f, 0.50f));
        ImGui::Begin("buffer", &state);
        size = ImGui::GetWindowSize();
        pos  = ImGui::GetWindowPos();
        {
            // Draw gl cursor when dragging
            if (ImGui::IsMouseDown(0))
            {
                ImGui::GetIO().MouseDrawCursor = true;
            }
            else
            {
                ImGui::GetIO().MouseDrawCursor = false;
            }

            //HOTKEY: ALT + M - "Add" Context Menu
            if ((ImGui::IsWindowHovered()) &&
                ((ImGui::IsKeyDown(ImGuiKey_ModAlt) && (ImGui::IsKeyPressed(ImGuiKey_M))) ||
                (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))))
            {
                ImGui::OpenPopup("bwcontextmenu");

            }
            if (ImGui::BeginPopupContextWindow("bwcontextmenu"))
            {
                if (ImGui::BeginMenu("Add"))
                {
                    if (ImGui::BeginMenu("Primitives"))
                    {
                        if (ImGui::MenuItem("Button"))
                            create("button",1);
                        if (ImGui::MenuItem("Radio Button"))
                            create("radio",1);
                        if (ImGui::MenuItem("Checkbox"))
                            create("checkbox",1);
                        if (ImGui::MenuItem("Text"))
                            create("text",1);
                        if (ImGui::MenuItem("Bullet"))
                            create("bullet",1);
                        if (ImGui::MenuItem("Arrow"))
                            create("arrow",1);
                        if (ImGui::MenuItem("Combo"))
                            create("combo",1);
                        if (ImGui::MenuItem("Listbox"))
                            create("listbox",1);
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Data Inputs"))
                    {
                        if (ImGui::MenuItem("Input Text"))
                            create("textinput",1);
                        if (ImGui::MenuItem("Input Int"))
                            create("inputint",1);
                        if (ImGui::MenuItem("Input Float"))
                            create("inputfloat",1);
                        if (ImGui::MenuItem("Input Double"))
                            create("inputdouble",1);
                        if (ImGui::MenuItem("Input Scientific"))
                            create("inputscientific",1);
                        if (ImGui::MenuItem("Input Float3"))
                            create("inputfloat3",1);
                        if (ImGui::MenuItem("Drag Int"))
                            create("dragint",1);
                        if (ImGui::MenuItem("Drag Int %"))
                            create("dragint100",1);
                        if (ImGui::MenuItem("Drag Float"))
                            create("dragfloat",1);
                        if (ImGui::MenuItem("Drag Float Small"))
                            create("dragfloatsmall",1);
                        if (ImGui::MenuItem("Slider Int"))
                            create("sliderint",1);
                        if (ImGui::MenuItem("Slider Float"))
                            create("sliderfloat",1);
                        if (ImGui::MenuItem("Slider Float Log"))
                            create("sliderfloatlog",1);
                        if (ImGui::MenuItem("Slider Angle"))
                            create("sliderangle",1);
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Color Pickers"))
                    {
                        if (ImGui::MenuItem("Color 1"))
                            create("color1",1);
                        if (ImGui::MenuItem("Color 2"))
                            create("color2",1);
                        if (ImGui::MenuItem("Color 3"))
                            create("color3",1);
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }

            for (auto i = objects.begin(); i != objects.end(); ++i)
            {
                Object &o = *i;

                if (o.state == false)
                {
                    i = objects.erase(i);
                    break;
                }
                else
                {
                    if (o.type != "child")
                    {
                        o.draw(&selected_obj_id, staticlayout);
                    }
                    else
                    {
                        o.child.drawall(&selected_obj_id, staticlayout);
                    }
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleColor(4);
    }
}

ImStudio::Object *ImStudio::BufferWindow::getobj(int id)
{
    for (Object &o : objects)
    {
        if (o.id == id)
        {
            return &o;
        }
    }
    return nullptr;
}

ImStudio::BaseObject *ImStudio::BufferWindow::getbaseobj(int id)
{
    for (Object &o : objects)
    {
        if (o.id == id)
        {
            return &o;
        }
        if (!o.child.objects.empty())
        {
            for (BaseObject &cw : o.child.objects)
            {
                if (cw.id == id)
                {
                    return &cw;
                }
            }
        }
    }
    return nullptr;
}

void ImStudio::BufferWindow::create(std::string type_, bool atcursor)
{
    idgen++;
    if (!((open_child) &&
          (getbaseobj(open_child_id)) &&
          (getobj(open_child_id)->child.open)))
    {
        // no child window open
        Object widget(idgen, type_);
        if (atcursor)
        {
            widget.pos = ImVec2(ImGui::GetMousePos().x - pos.x, ImGui::GetMousePos().y - pos.y);
        }
        objects.push_back(widget);
    }
    else
    {
        // child window open
        BaseObject childwidget(idgen, type_, open_child_id);
        getobj(open_child_id)->child.objects.push_back(childwidget);
    }
    selected_obj_id = idgen;
}


std::string ImStudio::BufferWindow::to_json_string() const {
    json j;
    j["state"] = state;
    j["size"] = size;
    j["pos"] = pos;
    j["idgen"] = idgen;
    j["open_child"] = open_child;
    j["open_child_id"] = open_child_id;
    j["selected_obj_id"] = selected_obj_id;
    j["staticlayout"] = staticlayout;
    json objects_json = json::array();
    for (const auto& obj : objects) {
        json obj_j;
        obj.to_json(obj_j);
        objects_json.push_back(obj_j);
    }
    j["objects"] = objects_json;
    return j.dump(4);
}

void ImStudio::BufferWindow::load_from_json(const std::string& json_str) {
    json j = json::parse(json_str);
    state = j.at("state").get<bool>();
    size = j.at("size").get<ImVec2>();
    pos = j.at("pos").get<ImVec2>();
    idgen = j.at("idgen").get<int>();
    open_child = j.at("open_child").get<bool>();
    open_child_id = j.at("open_child_id").get<int>();
    selected_obj_id = j.at("selected_obj_id").get<int>();
    staticlayout = j.at("staticlayout").get<bool>();
    objects.clear();
    if (j.contains("objects")) {
        for (const auto& obj_j : j.at("objects")) {
            Object obj(0, "");
            obj.from_json(obj_j);
            objects.push_back(obj);
        }
    }
    int max_id = 0;
    for (const auto& obj : objects) {
        max_id = std::max(max_id, obj.id);
        for (const auto& child_obj : obj.child.objects) {
            max_id = std::max(max_id, child_obj.id);
        }
    }
    idgen = max_id + 1;
}
