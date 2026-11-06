#ifndef VJOYSTICK_HPP
#define VJOYSTICK_HPP

#include "joystick.hpp"
#include "libevdev-1.0/libevdev/libevdev.h"
#include <libevdev-1.0/libevdev/libevdev-uinput.h>

namespace HandsOn {
    class VJoystick : public Joystick {
        public:
            VJoystick(std::string name, short naxes, short nbuttons, int id);
            ~VJoystick();
            virtual void Update() override;
            int Initialize();
            virtual void SetInput(Input& input) override;
            int GetVID();

        private:
            struct libevdev* evdev;
            struct libevdev_uinput* uidev;
            int vid = 0;
    };
}

#endif // VJOYSTICK_HPP
