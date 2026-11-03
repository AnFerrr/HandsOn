#include "../include/vjoystick.hpp"
#include <iostream>

namespace HandsOn {
    VJoystick::VJoystick(std::string name,
            short naxes,
            short nbuttons) : Joystick(name, naxes, nbuttons) {};

    VJoystick::~VJoystick() {
        if (this->uidev) {
            libevdev_uinput_destroy(this->uidev);
            this->uidev = nullptr;
        }
    };

    void VJoystick::SetAxisInput(Input& input) {
        libevdev_uinput_write_event(
                this->uidev,
                EV_ABS,
                input.dest_input,
                input.value);
    };
    int VJoystick::GetAxisInput(int input) {
        return 0;
    };

    void VJoystick::SetButtonInput(Input& input) {
        libevdev_uinput_write_event(
                this->uidev,
                EV_KEY,
                input.dest_input,
                input.value);
    };
    int VJoystick::GetButtonInput(int input) {
        return 0;
    };

    int VJoystick::Initialize() {
        int err;

        this->evdev = libevdev_new();
        if (!this->evdev)
            return -1;

        libevdev_set_name(this->evdev, this->name.c_str());

        libevdev_enable_event_type(this->evdev, EV_ABS);
        for (int i = 0; i < this->naxes ; i++) {
            struct input_absinfo absinfo = {0, -32768, 32768, 0, 0, 0};
            libevdev_enable_event_code(this->evdev, EV_ABS, i, &absinfo);
        }

        libevdev_enable_event_type(this->evdev, EV_KEY);
        for (int i = 0; i < this->nbuttons ; i++) {
            libevdev_enable_event_code(this->evdev, EV_KEY, 0x100 + i, nullptr);
        }

        libevdev_set_id_bustype(this->evdev, BUS_USB);
        libevdev_set_id_version(this->evdev, 0x0110);

        struct libevdev_uinput* uidev;
        err = libevdev_uinput_create_from_device(this->evdev,
                LIBEVDEV_UINPUT_OPEN_MANAGED,
                &uidev);
        if (err != 0) {
            std::cerr << "Failed to create uinput device (are you root / in the 'input' group?)\n";
            return -1;
        }
        std::cout << "Device live at: " << libevdev_uinput_get_devnode(uidev) << std::endl;

        this->uidev = uidev;
        libevdev_free(this->evdev);

        return 0;
    }

    void VJoystick::Update() {
        libevdev_uinput_write_event(this->uidev, EV_SYN, SYN_REPORT, 0);
    }
}
