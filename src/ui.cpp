#include "../include/ui.hpp"
#include "imgui.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL_opengl.h>
#include <algorithm>
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

void left_panel(
        HandsOn::Context& context,
        HandsOn::UIContext& ui_context,
        HandsOn::Joystick& joystick) {
    ImGui::BeginChild("Input list",
            ImVec2(ImGui::GetContentRegionAvail().x * 0.2f,
                ImGui::GetContentRegionAvail().y), ImGuiChildFlags_Borders);

    int value = 0;
    std::string button_name = "N/A";
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DrawLinesFull;
    if (ImGui::TreeNodeEx("Axes", base_flags)) {
        for (int i = 0; i < joystick.naxes; i++) {
            int push_count = 0;
            bool active = false;
            auto input = joystick.inputs[i];

            value = input->value;
            if (ui_context.selected_input == input) {
                push_count++;
                ImGui::PushStyleColor(ImGuiCol_Button,
                        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
            }
            if ((value > 300 || value < -300) && ui_context.listening) {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.33f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f));
                push_count += 2;
                active = true;
                ui_context.selected_input = input;
            }

            int index = i + 1;
            button_name = "Axis " + std::to_string(index);
            if (ImGui::Button(button_name.c_str()))
                ui_context.selected_input = input;
            ImGui::PopStyleColor(push_count);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("Buttons", base_flags)) {
        for (int i = joystick.naxes; i < joystick.inputs.size(); i++) {
            int push_count = 0;
            auto input = joystick.inputs[i];
            bool active = false;

            value = input->value;
            if (ui_context.selected_input == input) {
                push_count++;
                ImGui::PushStyleColor(ImGuiCol_Button,
                        ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
            }
            if (value == 1 && ui_context.listening) {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.33f, 1.0f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.0f, 0.0f, 0.0f));
                push_count += 2;
                active = true;
                ui_context.selected_input = input;
            }

            int index = i + 1 - joystick.naxes;
            button_name = "Button " + std::to_string(index);
            if (ImGui::Button(button_name.c_str()))
                ui_context.selected_input = input;
            ImGui::PopStyleColor(push_count);
        }
        ImGui::TreePop();
    }

    ImGui::EndChild();
}

void right_panel(HandsOn::Context& context, HandsOn::UIContext& ui_context) {
    static int item_selected_idx_input = 0;
    HandsOn::Input* selected_input = ui_context.selected_input.get();
    if (!selected_input)
        return;

    bool is_axis = selected_input->type;
    int input_id = selected_input->src_input;

    ImGui::BeginChild("Input config");
    ImGui::PushFont(NULL, 24.0f);
    ImGui::PopFont();
    ImGui::Text("Input value: %i", selected_input->value);

    auto& vjs_list = context.vjs_list;

    std::vector<std::string> name_list;
    name_list.push_back("None");
    for (auto entry : vjs_list)
        name_list.push_back(entry->name);
    int selected_vjs_id = 0;

    if (selected_input && selected_input->js_dest) {
        auto it = std::find_if(vjs_list.begin(), vjs_list.end(), [&](const auto& vjs_item) {
                return selected_input->js_dest->name == vjs_item->name;
                });

        if (it != vjs_list.end()) 
        {
            selected_vjs_id = std::distance(vjs_list.begin(), it) + 1;
        }
    }

    std::string combo_preview_value = name_list[selected_vjs_id];

    if (ImGui::BeginCombo("Target virtual joystick selection", combo_preview_value.c_str())) {
        for (int n = 0; n < name_list.size(); n++) {
            const bool is_selected = (selected_vjs_id == n);
            if (ImGui::Selectable(name_list[n].c_str(), is_selected)) {
                selected_vjs_id = n;
                item_selected_idx_input = 0;
                selected_input->js_dest = selected_vjs_id ?
                    vjs_list[selected_vjs_id - 1] : nullptr;
                selected_input->dest_input = -1;
                selected_input->dest_type = -1;
            }
        }
        ImGui::EndCombo();
    }

    if (selected_input && selected_input->js_dest) {
        int naxes = selected_input->js_dest->naxes;
        int nbuttons = selected_input->js_dest->nbuttons;
        int input_size = naxes + nbuttons;

        bool is_axis = false;

        name_list.clear();
        name_list.push_back("None");

        std::string name = "N/A";

        for (int i = 0; i < input_size; i++) {
            is_axis = i < naxes;
            int name_id = (is_axis) ? i : i - naxes;
            name = ((is_axis) ? "Axis " : "Button ") + std::to_string(name_id + 1);
            name_list.push_back(name);
        }
        combo_preview_value = name_list[item_selected_idx_input];

        if (ImGui::BeginCombo("Target virtual input selection", combo_preview_value.c_str())) {
            for (int n = 0; n < name_list.size(); n++) {
                is_axis = n < naxes;
                const bool is_selected = (item_selected_idx_input == n);
                if (ImGui::Selectable(name_list[n].c_str(), is_selected)) {
                    selected_input->dest_input = is_axis ? n - 1 : n - 1 - naxes;
                    selected_input->dest_type = is_axis ? HO_AXIS : HO_BUTTON;
                    item_selected_idx_input = n;
                }
            }
            ImGui::EndCombo();
        }
    }

    ImGui::EndChild();
}

