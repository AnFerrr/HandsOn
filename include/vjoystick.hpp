#ifndef VJOYSTICK_HPP
#define VJOYSTICK_HPP

#include "joystick.hpp"
#include "libevdev-1.0/libevdev/libevdev.h"
#include <libevdev-1.0/libevdev/libevdev-uinput.h>

namespace HandsOn {
    class VJoystick : public Joystick {
        public:
            VJoystick(std::string name, short naxes, short nbuttons);
            ~VJoystick();
            virtual void Update() override;
            int Initialize();
            virtual void SetInput(Input& input) override;
            virtual int GetInput(int input) override;

        private:
            struct libevdev* evdev;
            struct libevdev_uinput* uidev;
    };
}

#endif // VJOYSTICK_HPP
