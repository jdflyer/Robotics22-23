#include "drive.h"

input_frame nullInputFrame = {{false, false, false, false, false, false, false, false, false, false, false, false}, 0, 0, 0, 0};

void populateInputFrame(input_frame *frame)
{
    frame->leftStickX = controller.get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_LEFT_X);
    frame->leftStickY = controller.get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_LEFT_Y);
    frame->rightStickX = controller.get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_RIGHT_X);
    frame->rightStickY = controller.get_analog(pros::controller_analog_e_t::E_CONTROLLER_ANALOG_RIGHT_Y);
    frame->buttons.aPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_A);
    frame->buttons.bPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_B);
    frame->buttons.xPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_X);
    frame->buttons.yPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_Y);
    frame->buttons.leftPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_LEFT);
    frame->buttons.upPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_UP);
    frame->buttons.rightPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_RIGHT);
    frame->buttons.downPressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_DOWN);
    frame->buttons.l1Pressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_L1);
    frame->buttons.r1Pressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_R1);
    frame->buttons.l2Pressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_L2);
    frame->buttons.r2Pressed = controller.get_digital(pros::controller_digital_e_t::E_CONTROLLER_DIGITAL_R2);
}

void driveRobot(bool allowExit)
{
    input_frame frame1 = nullInputFrame;
    input_frame frame2 = nullInputFrame;
    input_frame *currentFrame = &frame1;
    input_frame *previousFrame = &frame2;
    driveState_t driveState;
    int analogSignal = 4095;
    driveState.init();
    int timer = 0;
    while (true)
    {
        input_frame *temp = previousFrame;
        previousFrame = currentFrame;
        currentFrame = temp;
        populateInputFrame(currentFrame);
        if (allowExit && currentFrame->buttons.bPressed)
        {
            break;
        }
        driveLoop(currentFrame, previousFrame, &driveState);

        timer++;
        if (timer == 10)
        {
            timer = 0;
#ifdef TEAM2
            controller.print(0, 0, "%d %03d %04d %03d %03d", driveState.inverseDrive, driveState.targetSpinnerSpeed, (int)driveState.targetDistance, (int)driveState.targetAngle, (int)leftSpinner.mMotor->get_actual_velocity());
#endif
#ifdef TEAM1
            controller.print(0, 0, "%d %03d %03d", driveState.inverseDrive, driveState.targetSpinnerSpeed, (int)topSpinner.mMotor->get_actual_velocity());
#endif
#ifdef TEAM3
            analogSignal = analogSignal + currentFrame->rightStickY;
            controller.print(0, 0, "%08d", analogSignal);
            analogOutput.set_value(analogSignal); // from 0 (0V) to 4095 (5V)
#endif
#ifdef TEAM4
            controller.print(0, 0, "%d %03d", driveState.inverseDrive, driveState.targetSpinnerSpeed);
#endif
#ifdef TEAM5
            controller.print(0, 0, "%d %d %d %05d", driveState.inverseDrive, driveState.catapultActive, driveState.catapultArmed, catapultRotationSensor.get_angle());
#endif
        }
        pros::delay(10);
    }
    controller.clear();
}

