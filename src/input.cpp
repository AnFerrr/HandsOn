#include "../include/input.hpp"

namespace HandsOn {
    Input::Input(std::shared_ptr<Joystick> js_dest,
            int src_input,
            int dest_input)
            : js_dest(js_dest), src_input(src_input),
            dest_input(dest_input) {};
}
