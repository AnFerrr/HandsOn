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

void sdljoystick_ui_content(HandsOn::Context& context, HandsOn::Joystick& joystick) {
    HandsOn::SDLJoystick* js = (HandsOn::SDLJoystick*)&joystick;

    static auto& selected_input = joystick.axes[0];

    ImGui::BeginChild("Input list",
            ImVec2(ImGui::GetContentRegionAvail().x * 0.2f,
                ImGui::GetContentRegionAvail().y));

    int value = 0;
    std::string axis_button_name = "N/A";
    // Lists Axes
    for (int i = 0; i < joystick.axes.size(); i++) {
        bool active = false;
        auto& input = joystick.axes[i];

        value = input->value; 
        if (value > 300 || value < -300) {
            ImGui::PushStyleColor(ImGuiCol_Button,
                    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
            active = true;
        }

        axis_button_name = "Axis " + std::to_string(i);
        if (ImGui::Button(axis_button_name.c_str()))
            selected_input = input;

        if (active)
            ImGui::PopStyleColor();
    }

    std::string button_button_name = "N/A";
    // List buttons
    for (int i = 0; i < joystick.buttons.size(); i++) {
        auto& input = joystick.buttons[i];
        bool active = false;

        value = input->value;
        if (value == 1) {
            ImGui::PushStyleColor(ImGuiCol_Button,
                    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
            active = true;
        }

        button_button_name = "Button " + std::to_string(i);
        if (ImGui::Button(button_button_name.c_str()))
            selected_input = input;
        if (active)
            ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Input config");
    ImGui::Text("%i", selected_input->value);

    std::string combo_preview_value = context.vjs_list[0]->name;
    auto& vjs_list = context.vjs_list;
    int item_selected_idx = 0;
    if (ImGui::BeginCombo("Target selection", combo_preview_value.c_str())) {
        for (int n = 0; n < vjs_list.size(); n++) {
            const bool is_selected = (item_selected_idx == n);
            if (ImGui::Selectable(vjs_list[n]->name.c_str(), is_selected))
                item_selected_idx = n;
        }
        ImGui::EndCombo();
    }

    ImGui::EndChild();
}

void joystick_tab_list(HandsOn::Context& context) {
    int item_selected_idx = 0;
    auto& js_list = context.js_list;
    auto& vjs_list = context.vjs_list;
    bool opened[] = { true, true };

    if (ImGui::BeginTabBar("MyTabBar")) {
        for (auto& js : js_list) {
            if (ImGui::BeginTabItem(js->name.c_str())) {
                sdljoystick_ui_content(context, *js.get());
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

    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);

    bool opened = true;
    if (ImGui::Begin("Joystick List")) {
        joystick_tab_list(context);

        ImGui::End();
    }

    imgui_frame_end(context);

}
