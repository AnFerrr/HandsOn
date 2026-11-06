#include "../include/sdl_joystick.hpp"
#include <stdexcept>

namespace HandsOn {
    SDLJoystick::SDLJoystick(std::string name, short naxes, short nbuttons,
            SDL_Joystick *js)
        : Joystick(name, naxes, nbuttons), js(js) {
            is_virtual = false;

            int total_input_count = 0;
            for (int i = 0; i < naxes; i++)
                this->inputs.push_back(std::make_shared<HandsOn::Input>(nullptr, i, i, HO_AXIS));
            for (int i = 0; i < nbuttons; i++)
                this->inputs.push_back(std::make_shared<HandsOn::Input>(nullptr, i, i + 0x100, HO_BUTTON));
        };

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

    SDLJoystick::~SDLJoystick() {
        SDL_CloseJoystick(this->js);
    }

    void SDLJoystick::SetInput(Input &input) {};
} // namespace HandsOn
