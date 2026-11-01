#ifndef INPUT_HPP
#define INPUT_HPP

#include <memory>
#include <vector>

namespace HandsOn {
    struct Joystick;

    struct Input {
        Input(std::shared_ptr<Joystick> js_dest, int axis)
            : js_dest(js_dest), axis(axis) {};

        std::vector<void (*)()> filters;
        std::shared_ptr<Joystick> js_dest;
        int axis;
        int value = 0;
    };
}

#endif // INPUT_HPP
