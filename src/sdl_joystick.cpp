#include "../include/sdl_joystick.hpp"
#include <stdexcept>

namespace HandsOn {
    SDLJoystick::SDLJoystick(std::string name, short naxes, short nbuttons,
            SDL_Joystick *js)
        : Joystick(name, naxes, nbuttons), js(js) {
            is_virtual = false;

            int total_input_count = 0;
            for (int i = 0; i < naxes; i++)
                this->inputs.emplace_back(nullptr, i, i, HO_AXIS);
            for (int i = 0; i < nbuttons; i++)
                this->inputs.emplace_back(nullptr, i, i + 0x100, HO_BUTTON);
        };

    void SDLJoystick::Update() {
        // TODO: Hat support
        for (auto& input : this->inputs) {
            switch (input.GetType()) {
                case HO_AXIS:
                    input.SetRawValue(SDL_GetJoystickAxis(this->js, input.GetSourceInput()));
                    break;
                case HO_BUTTON:
                    input.SetRawValue(SDL_GetJoystickButton(this->js, input.GetSourceInput()));
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
