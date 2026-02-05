#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "main.h"

// 'extern' tells the compiler these are defined in another file
extern pros::MotorGroup left_mg;
extern pros::MotorGroup right_mg;
extern pros::Motor intake_motor;
extern pros::Motor lift_motor;

extern int intake_volt;
extern int lift_volt;

extern float slow_mult;

#endif