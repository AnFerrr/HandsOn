#include "../include/vjoystick.hpp"

namespace HandsOn {
    VJoystick::~VJoystick() {
        libevdev_uinput_destroy(this->uidev);
        libevdev_free(this->evdev);
    };

    void VJoystick::SetAxis(int axis, int value) {
        libevdev_uinput_write_event(this->uidev, EV_ABS, axis, value);
    };
    int VJoystick::GetAxis(int axis) {
        return 0;
    };
}
