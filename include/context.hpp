#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "../include/joystick.hpp"
#include "../include/sdl_joystick.hpp"
#include "../include/vjoystick.hpp"

namespace HandsOn {
    struct Context {
        Context() = default;
        ~Context() {};
        
        SDL_Window* window;
        SDL_GLContext gl_context;

        std::vector<std::shared_ptr<Joystick>> js_list;
        std::vector<std::shared_ptr<Joystick>> vjs_list;

        std::unordered_map<int, int> key_binds;
        std::unordered_map<int, int> axis_binds;

        bool stopping = false;
    };
}

#endif // CONTEXT_HPP
