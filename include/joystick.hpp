#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <memory>
#include <string>

#include "input.hpp"

namespace HandsOn {
    struct Context;
    struct Joystick {
        Joystick(std::string name, short naxes, short nbuttons);
        std::string name = "N/A";
        short naxes = 4;
        short nbuttons = 30;
        /* TODO: make into a list of Input and have each input (buttons and
         * axes) be an Input?
         */
        std::vector<std::shared_ptr<Input>> inputs;

        virtual void Update() = 0;
        virtual void SetInput(Input& input) = 0;
        virtual int GetInput(int input) = 0;
    };
}

#endif // JOYSTICK_HPP
