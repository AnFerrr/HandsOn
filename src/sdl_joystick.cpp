#include "../include/sdl_joystick.hpp"
#include <stdexcept>

namespace HandsOn {
    SDLJoystick::SDLJoystick(std::string name, short naxes, short nbuttons,
            SDL_Joystick *js)
        : Joystick(name, naxes, nbuttons), js(js) {};

    void SDLJoystick::Update() {
        int input_value;

        for (auto& axis : this->axes)
            axis->value = SDL_GetJoystickAxis(this->js, axis->src_input);
        for (auto& button : this->buttons)
            button->value = SDL_GetJoystickButton(this->js, button->src_input);

    };

    void SDLJoystick::SetAxisInput(Input &input) {};

    int SDLJoystick::GetAxisInput(int input) {
        if (input < axes.size())
            return axes[input]->value;
        else
            throw(std::runtime_error("Selected axis doesn't exist."));
    };

    void SDLJoystick::SetButtonInput(Input &input) {};

    int SDLJoystick::GetButtonInput(int input) {
        if (input < axes.size())
            return buttons[input]->value;
        else
            throw(std::runtime_error("Selected axis doesn't exist."));
    };
} // namespace HandsOn
