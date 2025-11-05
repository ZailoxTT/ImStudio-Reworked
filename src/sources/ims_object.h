#pragma once

#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"

#include "ims_utils.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace nlohmann {
    template <>
    struct adl_serializer<ImVec2> {
        static void to_json(json& j, const ImVec2& v) {
            j = json::array({v.x, v.y});
        }
        static void from_json(const json& j, ImVec2& v) {
            if (j.is_array() && j.size() == 2) {
                v.x = j.at(0).get<float>();
                v.y = j.at(1).get<float>();
            } else {
                throw json::type_error::create(302, "Expected array of 2 floats for ImVec2", nullptr);
            }
        }
    };

    template <>
    struct adl_serializer<ImRect> {
        static void to_json(json& j, const ImRect& r) {
            j["min"] = r.Min;
            j["max"] = r.Max;
        }
        static void from_json(const json& j, ImRect& r) {
            r.Min = j.at("min").get<ImVec2>();
            r.Max = j.at("max").get<ImVec2>();
        }
    };
} // namespace nlohmann

namespace ImStudio
{

  class Object;

  class BaseObject
  {
    public:
      int                     id                      = 0;                    // Unique ID
      std::string             type                    = {};                   // Widget type
      std::string             identifier              = {};                   // type+id
      bool                    state                   = true;                 // Alive

      ImVec2                  pos                     = ImVec2(100, 100);     //--
      ImVec2                  size                    = {};                   //  | Widget vectors
      float                   width                   = 200;                  //--

      bool                    init                    = false;                //--
      bool                    propinit                = false;                //  | Initialised
      bool                    initial_selection       = true;                 //--

      bool                    locked                  = false;                //--
      bool                    center_h                = false;                //  | Properties
      bool                    autoresize              = true;                 //  |
      bool                    animate                 = true;                 //--

      std::string             label                   = "Label";              //--
      std::string             value_s                 = {};                   //  | Widget values/contents
      bool                    value_b                 = false;                //--

      bool                    ischildwidget           = false;                //--

      int                     item_current            = 0;                    //

      void draw               (int *select,           bool staticlayout);
      void del                ();

      BaseObject              ()                      = default;
      BaseObject              (int idgen_,            std::string type_,      int parent_id_);

      void to_json(json& j) const;
      void from_json(const json& j);

    private:
      void highlight          (int *select);
  };

  struct ContainerChild
  {
      int                     id                      = 0;                    // Unique ID

      ImRect                  freerect                = {};                   // Buffer rect ctrld by grabs
      ImRect                  windowrect              = {};                   // Window dimensions for highlighting

      bool                    open                    = true;                 //--
      bool                    locked                  = false;                //  | Properties
      bool                    border                  = true;                 //  |
      bool                    init                    = false;                //--

      int                     grab1_id                = 0;                    //--
      int                     grab2_id                = 0;                    //  | ugly
      ImVec2                  grab1                   = ImVec2(90, 90);       //  | storage
      ImVec2                  grab2                   = ImVec2(200, 200);     //  | stuff
      bool                    grabinit                = false;                //--

      std::vector<BaseObject> objects                 = {};
      void drawall            (int *select,           bool staticlayout);

      void to_json(json& j) const;
      void from_json(const json& j);
  };

  //Object can now store either a single BaseObject or a vector of BaseObjects
  class Object : public BaseObject
  {
    public:
    //BaseObject{}
      ContainerChild          child;
    //ContainerGroup          group;
      Object                  (int idgen_, std::string type_);

      void to_json(json& j) const;
      void from_json(const json& j);
  };

}
