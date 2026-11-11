#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../include/json.hpp"

using nlohmann::json;

// TODO: check for file error.
// BUG: loading twice crashes
void HandsOn::load_config(HandsOn::Context& context) {
    auto& js_list = context.GetJSList();
    auto& vjs_list = context.GetVJSList();
    std::ifstream f("config.json");
    json data = json::parse(f);

    int index = 0;
    for (auto& json_vjoystick : data["virtual_joysticks"]) {
        vjs_list.push_back(std::move(std::make_shared<HandsOn::VJoystick>(
                        json_vjoystick["name"].get<std::string>(),
                        json_vjoystick["naxes"].get<int>(),
                        json_vjoystick["nbuttons"].get<int>(),
                        json_vjoystick["vid"].get<int>())));
        if (((HandsOn::VJoystick*)
                    vjs_list[json_vjoystick["vid"].get<int>()]
                    .get())->Initialize() != 0)
            throw std::runtime_error("Couldn't create virtual Joystick.");
        index++;
    }

    index = 0;
    for (auto& json_joystick : data["hardware_joysticks"]) {
        for (auto& joystick : js_list) {
                std::cout << "Test" << std::endl;
            auto sdl_joystick = (HandsOn::SDLJoystick*)joystick.get();
            std::string guid;
            for (auto& data : SDL_GetJoystickGUID(sdl_joystick->js).data)
                guid += std::to_string(data);

            if (guid == json_joystick["guid"].get<std::string>()) {
                for (int i = 0; i < json_joystick["inputs"].size(); i++) {
                    auto& input = sdl_joystick->GetInput(i);
                    auto& json_input = json_joystick["inputs"][std::to_string(i)];
                    input.SetDestinationInput(json_input["dest_input"].get<int>());
                    input.SetDestinationType(json_input["dest_type"].get<int>());
                    input.SetType(json_input["type"].get<int>());

                    auto comp = [&](std::shared_ptr<HandsOn::Joystick> c_joystick) {
                        return c_joystick->GetName() == json_input["vjs_dest"];
                    };

                    auto it = std::find_if(vjs_list.begin(),
                            vjs_list.end(), comp);

                    std::shared_ptr<HandsOn::Joystick> selected = nullptr;
                    if (it != vjs_list.end())
                        selected = *it.base();
                    input.SetDestinationJoystick((json_input["vjs_dest"] == "none" ?
                            nullptr : selected));
                }
            }
        }
    }
}

nlohmann::json write_inputs(HandsOn::SDLJoystick& joystick) {
    nlohmann::json inputs;

    for (int i = 0; i < joystick.GetInputCount(); i++) {
        auto& input = joystick.GetInput(i);
        nlohmann::json json_inputs;
        json_inputs["type"] = input.GetType();
        json_inputs["dest_type"] = input.GetDestinationType();
        json_inputs["dest_input"] = input.GetDestinationInput();
        json_inputs["vjs_dest"] =
            (input() ? input.GetDestinationJoystick()->GetName() : "none");
        inputs[std::to_string(i)] = json_inputs; 
    }

    return inputs;
};

// TODO: check for file error.
void HandsOn::write_config(HandsOn::Context& context) {
    std::ofstream f("config.json");
    nlohmann::json config;
    const auto& js_list = context.GetJSList();
    const auto& vjs_list = context.GetVJSList();

    for (const auto& joystick : js_list) {
        auto sdl_joystick =
            (HandsOn::SDLJoystick*)joystick.get();

        nlohmann::json json_joystick;
        std::string guid;

        for (auto& data : SDL_GetJoystickGUID(sdl_joystick->js).data)
            guid += std::to_string(data);

        json_joystick["inputs"] = write_inputs(*sdl_joystick);
        json_joystick["guid"] = guid;
        json_joystick["name"] = sdl_joystick->GetName();

        config["hardware_joysticks"].push_back(json_joystick);
    }
    for (const auto& joystick : vjs_list) {
        auto v_joystick =
            (HandsOn::VJoystick*)joystick.get();

        nlohmann::json json_joystick;
        std::string guid;

        json_joystick["nbuttons"] = v_joystick->GetButtonsCount();
        json_joystick["naxes"] = v_joystick->GetAxesCount();
        json_joystick["vid"] = v_joystick->GetVID();
        json_joystick["name"] = v_joystick->GetName();

        config["virtual_joysticks"].push_back(json_joystick);
    }

    f << config.dump(4) << std::endl;
}
