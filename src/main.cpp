#include "SDL3/SDL.h"
#include "SDL3/SDL_joystick.h"
#include <SDL3/SDL_stdinc.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <string>
#include <unordered_map>
#include "libevdev-1.0/libevdev/libevdev.h"
#include "../include/joystick.hpp"
#include "../include/context.hpp"

SDL_Joystick* select_joystick() {
    bool js_valid = false;
    while (!js_valid) {
        std::cout << "Joystick list: " << std::endl;

        int count = 0;
        auto joysticks = SDL_GetJoysticks(&count);
        for (int i = 0; i < count; i++) {
            std::cout << " id: " << joysticks[i]
                << " name: " << SDL_GetJoystickNameForID(joysticks[i])
                << std::endl;
        }

        std::cout << "Input joystick id: ";
        std::string user_input;
        //std::cin >> user_input;
        //int js_id = std::stoi(user_input);
        int js_id = 5;

        SDL_JoystickID id;
        for (int i = 0; i < count; i++) {
            id = joysticks[i];
            if (id == js_id) {
                SDL_free(joysticks);
                return SDL_OpenJoystick(js_id);
                break;
            }
        }

        if (!js_valid)
            std::cout << "\n\n"
                << js_id << " is not a valid joystick id" << std::endl;

        SDL_free(joysticks);
    }

    return nullptr;
}

int sdl_init() {
    if (!SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO)) {
        std::cerr << "SDL3 couldn't initialize." << std::endl;
        return 1;
    }

    if (!SDL_HasJoystick()) {
        std::cerr << "No joystick detected" << std::endl;
        return 1;
    }

    return 0;
}

/* TODO: Rewrite to have a list of axes? To centralize modifications instead of
 * having to get axes from the joystick to edit them.
 *
 * Input and Output class?
 * List of I/O?
 * Just Inputs in and out?
 */
void setup_binds(HandsOn::Context& context) {
    context.js_selected->inputs[0] =
        std::make_shared<HandsOn::Input>(context.vjs, 0);
    context.js_selected->inputs[1] =
        std::make_shared<HandsOn::Input>(context.vjs, 1);
    context.js_selected->inputs[2] =
        std::make_shared<HandsOn::Input>(context.vjs, 2);
    context.js_selected->inputs[3] =
        std::make_shared<HandsOn::Input>(context.vjs, 3);

    //context.vjs->BindButton(
    //        HandsOn::Button(context.js_selected, 0),
    //        0x100);
    //context.vjs->BindButton(
    //        HandsOn::Button(context.js_selected, 1),
    //        0x101);
    //context.vjs->BindButton(
    //        HandsOn::Button(context.js_selected, 2),
    //        0x102);
    //context.vjs->BindButton(
    //        HandsOn::Button(context.js_selected, 3),
    //        0x103);
}

int init(HandsOn::Context& context) {
    int res = sdl_init();
    if (res != 0)
        return res;

    return res;
}

int main() {
    HandsOn::Context context;

    int res = init(context);
    if (res != 0)
        return res;

    context.vjs = std::make_shared<HandsOn::VJoystick>("VJoystick 0", 4, 4);
    if (!context.vjs->Initialize())
        return 2;

    if (res != 0)
        return 2;

    context.js_selected.reset(new HandsOn::SDLJoystick(select_joystick()));

    setup_binds(context);

    Sint16 bound_axis;
    bool button = false;
    do { 
        SDL_UpdateJoysticks();

        context.js_selected->Update();
        for (auto& input : context.js_selected->inputs) {
            input.second->js_dest->SetAxis(input.second->axis, input.second->value);
        }

        context.vjs->Update();
        button = SDL_GetJoystickButton(context.js_selected->js, 5);

        SDL_Delay(1);
    } while (!button);

    SDL_CloseJoystick(context.js_selected->js);
    SDL_Quit();

    return 0;
}