// TODO: Fix crash when removing a bound vjoystick
void sdljoystick_ui_content(
        HandsOn::Context& context,
        HandsOn::UIContext& ui_context,
        HandsOn::Joystick& joystick) {
    HandsOn::SDLJoystick* js = (HandsOn::SDLJoystick*)&joystick;

    bool listen_pressed = false;
    std::string listen_button_name = "Listen to inputs";
    if (ui_context.listening) {
        ImGui::PushStyleColor(ImGuiCol_Button,
                ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    listen_button_name = "Listening to inputs...";
    }
    if (ImGui::Button(listen_button_name.c_str())) {
        ui_context.listening = !ui_context.listening;
        listen_pressed = true;
    }
    if (!listen_pressed && ui_context.listening
            || listen_pressed && !ui_context.listening)
        ImGui::PopStyleColor();

    left_panel(context, ui_context, *js);

    ImGui::SameLine();

    right_panel(context, ui_context);
}

void joystick_tab_list(HandsOn::Context& context, HandsOn::UIContext& ui_context) {
    int item_selected_idx = 0;
    auto& js_list = context.js_list;
    auto& vjs_list = context.vjs_list;
    bool opened[] = { true, true };

    if (ImGui::BeginTabBar("MyTabBar")) {
        for (auto js : js_list) {
            if (ImGui::BeginTabItem(js->name.c_str())) {
                sdljoystick_ui_content(context, ui_context, *js.get());
                ImGui::EndTabItem();
            }
        }

        for (int i = 0; i < vjs_list.size(); i++) {
            auto js = vjs_list[i];
            bool result = ImGui::BeginTabItem(js->name.c_str());
            if (ImGui::IsItemClicked(2))
                vjs_list.erase(vjs_list.begin() + i);
            if (result) {
                ImGui::Text("This is the %s tab!", js->name.c_str());
                ImGui::EndTabItem();
            }
        }

        if (ImGui::TabItemButton("+")) {
            int index = context.vjs_list.size();
            std::string vjoystick_name = "HandsOn Virtual joystick: " + std::to_string(index);

            vjs_list.push_back(std::move(std::make_shared<HandsOn::VJoystick>(vjoystick_name, 4, 4)));
            if (((HandsOn::VJoystick*)vjs_list[index].get())->Initialize() != 0)
                throw std::runtime_error("Couldn't create virtual Joystick.");
        }

        ImGui::EndTabBar();
    }
}

void imgui_loop(HandsOn::Context& context, HandsOn::UIContext& ui_context) {
    static bool show_demo_window = true;
    ImGuiIO io;

    io = ImGui::GetIO();

    imgui_frame_init();

    ImGui::DockSpaceOverViewport();

    bool opened = true;
    if (ImGui::Button("Test")) {
        context.js_list[0]->inputs[0]->js_dest = context.vjs_list[0];
    }
    if (ImGui::Begin("Joystick List")) {
        joystick_tab_list(context, ui_context);

        ImGui::End();
    }

    imgui_frame_end(context);
}
