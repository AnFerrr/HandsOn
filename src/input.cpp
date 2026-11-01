#include "../include/input.hpp"

namespace HandsOn {
    Input::Input(std::shared_ptr<Joystick> js_dest, int axis)
            : js_dest(js_dest), axis(axis) {};
}
