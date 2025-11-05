#include "ims_object.h"

ImStudio::BaseObject::BaseObject(int idgen_, std::string type_, int parent_id_) // for child widgets
{
    ischildwidget = true;
    id            = idgen_;
    type          = type_;
    identifier    = "child" + std::to_string(parent_id_) + "::" + type_ + std::to_string(idgen_);
    value_s       = type_ + std::to_string(idgen_);
}

ImStudio::Object::Object(int idgen_, std::string type_) : BaseObject()
{
    if (type_ == "child")
    {
        child.objects.reserve(250);
        child.open = true;
        child.id   = idgen_;
    }
    id         = idgen_;
    type       = type_;
    identifier = type_ + std::to_string(idgen_);
    value_s    = type_ + std::to_string(idgen_);
}

//void ImStudio::BaseObject::draw(int *select, bool staticlayout = false)
// moved to ims_object_draw.cpp

void ImStudio::BaseObject::del()
{
    state = false;
}

void ImStudio::BaseObject::highlight(int *select)
{
    if (id == *select)
    {
        ImRect itemrect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
        itemrect.Min.x -= 5;
        itemrect.Min.y -= 5;
        itemrect.Max.x += 5;
        itemrect.Max.y += 5;
        ImGui::GetForegroundDrawList()->AddRect(itemrect.Min, itemrect.Max, IM_COL32(255, 255, 0, 255));
    }
}

void ImStudio::ContainerChild::drawall(int *select, bool staticlayout)
{
    static auto dl = ImGui::GetWindowDrawList();

    if (!grabinit)
    {
        srand(time(NULL));
        grab1_id = rand() % 100;
        grab2_id = grab1_id + 1;
    }

    freerect.Min.x = grab1.x;
    freerect.Min.y = grab1.y;
    freerect.Max.x = grab2.x + 15;
    freerect.Max.y = grab2.y + 14;

    if (!staticlayout)
        ImGui::SetCursorPos(freerect.Min);
    ImGui::BeginChild(id, freerect.GetSize(), border,
                      ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
    for (auto i = objects.begin(); i != objects.end(); ++i)
    {
        BaseObject &o = *i;
        if (o.state == false)
        {
            i = objects.erase(i);
            break;
        }
        else
        {
            o.draw(select, staticlayout);
        }
    }
    ImGui::EndChild();
    windowrect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

    if (open)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.000f, 1.000f, 0.110f, 1.000f));
    if (!staticlayout)
    {
        if(!locked){
        if (utils::GrabButton(grab1, grab1_id))
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            grab1   = utils::GetLocalCursor();
            *select = id;
        }

        if (utils::GrabButton(grab2, grab2_id))
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
            grab2   = utils::GetLocalCursor();
            *select = id;
        }}
    }
    if (open)
        ImGui::PopStyleColor(1);

    windowrect.Min.x -= 5;
    windowrect.Min.y -= 5;
    windowrect.Max.x += 5;
    windowrect.Max.y += 5;

    if (id == *select)
    {
        if (open)
        {
            dl->AddRect(windowrect.Min, windowrect.Max, IM_COL32(0, 255, 28, 255));
        }
        else
        {
            dl->AddRect(windowrect.Min, windowrect.Max, IM_COL32(255, 255, 0, 255));
        }
    }

    grabinit = true;
}

void ImStudio::BaseObject::to_json(json& j) const {
    j["id"] = id;
    j["type"] = type;
    j["identifier"] = identifier;
    j["state"] = state;
    j["pos"] = pos;
    j["size"] = size;
    j["width"] = width;
    j["init"] = init;
    j["propinit"] = propinit;
    j["initial_selection"] = initial_selection;
    j["locked"] = locked;
    j["center_h"] = center_h;
    j["autoresize"] = autoresize;
    j["animate"] = animate;
    j["label"] = label;
    j["value_s"] = value_s;
    j["value_b"] = value_b;
    j["ischildwidget"] = ischildwidget;
    j["item_current"] = item_current;
}

void ImStudio::BaseObject::from_json(const json& j) {
    id = j.at("id").get<int>();
    type = j.at("type").get<std::string>();
    identifier = j.at("identifier").get<std::string>();
    state = j.at("state").get<bool>();
    pos = j.at("pos").get<ImVec2>();
    size = j.at("size").get<ImVec2>();
    width = j.at("width").get<float>();
    init = j.at("init").get<bool>();
    propinit = j.at("propinit").get<bool>();
    initial_selection = j.at("initial_selection").get<bool>();
    locked = j.at("locked").get<bool>();
    center_h = j.at("center_h").get<bool>();
    autoresize = j.at("autoresize").get<bool>();
    animate = j.at("animate").get<bool>();
    label = j.at("label").get<std::string>();
    value_s = j.at("value_s").get<std::string>();
    value_b = j.at("value_b").get<bool>();
    ischildwidget = j.at("ischildwidget").get<bool>();
    item_current = j.at("item_current").get<int>();
}

void ImStudio::ContainerChild::to_json(json& j) const {
    j["id"] = id;
    j["freerect"] = freerect;
    j["windowrect"] = windowrect;
    j["open"] = open;
    j["locked"] = locked;
    j["border"] = border;
    j["init"] = init;
    j["grab1_id"] = grab1_id;
    j["grab2_id"] = grab2_id;
    j["grab1"] = grab1;
    j["grab2"] = grab2;
    j["grabinit"] = grabinit;
    json objects_json = json::array();
    for (const auto& obj : objects) {
        json obj_j;
        obj.to_json(obj_j);
        objects_json.push_back(obj_j);
    }
    j["objects"] = objects_json;
}

void ImStudio::ContainerChild::from_json(const json& j) {
    id = j.at("id").get<int>();
    freerect = j.at("freerect").get<ImRect>();
    windowrect = j.at("windowrect").get<ImRect>();
    open = j.at("open").get<bool>();
    locked = j.at("locked").get<bool>();
    border = j.at("border").get<bool>();
    init = j.at("init").get<bool>();
    grab1_id = j.at("grab1_id").get<int>();
    grab2_id = j.at("grab2_id").get<int>();
    grab1 = j.at("grab1").get<ImVec2>();
    grab2 = j.at("grab2").get<ImVec2>();
    grabinit = j.at("grabinit").get<bool>();
    objects.clear();
    if (j.contains("objects")) {
        for (const auto& obj_j : j.at("objects")) {
            BaseObject obj;
            obj.from_json(obj_j);
            objects.push_back(obj);
        }
    }
}

void ImStudio::Object::to_json(json& j) const {
    BaseObject::to_json(j);
    json child_j;
    child.to_json(child_j);
    j["child"] = child_j;
}

void ImStudio::Object::from_json(const json& j) {
    BaseObject::from_json(j);
    child.from_json(j.at("child"));
}
