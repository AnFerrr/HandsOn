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

int create_joystick_list(HandsOn::Context& context) {
    if (!SDL_HasJoystick()) {
        std::cerr << "No joystick detected" << std::endl;
        return -1;
    }

    int count = 0;
    auto joysticks = SDL_GetJoysticks(&count);
    for (int i = 0; i < count; i++) {
        SDL_Joystick* joystick = SDL_OpenJoystick(joysticks[i]);
        std::string name = SDL_GetJoystickName(joystick);
        auto ho_joystick = std::make_shared<HandsOn::SDLJoystick>(
                name,
                SDL_GetNumJoystickAxes(joystick),
                SDL_GetNumJoystickButtons(joystick),
                joystick
                );

        context.js_list.push_back(ho_joystick);
    }

    SDL_free(joysticks);

    return 0;
}

int sdl_init() {
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (!SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO)) {
        std::cerr << "SDL3 couldn't initialize." << std::endl;
        return 1;
    }

    return 0;
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

    context.vjs_list.push_back(std::make_shared<HandsOn::VJoystick>("VJoystick 0", 4, 4));
    if (((HandsOn::VJoystick*)context.vjs_list[0].get())->Initialize() != 0)
        return 2;
    context.vjs_list.push_back(std::make_shared<HandsOn::VJoystick>("VJoystick 1", 4, 4));
    if (((HandsOn::VJoystick*)context.vjs_list[1].get())->Initialize() != 0)
        return 2;

    if (res != 0)
        return 2;

    bool joystick_close = false;

    auto vjs = context.vjs_list[0];

    do {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                context.stopping = true;
            } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                    event.window.windowID == SDL_GetWindowID(context.window)) {
                context.stopping = true;
            } else if (event.type == SDL_EVENT_JOYSTICK_REMOVED) {
                for (int i = 0; i < context.js_list.size(); i++) {
                    HandsOn::SDLJoystick* sdl_joystick =
                        (HandsOn::SDLJoystick*)context.js_list[i].get();
                    if (sdl_joystick->js &&
                            (SDL_GetJoystickID(sdl_joystick->js) == event.gdevice.which)) {
                        SDL_CloseJoystick(sdl_joystick->js);
                        context.js_list.erase(context.js_list.begin() + i);
                    }
                }
            }else if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
                auto new_joystick = SDL_OpenJoystick(event.gdevice.which);
                std::string name = SDL_GetJoystickName(new_joystick);
                auto ho_joystick = std::make_shared<HandsOn::SDLJoystick>(
                        name,
                        SDL_GetNumJoystickAxes(new_joystick),
                        SDL_GetNumJoystickButtons(new_joystick),
                        new_joystick
                        );

                context.js_list.push_back(ho_joystick);
                context.js_list[0]->inputs[0]->filters.push_back([](HandsOn::Input& input){
                        input.value = input.value * -1;
                        });
            }

        }
        imgui_loop(context);

        for (auto& joystick : context.js_list) {
            joystick->Update();
            for (auto& input : joystick->inputs) {
                for (auto& filter : input->filters)
                    filter(*input);
                if (input->js_dest)
                    input->js_dest->SetInput(*input);
            }
        }

        vjs->Update();

        SDL_Delay(1);
    } while (!context.stopping && !joystick_close);

    SDL_Quit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
