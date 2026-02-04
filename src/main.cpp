#include "main.h"
#include "auton_select.hpp"
#include "autons.hpp"
#include "globals.hpp"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	create_auton_selector();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    // This looks a auton var (1-8) and runs the matching function
    switch (selected_auton) {
        case 1: left_qual_auton();   break;
        case 2: right_qual_auton();  break;
        case 3: skills_auton_for_qual(); break;
        case 4: right_qual_auton_center(); break;
        case 5: left_qual_auton_ram(); break;
        
        default: 
            // Fallback if something goes wrong
            printf("No auton selected!\n");
            break;
    }

}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

void opcontrol() {
    pros::Controller master(pros::E_CONTROLLER_MASTER);
    bool intake_active, intake_rev, lift_active, lift_rev; 
    intake_active = intake_rev = lift_active = lift_rev = false;

    while (true) {
        
        // --- SAFETY CHECK ---
        // If the debug run is active, SKIP all joystick controls
        if (is_debug_running) {
            pros::delay(20); // Wait and let the auton task work (ms)
            continue;        // Skip the rest of the loop
        }

        // Intake Logic
        if (master.get_digital_new_press(DIGITAL_UP)) {
            intake_rev = !intake_rev;
            if (intake_active) {
                if (!intake_rev) {
                    intake_motor.move_voltage(intake_volt);
                } else {
                    intake_motor.move_voltage(-intake_volt);
                }
            }
        }

        if (master.get_digital_new_press(DIGITAL_R1)) {
            intake_active = !intake_active;
        }

        if (intake_active) {
            intake_motor.move_voltage(intake_volt);
        } else {
            intake_motor.move_voltage(0);
        }

        // Lifting

        if (master.get_digital_new_press(DIGITAL_RIGHT)) {
            lift_rev = !lift_rev;
            if (lift_active) {
                if (!lift_rev) {
                    lift_motor.move_voltage(intake_volt);
                } else {
                    lift_motor.move_voltage(-intake_volt);
                }
            }
        }

        if (master.get_digital_new_press(DIGITAL_L1)) {
            lift_active = !lift_active;
        }

        if (lift_active) {
            if (!lift_rev) {
                lift_motor.move_voltage(lift_volt);
            } else {
                lift_motor.move_voltage(-lift_volt);
            }
        } else {
            lift_motor.move_voltage(0);
        }

        // TODO - implement slow motor logic


        // Drive Logic
        int dir = master.get_analog(ANALOG_LEFT_Y);    
        int turn = master.get_analog(ANALOG_RIGHT_X);  
        left_mg.move(dir - turn);                      
        right_mg.move(dir + turn);                     
        
        pros::delay(20);                               
    }
}