#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include "SDL3/SDL.h"
#include "SDL3/SDL_joystick.h"
#include <SDL3/SDL_stdinc.h>
#include <iostream>
#include <memory>
#include <vector>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <string>
#include <unordered_map>
#include "input.hpp"
#include "libevdev-1.0/libevdev/libevdev.h"

namespace HandsOn {
    struct Context;
    struct Joystick {
        std::string name = "N/A";
        short naxes = 4;
        short nbuttons = 30;
        std::unordered_map<int, std::shared_ptr<Input>> inputs;

        virtual void Update() = 0;
        virtual void SetAxis(int axis, int value) = 0;
        virtual int GetAxis(int axis) = 0;
    };

    struct SDLJoystick : public Joystick {
        SDLJoystick(SDL_Joystick* joystick);
        virtual void Update() override {
            int axis;

            for (auto& input : inputs) {
                axis = SDL_GetJoystickAxis(this->js, input.first);
                input.second->value = axis;
            }
        };
        virtual void SetAxis(int axis, int value) override {};
        virtual int GetAxis(int axis) override { return inputs[axis]->value; };

        SDL_JoystickID js_id;
        SDL_Joystick* js;
    };

    class VJoystick : public Joystick {
        public:
            VJoystick(std::string name, short naxes, short nbuttons);
            virtual void Update() override;
            int Initialize();
            ~VJoystick() {
                libevdev_uinput_destroy(this->uidev);
                libevdev_free(this->evdev);
            };
            virtual void SetAxis(int axis, int value) override {
                libevdev_uinput_write_event(this->uidev, EV_ABS, axis, value);
            };
            virtual int GetAxis(int axis) override { return 0; };

        private:
            struct libevdev* evdev;
            struct libevdev_uinput* uidev;
    };
}

#endif // JOYSTICK_HPP
