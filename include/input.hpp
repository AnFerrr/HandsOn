#ifndef INPUT_HPP
#define INPUT_HPP

#include <functional>
#include <memory>
#include <vector>
#include <string>
#include "libevdev-1.0/libevdev/libevdev.h"
#include <libevdev-1.0/libevdev/libevdev-uinput.h>

#define HO_AXIS EV_ABS
#define HO_BUTTON EV_KEY

namespace HandsOn {
    struct Joystick;

    // TODO: At some point, add lua filters/scripts :D
    struct Input {
        public:
            Input(std::shared_ptr<Joystick> js_dest,
                    int src_input,
                    int dest_input,
                    int type);

            void ApplyFilters() {
                this->value = this->raw_value;
                for (auto& filter : filters)
                    filter(raw_value);
            };

            void Clear() {
                this->RemoveDestinationJoystick();
                this->RemoveDestinationInput();
                this->RemoveDestinationType();
                this->RemoveType();
                this->value = -42;
                this->raw_value = -42;
                this->ClearFilters();
            };

            int GetSourceInput() const {
                return src_input;
            };

            int GetDestinationInput() const {
                return dest_input;
            };
            void SetDestinationInput(int dest_input) {
                this->dest_input = dest_input;
            };
            void RemoveDestinationInput() {
                this->dest_input = -1;
            }

            // TODO: Apply filter once per loop
            float GetValue() const {
                return this->value;
            };

            // TODO: corrective pass to set values everywhere to be float
            void SetRawValue(float value) {
                this->raw_value = value;
            };
            float GetRawValue() const {
                return this->raw_value;
            };

            void SetType(int type) {
                this->type = type;
            };
            int GetType() const {
                return this->type;
            };
            void RemoveType() {
                this->type = -1;
            };

            int GetDestinationType() const {
                return this->dest_type;
            };
            void SetDestinationType(int dest_type) {
                this->dest_type = dest_type;
            };
            void RemoveDestinationType() {
                this->dest_type = -1;
            };

            void AddFilter(std::function<void(float value)> filter) {
                this->filters.push_back(filter);
            };
            void RemoveFilter(int filter_index) {
                this->filters.erase(filters.begin() + filter_index);
            };
            const std::vector<std::function<void(float& value)>>& GetFilters() const {
                return this->filters;
            };
            void ClearFilters() {
                this->filters.clear();
            };

            void SetDestinationJoystick(std::shared_ptr<Joystick> js_dest) {
                this->js_dest = js_dest;
            };
            std::shared_ptr<Joystick> GetDestinationJoystick() const {
                return this->js_dest;
            };
            void RemoveDestinationJoystick() {
                this->js_dest = nullptr;
            };

            bool operator()() const {
                return this->js_dest != nullptr;
            };

        private:
            std::shared_ptr<Joystick> js_dest;
            // TODO: Make filters objects?
            std::vector<std::function<void(float& value)>> filters;
            int src_input;
            int dest_input;
            float raw_value = -42;
            float value = -42;
            int type = -1;
            int dest_type = -1;
    };
}

#endif // INPUT_HPP
