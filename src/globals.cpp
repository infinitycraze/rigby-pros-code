#include "main.h"

//Define Devices here
/*Comp Bot devices
pros::MotorGroup left_mg({-1, -2, -3});  
pros::MotorGroup right_mg({4, 5, 6});   
pros::Motor intake_motor(7);
*/
pros::MotorGroup left_mg({1, 12});
pros::MotorGroup right_mg({-10, -20});
pros::Motor intake_motor({13});
pros::Motor lift_motor({19});

int intake_volt = 12000;
int lift_volt = 8000;

float slow_mult = 0.5f;
