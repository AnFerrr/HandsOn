#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "../include/joystick.hpp"

namespace HandsOn {
    struct Context {
        Context() = default;
        ~Context() {};
        std::vector<SDLJoystick> js_list;

        std::shared_ptr<SDLJoystick> js_selected;
        std::shared_ptr<VJoystick> vjs;

        std::unordered_map<int, int> key_binds;
        std::unordered_map<int, int> axis_binds;
    };
}

#endif // CONTEXT_HPP
