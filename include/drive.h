#ifndef DRIVE_H
#define DRIVE_H
#include "api.h"
#include "hardware.h"

struct input_frame_bitfield {
    bool aPressed : 1; /*use a bitfield to conserve space*/
    bool bPressed : 1;
    bool xPressed : 1;
    bool yPressed : 1;
    bool leftPressed : 1;
    bool upPressed : 1;
    bool rightPressed : 1;
    bool downPressed : 1;
    bool l1Pressed : 1;
    bool r1Pressed : 1;
    bool l2Pressed : 1;
    bool r2Pressed : 1;
};

struct input_frame {
    input_frame_bitfield buttons;
    int8_t leftStickX;
    int8_t leftStickY;
    int8_t rightStickX;
    int8_t rightStickY;
};

struct driveState_t {
#ifdef TEAM1
    bool spinnerActive;
    bool inverseDrive;
    int32_t targetSpinnerSpeed;
    void init() {
        spinnerActive = false;
        inverseDrive = false;
        targetSpinnerSpeed = 127;
    }
#endif
#ifdef TEAM2
    bool spinnerActive;
    bool inverseDrive;
    int32_t targetSpinnerSpeed;
    bool targetGoal; //if true drive will use targetting routing (exitable by b)
    double targetAngle;
    double targetDistance;
    bool release;
    void init() {
        spinnerActive = false;
        inverseDrive = false;
        targetSpinnerSpeed = 127;
        targetGoal = false;
        targetAngle = 0.0;
        targetDistance = 0.0;
        release = false;
    }
#endif
#ifdef TEAM3
    bool release;
    void init() {
        release = false;
    }
#endif
#ifdef TEAM4
    bool inverseDrive;
    bool spinnerActive;
    int32_t targetSpinnerSpeed;
    void init() {
        inverseDrive = false;
        spinnerActive = false;
        targetSpinnerSpeed = 127;
    }
#endif
#ifdef TEAM5
    bool inverseDrive;
    bool catapultActive;
    bool catapultArmed;
    bool release;
    void init() {
        inverseDrive = false;
        catapultActive = false;
        catapultArmed = false;
        release = false;
    }
#endif
#ifdef TEAM6
    void init() {}
#endif
};

void populateInputFrame(input_frame* frame);
void driveRobot(bool allowExit);
void driveLoop(input_frame* currentFrame, input_frame* prevFrame,driveState_t* state);

extern input_frame nullInputFrame;

#endif