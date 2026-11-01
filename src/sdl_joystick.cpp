#include "../include/sdl_joystick.hpp"

namespace HandsOn {
    void SDLJoystick::Update() {
        int axis;

        for (auto& input : inputs) {
            axis = SDL_GetJoystickAxis(this->js, input.first);
            input.second->value = axis;
        }
    };

    void SDLJoystick::SetAxis(int axis, int value) {

    };

    int SDLJoystick::GetAxis(int axis) {
        return inputs[axis]->value;
    };
}
