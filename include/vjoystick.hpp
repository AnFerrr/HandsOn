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
            virtual void SetAxisInput(Input& input) override;
            virtual int GetAxisInput(int input) override;
            virtual void SetButtonInput(Input& input) override;
            virtual int GetButtonInput(int input) override;

        private:
            struct libevdev* evdev;
            struct libevdev_uinput* uidev;
    };
}

#endif // VJOYSTICK_HPP
