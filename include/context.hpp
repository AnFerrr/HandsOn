#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "../include/joystick.hpp"
#include "../include/sdl_joystick.hpp"
#include "../include/vjoystick.hpp"

namespace HandsOn {
    // TODO: Make vj_list and j_list a single list
    class Context {
        public:
            void SetWindow(SDL_Window* window) {
                this->window = window;
            };
            SDL_Window* GetWindow() { return window; };

            void SetGLContext(SDL_GLContext& gl_context) {
                this->gl_context = gl_context;
            };
            SDL_GLContext& GetGLContext() { return gl_context; };

            void AddJoystick(std::shared_ptr<Joystick> joystick);
            void AddVJoystick(std::shared_ptr<VJoystick> vjoystick);

            void RemoveJoystick(int joystick_index);
            void RemoveVJoystick(int vjoystick_index);

            int JSCount() { return js_list.size(); };
            int VJSCount() { return vjs_list.size(); };

            const std::vector<std::shared_ptr<Joystick>>& GetJSList() const {
                return js_list;
            };
            const std::vector<std::shared_ptr<Joystick>>& GetVJSList() const {
                return vjs_list;
            };

            std::vector<std::shared_ptr<Joystick>>& GetJSList() {
                return js_list;
            };
            std::vector<std::shared_ptr<Joystick>>& GetVJSList() {
                return vjs_list;
            };

            void Stop() { stopping = true; };

        private:
            SDL_Window* window;
            SDL_GLContext gl_context;

            std::vector<std::shared_ptr<Joystick>> js_list;
            std::vector<std::shared_ptr<Joystick>> vjs_list;

        public:
            bool stopping = false;
    };
}

#endif // CONTEXT_HPP
