#include "../include/ui.hpp"
#include "imgui.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL_opengl.h>
#include <iostream>
#include <string>

int make_window(SDL_Window*& window, SDL_GLContext& gl_context) {
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
 
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("Joystick Monitor", 1902, 1080, window_flags);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return -1;
    }
 
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0);
    
    return 0;
}

int init_imgui(HandsOn::Context& context) {
    SDL_Window* window;
    SDL_GLContext gl_context;
    if (make_window(window, gl_context) != 0) {
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    if (!ImGui_ImplOpenGL3_Init()) {
        std::cerr << "OpenGL3 Initialization failed." << std::endl;
        return -1;
    }

    context.gl_context = gl_context;
    context.window = window;

    return 0;
}

void imgui_frame_init() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void imgui_frame_end(HandsOn::Context& context) {
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(context.window);
}

void imgui_loop(HandsOn::Context& context) {
    static bool show_demo_window = true;
    ImGuiIO io;

    io = ImGui::GetIO();
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            context.stopping = true;
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(context.window))
            context.stopping = true;
    }

    imgui_frame_init();

    ImGui::DockSpaceOverViewport();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    int item_selected_idx = 0;
    auto& js_list = context.js_list;
    auto& vjs_list = context.vjs_list;
    bool opened[] = { true, true };
    std::string button_button_name = "N/A";
    if (ImGui::Begin("Joystick List")) {
        if (ImGui::BeginTabBar("MyTabBar")) {
            for (auto& js : js_list) {
                if (ImGui::BeginTabItem(js->name.c_str())) {
                    for (int i = 0; i < js->nbuttons; i++) {
                        auto& sdl_js = ((HandsOn::SDLJoystick*)js.get())->js;
                        bool pressed = SDL_GetJoystickButton(sdl_js, i); 
                        if (pressed)
                            ImGui::PushStyleColor(ImGuiCol_Button,
                                    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
                        button_button_name = "Button " + std::to_string(i);
                        ImGui::Button(button_button_name.c_str());
                        if (pressed)
                            ImGui::PopStyleColor();
                    }
                    ImGui::EndTabItem();
                }
            }

            for (auto& js : vjs_list) {
                if (ImGui::BeginTabItem(js->name.c_str())) {
                    ImGui::Text("This is the %s tab!", js->name.c_str());
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    imgui_frame_end(context);

}
