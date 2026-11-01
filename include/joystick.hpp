#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "input.hpp"

namespace HandsOn {
    struct Context;
    struct Joystick {
        std::string name = "N/A";
        short naxes = 4;
        short nbuttons = 30;
        std::unordered_map<int, std::shared_ptr<Input>> inputs;

        virtual void Update() = 0;
        virtual void SetAxis(int axis, int value) = 0;
        virtual int GetAxis(int axis) = 0;
    };
}

#endif // JOYSTICK_HPP
