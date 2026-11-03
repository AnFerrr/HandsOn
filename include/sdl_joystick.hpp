#ifndef SDL_JOYSTICK_HPP
#define SDL_JOYSTICK_HPP

#include "joystick.hpp"
#include "SDL3/SDL.h"
#include "SDL3/SDL_joystick.h"
#include <SDL3/SDL_stdinc.h>

namespace HandsOn {
    struct SDLJoystick : public Joystick {
        SDLJoystick(std::string name,
                short naxes,
                short nbuttons,
                SDL_Joystick* js);
        virtual void Update() override;
        virtual int GetInput(int input) override;
        virtual void SetInput(Input& input) override;
        //virtual int GetButtonInput(int input) override;
        //virtual void SetButtonInput(Input& input) override;

        SDL_JoystickID js_id;
        SDL_Joystick* js;
    };
}

#endif // SDL_JOYSTICK_HPP
