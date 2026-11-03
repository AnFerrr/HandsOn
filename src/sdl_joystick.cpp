#include "../include/sdl_joystick.hpp"
#include <stdexcept>

namespace HandsOn {
    SDLJoystick::SDLJoystick(std::string name, short naxes, short nbuttons,
            SDL_Joystick *js)
        : Joystick(name, naxes, nbuttons), js(js) {};

    void SDLJoystick::Update() {
        int input_value;

        // TODO: Hat support
        for (auto& input : this->inputs) {
            switch (input->type) {
                case HO_AXIS:
                    input->value = SDL_GetJoystickAxis(this->js, input->src_input);
                    break;
                case HO_BUTTON:
                    input->value = SDL_GetJoystickButton(this->js, input->src_input);
                    break;
                default:
                    break;
            }
        }
    };

    void SDLJoystick::SetInput(Input &input) {};

    int SDLJoystick::GetInput(int input) {
        /*
        if (input < inputs.size())
            return inputs[input]->value;
        else
            throw(std::runtime_error("Selected axis doesn't exist."));
            */
        throw(std::runtime_error("Oops this has been called."));
    };
} // namespace HandsOn
