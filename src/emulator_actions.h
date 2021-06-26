#pragma once

#include "emulator_state.h"
#include "gui/gui.h"

namespace ees::emulator
{

void update_bench(ees::gui::Application &app, ees::emulator::ApplicationState &state);
void refresh_serial_ports(ees::emulator::ApplicationState &state);
void open_serial_ports(ees::gui::Application &app);

}