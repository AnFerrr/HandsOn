#ifndef INPUT_HPP
#define INPUT_HPP

#include <functional>
#include <memory>
#include <vector>
#include <string>
#include "libevdev-1.0/libevdev/libevdev.h"
#include <libevdev-1.0/libevdev/libevdev-uinput.h>

namespace HandsOn {
    struct Joystick;

    struct Input {
        Input(std::shared_ptr<Joystick> js_dest,
                int src_input,
                int dest_input);

        std::vector<std::function<void(Input& input)>> filters;
        std::shared_ptr<Joystick> js_dest;
        int src_input;
        int dest_input;
        int value = 0;
    };
}

#endif // INPUT_HPP
