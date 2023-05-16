#ifndef HARDWARE_H
#define HARDWARE_H

#include "api.h"

#if !defined(TEAM1) && !defined(TEAM2) && !defined(TEAM3) && !defined(TEAM4) && !defined(TEAM5) && !defined(TEAM6)
#error "No Team defined!"
#endif


extern pros::Controller controller;
extern pros::ADIAnalogOut analogOutput;
extern pros::ADIDigitalOut digitalOut;

void hardwareInit();

class Motor {
public:
    const char* mName;
    uint8_t mPortNumber; //default
    bool mReversed; //default
    int8_t mTargetVelocity;
    pros::Motor* mMotor;

    Motor(const char* name, int defaultPort, bool reversed);
    ~Motor();

    void restartMotor();
    void setupMotor();
    void operator=(int32_t value);

    static void loadMotorsFromFile();
    static void saveMotorsToFile();
    static void restartAllMotors();
};

extern Motor* motors[];

#ifdef TEAM1

extern Motor topSpinner;
extern Motor bottomSpinner;
extern Motor leftWheel1;
extern Motor rightWheel1;
extern Motor leftWheel2;
extern Motor rightWheel2;
extern Motor intakeMotor;
extern Motor releaseMotor;
extern Motor spinnerMotor;

#define MOTOR_NUM 8

#endif

#ifdef TEAM2
extern Motor leftSpinner;
extern Motor rightSpinner;
extern Motor feedTape;
extern Motor intakeMotor;
extern Motor leftWheel;
extern Motor rightWheel;
extern Motor leftWheel2;
extern Motor rightWheel2;

extern pros::Vision visionSensor;
extern pros::vision_signature_s_t blueGoalSig;
extern pros::vision_signature_s_t redGoalSig;

extern pros::Distance leftDistanceSensor;
extern pros::Distance backDistanceSensor;

extern pros::Imu intertialSensor;

#define BLUEGOAL_SIGNATURE_ID 1
#define REDGOAL_SIGNATURE_ID 2

#define MOTOR_NUM 8

extern pros::ADIDigitalOut releasePort;

#endif

#ifdef TEAM3

#define MOTOR_NUM 8

extern Motor leftWheel1;
extern Motor rightWheel1;
extern Motor leftWheel2;
extern Motor rightWheel2;
extern Motor leftWheel3;
extern Motor rightWheel3;
extern Motor spinnerMotor;
extern Motor spoolMotor;

extern pros::ADIDigitalOut releasePort;

#endif

#ifdef TEAM4

#define MOTOR_NUM 8

extern Motor leftWheel1;
extern Motor leftWheel2;
extern Motor rightWheel1;
extern Motor rightWheel2;
extern Motor leftSpinner;
extern Motor rightSpinner;
extern Motor intakeMotor1;
extern Motor intakeMotor2;

#endif

#ifdef TEAM5

#define MOTOR_NUM 8

extern Motor leftWheel1;
extern Motor rightWheel1;
extern Motor leftWheel2;
extern Motor rightWheel2;
extern Motor catapultMotor1;
extern Motor catapultMotor2;
extern Motor intakeMotor1;
extern Motor intakeMotor2;

extern pros::Rotation catapultRotationSensor;
extern pros::ADIDigitalOut releasePort;

#endif

#ifdef TEAM6

#define MOTOR_NUM 4

extern Motor leftWheel;
extern Motor rightWheel;
extern Motor catapultMotor1;
extern Motor catapultMotor2;

#endif

#endif