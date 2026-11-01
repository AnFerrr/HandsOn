#ifndef SDL_JOYSTICK_HPP
#define SDL_JOYSTICK_HPP

#include "joystick.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_joystick.h"
#include <SDL3/SDL_stdinc.h>

namespace HandsOn {
    struct SDLJoystick : public Joystick {
        SDLJoystick(SDL_Joystick* joystick);
        virtual void Update() override;
        virtual void SetAxis(int axis, int value) override;
        virtual int GetAxis(int axis) override;

        SDL_JoystickID js_id;
        SDL_Joystick* js;
    };
}

#endif // SDL_JOYSTICK_HPP
