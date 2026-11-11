#include "../include/joystick.hpp"

namespace HandsOn {
    Joystick::Joystick(std::string name, short naxes, short nbuttons, bool is_virtual)
    : name(name), naxes(naxes), nbuttons(nbuttons), is_virtual(is_virtual) {};
}
