#ifndef UI_HPP
#define UI_HPP

#include "context.hpp"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

int init_imgui(HandsOn::Context& context);
void imgui_frame_init();
void imgui_frame_end(HandsOn::Context& context);
void imgui_loop(HandsOn::Context& context);

#endif // UI_HPP
