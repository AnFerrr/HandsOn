#include "../include/context.hpp"

namespace HandsOn {
    void Context::AddJoystick(std::shared_ptr<Joystick> joystick) {
        this->js_list.push_back(std::move(joystick));
    }
    void Context::AddVJoystick(std::shared_ptr<VJoystick> vjoystick) {
        this->vjs_list.push_back(std::move(vjoystick));
    }

    void Context::RemoveJoystick(int joystick_index) {
        js_list.erase(js_list.begin() + joystick_index);
    };
    void Context::RemoveVJoystick(int vjoystick_index) {
        vjs_list.erase(vjs_list.begin() + vjoystick_index);
    };
}
