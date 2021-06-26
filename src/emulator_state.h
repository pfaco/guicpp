#pragma once

#include <array>
#include <vector>
#include <string>

namespace ees::emulator
{

struct BenchPhaseState
{
    double volt = 120.0;
    double curr = 15.0;
    double angle = 0.0;
};

struct BenchState
{
    BenchPhaseState phase_a;
    BenchPhaseState phase_b;
    BenchPhaseState phase_c;
    double angle_va_vb = 120.0;
    double angle_va_vc = 240.0;
    double freq = 60.0;
    double bat_volt = 3.6;
    double temperature = 25.0;
};

struct ButtonsState
{
    bool is_pressed_display;
    bool is_pressed_demand;
    bool is_pressed_terminal_cover;
    bool is_pressed_nic_cover;
    bool is_pressed_s1;
};

struct SingleSerialPortState
{
    const char * selected_value = "";
    bool is_open;
};

struct SerialPortState
{
    SingleSerialPortState optical;
    SingleSerialPortState rs485;
    SingleSerialPortState uo;
    SingleSerialPortState nic;
    SingleSerialPortState bench;
    std::vector<std::string> port_list;
};

struct DisplayState
{
    int counter = 0;
    char value[9] = "00000000";
    bool low_battery = true;
    bool relay_close = true;
    bool relay_open = true;
    bool communication = true;
    bool billing = true;
    bool alarm = true;
};

struct EepromState
{
    std::array<uint8_t, 4*1024> bytes;
};

struct DflashState
{
    int current_sector = 0;
    std::vector<std::vector<uint8_t>> bytes{512};
};

struct SingleLedState
{
    bool is_on = true;
    bool is_off = false;
};

struct LedState
{
    SingleLedState kwh;
    SingleLedState kvarh;
    SingleLedState aux;
};

struct ApplicationState
{
    BenchState bench;
    ButtonsState buttons;
    SerialPortState serial_ports;
    DisplayState display;
    EepromState eeprom;
    DflashState dflash;
    LedState led;
};

}
