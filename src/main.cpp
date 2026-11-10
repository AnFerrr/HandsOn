#include "SDL3/SDL.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_joystick.h"
#include <SDL3/SDL_stdinc.h>
#include <iostream>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <string>
#include "libevdev-1.0/libevdev/libevdev.h"
#include "../include/ui.hpp"
#include "../include/joystick.hpp"
#include "../include/context.hpp"

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
    HandsOn::UIContext ui_context;

    int res = init(context);
    if (res != 0)
        return res;

    do {
        auto& js_list = context.GetJSList();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                context.Stop();
            } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                    event.window.windowID == SDL_GetWindowID(context.GetWindow())) {
                context.Stop();
            } else if (event.type == SDL_EVENT_JOYSTICK_REMOVED) {
                for (int i = 0; i < context.JSCount(); i++) {
                    HandsOn::SDLJoystick* sdl_joystick =
                        (HandsOn::SDLJoystick*)js_list[i].get();
                    if (sdl_joystick->js &&
                            (SDL_GetJoystickID(sdl_joystick->js) == event.gdevice.which)) {
                        // TODO: This should be redundant
                        //SDL_CloseJoystick(sdl_joystick->js);
                        context.RemoveJoystick(i);
                    }
                }
            } else if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
                auto new_joystick = SDL_OpenJoystick(event.gdevice.which);
                std::string name = SDL_GetJoystickName(new_joystick);
                if (name.rfind("HandsOn Virtual joystick: ", 0) != 0) {
                    std::shared_ptr<HandsOn::Joystick> ho_joystick =
                        std::make_shared<HandsOn::SDLJoystick>(
                                name,
                                SDL_GetNumJoystickAxes(new_joystick),
                                SDL_GetNumJoystickButtons(new_joystick),
                                new_joystick
                                );

                    context.AddJoystick(ho_joystick);
                }
            }

        }
        imgui_loop(context, ui_context);

        for (auto& joystick : js_list) {
            joystick->Update();
            for (auto& input : joystick->inputs) {
                // TODO: Redo this
                input.ApplyFilters();
                if (input())
                    input.GetDestinationJoystick()->SetInput(input);
            }
        }

        auto& vjs_list = context.GetVJSList();
        for (auto& vjs : vjs_list)
            vjs->Update();

        SDL_Delay(1);
    } while (!context.stopping);

    // TODO: Check errors
    SDL_GL_DestroyContext(context.GetGLContext());
    SDL_Quit();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
