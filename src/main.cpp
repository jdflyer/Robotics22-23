#include "main.h"
#include "hardware.h"
#include "auton.h"
#include "drive.h"
#include "robotmenu.h"


void initialize() {
	/*const char* status = load_auton_file("/usd/scripts/testscript.txt");
	if (status != nullptr) {
		controller.print(0,0,"%s",status);
	}else {
		controller.print(0,0,"%f",commandVector[1].params[0]);
	}*/
	analogOutput.set_value(4095); //from 0 (0V) to 4095 (5V)
	digitalOut.set_value(1);

	hardwareInit();

init:
	controller.clear();
	pros::delay(50);

	controller.print(0,0,"Press X To Drive");
	pros::delay(50);
	controller.print(1,0,"Press A For menu");

	while(true) {
		if (controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_X)) {
			controller.clear();
			driveRobot(true);
			goto init;
		}else if(controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_A)) {
			{	
				Menu mainMenu;
				controller.clear();
				bool exit = mainMenu.runMainMenu();
				if (exit) {
					return;
				}
			}
			goto init;
		}
		pros::delay(10);
	}
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
	controller.clear();
	pros::screen::erase();
	PRINT(0,"Disaled",0);
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {

}

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
	controller.clear();
	if (mainAutonSet.set) {
		if (mainAutonSet.isMacro) {
			autonPlayMacro(mainAutonSet.info.macroInfo.buffer);
			free(mainAutonSet.info.macroInfo.buffer);
			return;
		}else{

		}
	}else{
		pros::screen::erase();
		PRINT(0,"Auton Not Set!",0);
	}
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *>().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	pros::screen::erase();
	PRINT(0,"operator control",0);
	driveRobot(false);
}
