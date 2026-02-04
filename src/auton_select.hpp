#ifndef AUTON_SELECT_HPP
#define AUTON_SELECT_HPP

#include "liblvgl/lvgl.h"

// "extern" tells the compiler that this variable exists somewhere else (the .cpp)
extern int selected_auton;

// Declaration of the UI function
void create_auton_selector();

extern bool is_debug_running;

#endif