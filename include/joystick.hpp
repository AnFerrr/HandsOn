#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include <memory>
#include <string>

#include "input.hpp"

namespace HandsOn {
    class Context;

    class Joystick {
        public:
            Joystick(std::string name, short naxes, short nbuttons, bool is_virtual);

            virtual void Update() = 0;
            // TODO: Rename/rework this
            virtual void SetInputValue(Input& input) = 0;

            const std::string& GetName() const {
                return this->name;
            };
            void SetName(std::string name) {
                this->name = name;
            };

            short GetAxesCount() const {
                return this->naxes;
            };
            // TODO: SetAxesCount(), for vjoystick editing purposes

            short GetButtonsCount() const {
                return this->nbuttons;
            };
            // TODO: SetButtonCount(), for vjoystick editing purposes

            const std::vector<Input>& GetInputs() const {
                return this->inputs;
            };
            // TODO: I'd like this to be removed
            std::vector<Input>& GetInputs() {
                return this->inputs;
            };
            short GetInputCount() const {
                return this->inputs.size();
            };
            const Input& GetInput(short index) const {
                return this->inputs[index];
            };
            Input& GetInput(short index) {
                return this->inputs[index];
            };

            bool IsVirtual() const {
                return this->is_virtual;
            };

        private:
            std::string name = "N/A";
            short naxes = 4;
            short nbuttons = 30;
            std::vector<Input> inputs;
            bool is_virtual = false;

    };
}

#endif // JOYSTICK_HPP