void driveLoop(input_frame *currentFrame, input_frame *prevFrame, driveState_t *state)
{

#ifdef TEAM1

    if (currentFrame->buttons.upPressed == true && prevFrame->buttons.upPressed == false)
    {
        state->spinnerActive = !state->spinnerActive;
    }
    if (currentFrame->buttons.l1Pressed == true && prevFrame->buttons.l1Pressed == false)
    {
        state->inverseDrive = !state->inverseDrive;
    }

    if (currentFrame->rightStickY > 15 || currentFrame->rightStickY < -15)
    {
        state->targetSpinnerSpeed = state->targetSpinnerSpeed + (currentFrame->rightStickY / 100);
    }

    if (state->targetSpinnerSpeed > 127)
    {
        state->targetSpinnerSpeed = 127;
    }
    else if (state->targetSpinnerSpeed < 10)
    {
        state->targetSpinnerSpeed = 10;
    }

    if (state->spinnerActive)
    {
        topSpinner = state->targetSpinnerSpeed;
        bottomSpinner = state->targetSpinnerSpeed;
    }
    else
    {
        topSpinner = 0;
        bottomSpinner = 0;
    }

    int32_t driveSpeed = currentFrame->leftStickY;
    int32_t turnSpeed = currentFrame->rightStickX;

    if (state->inverseDrive == false)
    {
        leftWheel1 = driveSpeed + turnSpeed;
        leftWheel2 = driveSpeed + turnSpeed;
        rightWheel1 = driveSpeed - turnSpeed;
        rightWheel2 = driveSpeed - turnSpeed;
    }
    else
    {
        leftWheel1 = -driveSpeed + turnSpeed;
        leftWheel2 = -driveSpeed + turnSpeed;
        rightWheel1 = -driveSpeed - turnSpeed;
        rightWheel2 = -driveSpeed - turnSpeed;
    }

    if (currentFrame->buttons.aPressed)
    {
        intakeMotor = 127;
    }
    else
    {
        intakeMotor = 0;
    }

    if (currentFrame->buttons.yPressed)
    {
        releaseMotor = 127;
    }
    else
    {
        releaseMotor = 0;
    }

#endif

#ifdef TEAM2

    struct distanceToSpeedData_t
    {
        int16_t distanceToGoal;
        int8_t targetSpeed;
    };

    const distanceToSpeedData_t distanceToSpeed[] = {
        {2434, 112},

    };

    /*if (state->targetGoal) {
        if (currentFrame->buttons.downPressed&&prevFrame->buttons.downPressed==false) {
            state->targetGoal = false;
            state->targetAngle = 0.0;
            state->targetDistance = 0.0;
            return;
        }

        return;
    }*/

    if (currentFrame->buttons.leftPressed && prevFrame->buttons.leftPressed == false)
    {

        int32_t leftDistance = leftDistanceSensor.get();
        int32_t backDistance = backDistanceSensor.get();
        double angle = intertialSensor.get_heading();

        if (leftDistance != 9999 && backDistance != 9999)
        {
            const double distanceFromGoalToBackWall = 3200.4; // 126 inches; 3200.4mm
            const double distanceFromSideWallToGoal = 457.2;  // 18 inches; 457.2mm
            const double leftDistanceAdjust = 228.6;          // distance from left sensor to shooter
            const double backDistanceAdjust = 381.0;

            state->targetGoal = true;
            double nearestAngle = round(angle / 90.0) * 90.0;
            double theta = fabs(angle - nearestAngle);

            double leftDistanceTrue = ((double)leftDistance) * cos(theta * (M_PI / 180.0));
            double backDistanceTrue = ((double)backDistance) * cos(theta * (M_PI / 180.0));
            leftDistanceTrue = leftDistanceTrue + leftDistanceAdjust - distanceFromSideWallToGoal;
            backDistanceTrue = distanceFromGoalToBackWall - (backDistanceTrue + backDistanceAdjust);

            double distanceFromGoal = sqrt((leftDistanceTrue * leftDistanceTrue) + (backDistanceTrue * backDistanceTrue));

            state->targetAngle = atan(backDistanceTrue / leftDistanceTrue) * (180.0 / M_PI);
            state->targetDistance = distanceFromGoal;
        }
    }

    if (currentFrame->buttons.upPressed == true && prevFrame->buttons.upPressed == false)
    {
        state->spinnerActive = !state->spinnerActive;
    }
    if (currentFrame->buttons.l1Pressed == true && prevFrame->buttons.l1Pressed == false)
    {
        state->inverseDrive = !state->inverseDrive;
    }

    int8_t driveSpeed = currentFrame->leftStickY;
    int8_t turnSpeed = currentFrame->rightStickX;

    if (state->inverseDrive == false)
    {
        driveSpeed = -driveSpeed;
    }

    leftWheel = driveSpeed - turnSpeed;
    leftWheel2 = driveSpeed - turnSpeed;
    rightWheel = driveSpeed + turnSpeed;
    rightWheel2 = driveSpeed + turnSpeed;

    if (currentFrame->rightStickY > 15 || currentFrame->rightStickY < -15)
    {
        state->targetSpinnerSpeed = state->targetSpinnerSpeed + (currentFrame->rightStickY / 100);
    }

    if (state->targetSpinnerSpeed > 127)
    {
        state->targetSpinnerSpeed = 127;
    }
    else if (state->targetSpinnerSpeed < 10)
    {
        state->targetSpinnerSpeed = 10;
    }

    if (state->spinnerActive)
    {
        leftSpinner = state->targetSpinnerSpeed;
        rightSpinner = state->targetSpinnerSpeed;
    }
    else
    {
        leftSpinner = 0;
        rightSpinner = 0;
    }

    if (currentFrame->buttons.aPressed)
    {
        feedTape = 50;
    }
    /*else if (currentFrame->buttons.downPressed)
    {
        feedTape = -127;
    }*/
    else
    {
        feedTape = 0;
    }

    if (currentFrame->buttons.r1Pressed)
    {
        intakeMotor = 127;
    }
    else if (currentFrame->buttons.r2Pressed)
    {
        intakeMotor = -127;
    }
    else
    {
        intakeMotor = 0;
    }

    if (currentFrame->buttons.yPressed == true && prevFrame->buttons.yPressed == false)
    {
        state->release = !state->release;
    }
    releasePort.set_value(state->release);

#endif

#ifdef TEAM3
    int32_t driveSpeed = -currentFrame->leftStickY;
    int32_t turnSpeed = currentFrame->rightStickX;
    int32_t strafeSpeed = -currentFrame->leftStickX;

    if (driveSpeed < 20 && driveSpeed > -20)
    {
        driveSpeed = 0;
    }
    if (turnSpeed < 20 && turnSpeed > -20)
    {
        turnSpeed = 0;
    }
    if (strafeSpeed < 20 && strafeSpeed > -20)
    {
        strafeSpeed = 0;
    }

    // l3 r3
    // l2 r2
    // l1 r1

    leftWheel1 = driveSpeed + turnSpeed - strafeSpeed;
    leftWheel2 = driveSpeed + turnSpeed;
    leftWheel3 = driveSpeed + turnSpeed + strafeSpeed;
    rightWheel1 = driveSpeed - turnSpeed + strafeSpeed;
    rightWheel2 = driveSpeed - turnSpeed;
    rightWheel3 = driveSpeed - turnSpeed - strafeSpeed;

    if (currentFrame->buttons.r1Pressed)
    {
        spinnerMotor = 127;
    }
    else if (currentFrame->buttons.r2Pressed)
    {
        spinnerMotor = -127;
    }
    else
    {
        spinnerMotor = 0;
    }

    if (currentFrame->buttons.l1Pressed)
    {
        spoolMotor = 127;
    }
    else if (currentFrame->buttons.l2Pressed)
    {
        spoolMotor = -127;
    }
    else
    {
        spoolMotor = 0;
    }

    if (currentFrame->buttons.yPressed == true && prevFrame->buttons.yPressed == false)
    {
        state->release = !state->release;
    }
    releasePort.set_value(state->release);
#endif

#ifdef TEAM4
    if (currentFrame->buttons.upPressed == true && prevFrame->buttons.upPressed == false)
    {
        state->spinnerActive = !state->spinnerActive;
    }
    if (currentFrame->buttons.l1Pressed == true && prevFrame->buttons.l1Pressed == false)
    {
        state->inverseDrive = !state->inverseDrive;
    }

    if (currentFrame->rightStickY > 15 || currentFrame->rightStickY < -15)
    {
        state->targetSpinnerSpeed = state->targetSpinnerSpeed + (currentFrame->rightStickY / 100);
    }

    if (state->targetSpinnerSpeed > 127)
    {
        state->targetSpinnerSpeed = 127;
    }
    else if (state->targetSpinnerSpeed < 10)
    {
        state->targetSpinnerSpeed = 10;
    }

    if (state->spinnerActive)
    {
        leftSpinner = state->targetSpinnerSpeed;
        rightSpinner = state->targetSpinnerSpeed;
    }
    else
    {
        leftSpinner = 0;
        rightSpinner = 0;
    }

    int32_t driveSpeed = currentFrame->leftStickY;
    int32_t turnSpeed = currentFrame->rightStickX;

    if (state->inverseDrive == false)
    {
        leftWheel1 = driveSpeed + turnSpeed;
        leftWheel2 = driveSpeed + turnSpeed;
        rightWheel1 = driveSpeed - turnSpeed;
        rightWheel2 = driveSpeed - turnSpeed;
    }
    else
    {
        leftWheel1 = -driveSpeed + turnSpeed;
        rightWheel1 = -driveSpeed - turnSpeed;
        leftWheel2 = -driveSpeed + turnSpeed;
        rightWheel2 = -driveSpeed - turnSpeed;
    }

    if (currentFrame->buttons.aPressed)
    {
        intakeMotor1 = 127;
        intakeMotor2 = 127;
    }
    else
    {
        intakeMotor1 = 0;
        intakeMotor2 = 0;
    }

#endif

#ifdef TEAM5

    if (currentFrame->buttons.l1Pressed == true && prevFrame->buttons.l1Pressed == false)
    {
        state->inverseDrive = !state->inverseDrive;
    }

    if (currentFrame->buttons.upPressed == true && prevFrame->buttons.upPressed == false)
    {
        state->catapultArmed = !state->catapultArmed;
    }
    {
    }

    if (currentFrame->buttons.aPressed == true && prevFrame->buttons.aPressed == false)
    {
        if (state->catapultArmed)
        {
            state->catapultActive = !state->catapultActive;
        }
    }

    if (currentFrame->buttons.yPressed == true && prevFrame->buttons.yPressed == false)
    {
        state->release = !state->release;
    }

    int32_t driveSpeed = currentFrame->leftStickY;
    int32_t turnSpeed = currentFrame->rightStickX;

    if (abs(driveSpeed) < 20)
    {
        driveSpeed = 0;
    }
    if (abs(turnSpeed) < 20)
    {
        turnSpeed = 0;
    }

    if (state->inverseDrive == true)
    {
        driveSpeed = -driveSpeed;
    }

    leftWheel1 = driveSpeed + turnSpeed;
    leftWheel2 = driveSpeed + turnSpeed;
    rightWheel1 = driveSpeed - turnSpeed;
    rightWheel2 = driveSpeed - turnSpeed;

    // catapultMotor1 = 127;
    // catapultMotor2 = 127;

    if (state->catapultArmed)
    {
        if (state->catapultActive)
        {
            catapultMotor1.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
            catapultMotor1 = 127;
            catapultMotor2.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
            catapultMotor2 = 127;
            printf("launching\n");
            if (catapultRotationSensor.get_angle() > 10000 && catapultRotationSensor.get_angle() < 30000)
            {
                state->catapultActive = false;
                printf("resetting\n");
            }
        }
        if (state->catapultActive == false)
        {
            if (catapultRotationSensor.get_angle() < 1000 || catapultRotationSensor.get_angle() > 30000)
            {
                catapultMotor1.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_HOLD);
                catapultMotor1.mMotor->brake();
                catapultMotor2.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_HOLD);
                catapultMotor2.mMotor->brake();
                printf("stopped\n");
            }
            else
            {
                catapultMotor1.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
                catapultMotor2.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
                catapultMotor1 = 127;
                catapultMotor2 = 127;
                printf("down\n");
            }
        }
    }
    else if (state->catapultArmed == false)
    {
        catapultMotor1.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
        catapultMotor2.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
        if (currentFrame->buttons.l2Pressed)
        {
            catapultMotor1 = 127;
            catapultMotor2 = 127;
        }
        else
        {
            catapultMotor1 = 0;
            catapultMotor2 = 0;
        }
    }

    if (currentFrame->buttons.r1Pressed)
    {
        intakeMotor1 = 127;
        intakeMotor2 = 127;
    }
    else if (currentFrame->buttons.r2Pressed)
    {
        intakeMotor1 = -127;
        intakeMotor2 = -127;
    }
    else
    {
        intakeMotor1 = 0;
        intakeMotor2 = 0;
    }

    releasePort.set_value(state->release);

#endif

#ifdef TEAM6
    if (currentFrame->buttons.aPressed) {
        catapultMotor1.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
        catapultMotor2.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_COAST);
        catapultMotor1 = 127;
        catapultMotor2 = 127;
    }else{
        catapultMotor1.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_HOLD);
        catapultMotor2.mMotor->set_brake_mode(pros::motor_brake_mode_e::E_MOTOR_BRAKE_HOLD);
        catapultMotor1.mMotor->brake();
        catapultMotor2.mMotor->brake();
    }

    int32_t driveSpeed = currentFrame->leftStickY;
    int32_t turnSpeed = currentFrame->rightStickX;

    leftWheel = driveSpeed + turnSpeed;
    rightWheel = driveSpeed - turnSpeed;

#endif
}
