#include "main.h"
#include "globals.hpp"

void left_qual_auton() {
    // Example left qualification auton code
    left_mg.move_relative(1000, 100);  // Move left motors forward 1000 ticks at speed 100
    right_mg.move_relative(1000, 100); // Move right motors forward 1000 ticks at speed 100
    pros::delay(2000);                 // Wait for 2 seconds
    intake_motor.move_velocity(200);   // Start intake motor at velocity 200
    pros::delay(1000);                 // Run intake for 1 second
    intake_motor.move_velocity(0);     // Stop intake motor

}