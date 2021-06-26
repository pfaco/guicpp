#include "emulator_actions.h"

namespace ees::emulator
{

void update_bench(ees::gui::Application &app, ees::emulator::ApplicationState &state)
{
    app.info("Bench updated to {{{}V, {}V, {}V, {}A, {}A, {}A, {}, {}, {}}}", 
        state.bench.phase_a.volt,  state.bench.phase_b.volt,  state.bench.phase_c.volt, 
        state.bench.phase_a.curr,  state.bench.phase_b.curr,  state.bench.phase_c.curr, 
        state.bench.phase_a.angle, state.bench.phase_b.angle, state.bench.phase_c.angle
    );
    ++state.display.counter;
    state.display.value[0] = 'H';
    state.display.value[1] = 'E';
    state.display.value[2] = 'L';
    state.display.value[3] = 'L';
    state.display.value[6] = ((state.display.counter / 10) % 10) + '0';
    state.display.value[7] = (state.display.counter % 10) + '0';
}

void refresh_serial_ports(ees::emulator::ApplicationState &state)
{
    state.serial_ports.port_list.clear();
    state.serial_ports.port_list.push_back("COM3");
    state.serial_ports.port_list.push_back("COM4");
    state.serial_ports.port_list.push_back("COM8");
    state.serial_ports.port_list.push_back("COM13");
    state.display.alarm ^= true;
}

void open_serial_ports(ees::gui::Application &app)
{
    app.info("Ports open");
}

}
