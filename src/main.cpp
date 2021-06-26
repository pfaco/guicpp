#include <thread>
#include "gui/gui.h"
#include "resources/imgs.h"
#include "resources/fonts.h"
#include "emulator_actions.h"

#define RESOURCE(X) X, X ##_size

int main(int argc, char *argv[])
{
    using namespace ees;
    ees::emulator::ApplicationState state;
    gui::FontConfig font_display_value = {fonts_digital7, fonts_digital7_size, 50.0, 7.0};

    gui::Application app(gui::Size{1400, 850}, "EletraGui", gui::Size{790, 300}, gui::Position{5, 545});

    app.init(fonts_Roboto_Medium, fonts_Roboto_Medium_size, 14.0);

    app.add(gui::MemoryEditorWindow("EEPROM", state.eeprom.bytes.data(), state.eeprom.bytes.size(), gui::Size{580,800}, gui::Position{800, 35}));
    app.add(gui::SectorMemoryEditorWindow("DFLASH", state.dflash.bytes, state.dflash.current_sector, gui::Size{580,800}, gui::Position{835, 5}));

    app.add(gui::Window("Control")
        .add(gui::InputInteger("DFLASH Sector", state.dflash.current_sector))
        .add(gui::OnClickButton("Save State", [](){}))
        .add(gui::FrameRateLabel()));
    
    app.add(gui::Window("Buttons", gui::Size{100,120}, gui::Position{5, 5})
        .add(gui::HBox()
            .add(gui::StateButton("Display", state.buttons.is_pressed_display))
            .add(gui::StateButton("Demand",  state.buttons.is_pressed_demand))));

    app.add(gui::Window("Input", gui::Size{150,120}, gui::Position{110, 5})
        .add(gui::CheckBox("Main Cover",     state.buttons.is_pressed_s1))
        .add(gui::CheckBox("NIC Cover",      state.buttons.is_pressed_nic_cover))
        .add(gui::CheckBox("Terminal Cover", state.buttons.is_pressed_terminal_cover))
        .add(gui::CheckBox("Relay Voltage",  state.buttons.is_pressed_terminal_cover)));

    app.add(gui::Window("LEDs", gui::Size{250,300}, gui::Position{270, 240})
        .add(gui::HBox()
            .add(gui::Stack()
                .add(gui::ImageResource(img_led_on,  img_led_on_size,  &state.led.kwh.is_on))
                .add(gui::ImageResource(img_led_off, img_led_off_size, &state.led.kwh.is_off)))
            .add(gui::Stack()
                .add(gui::ImageResource(img_led_on,  img_led_on_size,  &state.led.kvarh.is_on))
                .add(gui::ImageResource(img_led_off, img_led_off_size, &state.led.kvarh.is_off)))
            .add(gui::Stack()
                .add(gui::ImageResource(img_led_on,  img_led_on_size,  &state.led.aux.is_on))
                .add(gui::ImageResource(img_led_off, img_led_off_size, &state.led.aux.is_off))))
            .add(gui::TextField("kwh",   gui::Position{-82, 40}))
            .add(gui::TextField("kvarh", gui::Position{47, -18}))
            .add(gui::TextField("aux",   gui::Position{95, -18})));

    app.add(gui::Window("Serial Ports", gui::Size{250,300}, gui::Position{270, 240})
        .add(gui::ComboBoxString("Optical Port", state.serial_ports.port_list, state.serial_ports.optical.selected_value))
        .add(gui::ComboBoxString("RS-485 Port",  state.serial_ports.port_list, state.serial_ports.rs485.selected_value))
        .add(gui::ComboBoxString("NIC Port",     state.serial_ports.port_list, state.serial_ports.nic.selected_value))
        .add(gui::ComboBoxString("UO Port",      state.serial_ports.port_list, state.serial_ports.uo.selected_value))
        .add(gui::ComboBoxString("Bench Port",   state.serial_ports.port_list, state.serial_ports.bench.selected_value))
        .add(gui::HBox()
            .add(gui::OnClickButton("Refresh", [&](){ emulator::refresh_serial_ports(state); }))
            .add(gui::OnClickButton("Open Ports", [&](){ emulator::open_serial_ports(app); })))
        .add(gui::CheckBox("Optical port open", state.serial_ports.optical.is_open))
        .add(gui::CheckBox("RS-485 port open", state.serial_ports.rs485.is_open))
        .add(gui::CheckBox("NIC port open", state.serial_ports.rs485.is_open))
        .add(gui::CheckBox("UO port open", state.serial_ports.rs485.is_open))
        .add(gui::CheckBox("Bench port open", state.serial_ports.rs485.is_open)));

    app.add(gui::Window("Bench", gui::Size{260,410}, gui::Position{5, 130})
        .add(gui::TabBar()
            .add(gui::TabItem("Input")
                .add(gui::InputDouble("Voltage A",   state.bench.phase_a.volt))
                .add(gui::InputDouble("Voltage B",   state.bench.phase_b.volt))
                .add(gui::InputDouble("Voltage C",   state.bench.phase_c.volt))
                .add(gui::InputDouble("Current A",   state.bench.phase_a.curr))
                .add(gui::InputDouble("Current B",   state.bench.phase_b.curr))
                .add(gui::InputDouble("Current C",   state.bench.phase_c.curr))
                .add(gui::InputDouble("Angle A",     state.bench.phase_a.angle))
                .add(gui::InputDouble("Angle B",     state.bench.phase_b.angle))
                .add(gui::InputDouble("Angle C",     state.bench.phase_c.angle))
                .add(gui::InputDouble("Angle VaVb",  state.bench.angle_va_vb))
                .add(gui::InputDouble("Angle VaVc",  state.bench.angle_va_vc))
                .add(gui::InputDouble("Frequency",   state.bench.freq))
                .add(gui::InputDouble("Battery",     state.bench.bat_volt))
                .add(gui::InputDouble("Temperature", state.bench.temperature))
                .add(gui::OnClickButton("Update",    [&](){ emulator::update_bench(app, state); })))
            .add(gui::TabItem("State"))));

    app.add(gui::Window("Display", gui::Size{520,220}, gui::Position{270, 15})
        .add(gui::Stack()
            .add(gui::ImageResource(RESOURCE(img_display_alarm), &state.display.alarm))
            .add(gui::ImageResource(RESOURCE(img_display_alt1)))
            .add(gui::ImageResource(RESOURCE(img_display_alt2)))
            .add(gui::ImageResource(RESOURCE(img_display_billing), &state.display.billing))
            .add(gui::ImageResource(RESOURCE(img_display_communication), &state.display.communication))
            .add(gui::ImageResource(RESOURCE(img_display_relay_close), &state.display.relay_close))
            .add(gui::ImageResource(RESOURCE(img_display_relay_open), &state.display.relay_open))
            .add(gui::ImageResource(RESOURCE(img_display_low_battery), &state.display.low_battery))
            .add(gui::FontText(font_display_value, state.display.value, gui::Position{100, 85}))
        ));
    
    app.run();

    return 0;
}
