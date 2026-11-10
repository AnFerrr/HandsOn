#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <memory>
#include <string>

#include "input.hpp"

namespace HandsOn {
    class Context;
    struct Joystick {
        Joystick(std::string name, short naxes, short nbuttons);
        std::string name = "N/A";
        short naxes = 4;
        short nbuttons = 30;
        std::vector<Input> inputs;
        bool is_virtual = false;

        virtual void Update() = 0;
        virtual void SetInput(Input& input) = 0;
    };
}

#endif // JOYSTICK_HPP
