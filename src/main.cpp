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
    // Print a simple Hello World message on startup
    printf("Hello World!\n");
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

    // Everything is off because the bot just started
    bool intake_active, intake_rev, lift_active, lift_rev; 
    intake_active = intake_rev = lift_active = lift_rev = false;

    float intake_slow = 1.0f; 
    float lift_slow = 1.0f;


    while (true) {
        
        // --- SAFETY CHECK ---
        // If the debug run is active, SKIP all joystick controls
        if (is_debug_running) {
            pros::delay(20); // Wait and let the auton task work (ms)
            continue;        // Skip the rest of the loop
        }
        
        
        if (master.get_digital_new_press(DIGITAL_UP)) {
            if (intake_slow == 1.0f) {
                intake_slow *= slow_mult;
            } else {
                intake_slow = 1.0f;
            }
            // update intake speed
            if (!intake_rev) {
                intake_motor.move_voltage(intake_volt * intake_slow);
            } else {
                intake_motor.move_voltage(-intake_volt * intake_slow);
            }
        }

        if (master.get_digital_new_press(DIGITAL_RIGHT)) {
            if (lift_slow == 1.0f) {
                lift_slow *= slow_mult;
            } else {
                lift_slow = 1.0f;
            }
            // update lift speed
            if (!lift_rev) {
                lift_motor.move_voltage(lift_volt * lift_slow);
            } else {
                lift_motor.move_voltage(-lift_volt * lift_slow);
            }
        }

        // Intake Logic

        // When user presses L2 activates reverse
        if (master.get_digital_new_press(DIGITAL_L2)) {
            intake_rev = true;
            if (intake_active) {
                intake_motor.move_voltage(-intake_volt * intake_slow);
            }
        }

        if (master.get_digital_new_release(DIGITAL_L2)) {
            intake_rev = false;
            if (intake_active) {
                intake_motor.move_voltage(intake_volt * intake_slow);
            }
        }
        
        // Toggle on/off intake
        if (master.get_digital_new_press(DIGITAL_L1)) {
            intake_active = !intake_active;
        }
        
        // Turns the intake on/off/slow/reverse based on the variables
        if (intake_active) {
            if (!intake_rev) {
                intake_motor.move_voltage(intake_volt * intake_slow);
            } else {
                intake_motor.move_voltage(-intake_volt * intake_slow);
            }
        } else {
            intake_motor.move_voltage(0);
        }

        // Lifting


        // When user presses R2 activates reverse
        if (master.get_digital_new_press(DIGITAL_R2)) {
            lift_rev = true;
            if (lift_active) {
                lift_motor.move_voltage(-lift_volt * lift_slow);
            }
        }

        // deactivates when user releases R2
        if (master.get_digital_new_release(DIGITAL_R2)) {
            lift_rev = false;
            if (lift_active) {
                lift_motor.move_voltage(lift_volt * lift_slow);
            }
        }

        // Toggle on/off lift
        if (master.get_digital_new_press(DIGITAL_R1)) {
            lift_active = !lift_active;
        }

        if (lift_active) {
            if (!lift_rev) {
                lift_motor.move_voltage(lift_volt * lift_slow);
            } else {
                lift_motor.move_voltage(-lift_volt * lift_slow);
            }
        } else {
            lift_motor.move_voltage(0);
        }

        // Drive Logic
        int dir = master.get_analog(ANALOG_LEFT_Y);    
        int turn = master.get_analog(ANALOG_RIGHT_X);  
        left_mg.move(dir - turn);                      
        right_mg.move(dir + turn);                     
        
        pros::delay(20);                               
    }
}