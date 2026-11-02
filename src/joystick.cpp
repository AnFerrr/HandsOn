#include "../include/joystick.hpp"

namespace HandsOn {
    Joystick::Joystick(std::string name, short naxes, short nbuttons)
    : name(name), naxes(naxes), nbuttons(nbuttons) {};
}
