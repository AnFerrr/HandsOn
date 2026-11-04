#include "../include/input.hpp"

namespace HandsOn {
    Input::Input(std::shared_ptr<Joystick> js_dest,
            int src_input,
            int dest_input,
            int type)
            : js_dest(js_dest), src_input(src_input),
            dest_input(dest_input), type(type), dest_type(type) {};
}
