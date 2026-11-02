#include "../include/sdl_joystick.hpp"

namespace HandsOn {
    SDLJoystick::SDLJoystick(std::string name,
            short naxes,
            short nbuttons,
            SDL_Joystick* js)
        : Joystick(name, naxes, nbuttons), js(js) {};

    void SDLJoystick::Update() {
        int input_value;

        for (auto& input : inputs) {
            switch(input->type) {
                case HO_AXIS:
                    input_value = SDL_GetJoystickAxis(this->js,
                            input->src_input);
                    break;
                case HO_BUTTON:
                    input_value = SDL_GetJoystickButton(this->js,
                            input->src_input);
                    break;
                default:
                    input_value = -42;
                    break;
            }

            input->value = input_value;
        }
    };

    void SDLJoystick::SetInput(Input& input) {};

    int SDLJoystick::GetInput(int input) {
        return -42;
        //return inputs[input]->value;
    };
}
