#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_joystick.h"
#include <SDL3/SDL_stdinc.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <string>
#include <unordered_map>
#include "libevdev-1.0/libevdev/libevdev.h"
#include "../include/ui.hpp"
#include "../include/joystick.hpp"
#include "../include/context.hpp"

int update_joystick_list(HandsOn::Context& context) {
    if (!SDL_HasJoystick()) {
        std::cerr << "No joystick detected" << std::endl;
        return -1;
    }

    int count = 0;
    auto joysticks = SDL_GetJoysticks(&count);
    for (int i = 0; i < count; i++) {
        SDL_Joystick* joystick = SDL_OpenJoystick(joysticks[i]);
        std::string name = SDL_GetJoystickName(joystick);
        context.js_list.push_back(std::make_shared<HandsOn::SDLJoystick>(
                name,
                SDL_GetNumJoystickAxes(joystick),
                SDL_GetNumJoystickButtons(joystick),
                joystick
                ));
    }

    SDL_free(joysticks);

    return 0;
}

SDL_Joystick* select_joystick() {
    if (!SDL_HasJoystick()) {
        std::cerr << "No joystick detected" << std::endl;
        return nullptr;
    }

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
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (!SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO)) {
        std::cerr << "SDL3 couldn't initialize." << std::endl;
        return 1;
    }

    return 0;
}

void setup_binds(HandsOn::Context& context) {
    auto js_selected = context.js_list[0];
    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 0, 0, HO_AXIS));
    js_selected->inputs[0]->filters.push_back([](HandsOn::Input& input){
        input.value = input.value * -1;
    });
    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 1, 1, HO_AXIS));
    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 2, 2, HO_AXIS));
    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 3, 3, HO_AXIS));

    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 0, 0x100, HO_BUTTON));
    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 1, 0x101, HO_BUTTON));
    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 2, 0x102, HO_BUTTON));
    js_selected->inputs.push_back(
        std::make_shared<HandsOn::Input>(context.vjs, 3, 0x103, HO_BUTTON));
}

int init(HandsOn::Context& context) {
    int res = sdl_init();
    if (res != 0)
        return res;

    res = init_imgui(context);
    if (res != 0)
        return res;

    return res;
}

int main() {
    HandsOn::Context context;

    int res = init(context);
    if (res != 0)
        return res;

    update_joystick_list(context);

    context.vjs = std::make_shared<HandsOn::VJoystick>("VJoystick 0", 4, 4);
    if (context.vjs->Initialize() != 0)
        return 2;

    if (res != 0)
        return 2;

    setup_binds(context);

    bool joystick_close = false;

    auto selected = context.js_list[0];

    do {
        imgui_loop(context);

        context.js_list[0]->Update();
        for (auto& input : selected->inputs) {
            for (auto& filter : input->filters)
                filter(*input);
            input->js_dest->SetInput(*input);
        }

        context.vjs->Update();
        joystick_close = SDL_GetJoystickButton(((HandsOn::SDLJoystick*)selected.get())->js, 5);

        SDL_Delay(1);
    } while (!context.stopping && !joystick_close);

    SDL_CloseJoystick(((HandsOn::SDLJoystick*)selected.get())->js);
    SDL_Quit();

    return 0;
}
