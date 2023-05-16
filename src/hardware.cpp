#include "hardware.h"

pros::Controller controller(pros::controller_id_e_t::E_CONTROLLER_MASTER);
pros::ADIAnalogOut analogOutput('B');
pros::ADIDigitalOut digitalOut('C');

void hardwareInit()
{
    Motor::loadMotorsFromFile();
#ifdef TEAM2

    visionSensor.set_signature(BLUEGOAL_SIGNATURE_ID, &blueGoalSig);
    visionSensor.set_signature(REDGOAL_SIGNATURE_ID, &redGoalSig);
    intertialSensor.reset();
#endif
#ifdef TEAM5
    catapultRotationSensor.set_reversed(false);
    catapultRotationSensor.reset_position();
    catapultRotationSensor.set_position(0);
#endif
}

void Motor::operator=(int32_t value)
{
    if (value > 127)
    {
        value = 127;
    }
    else if (value < -127)
    {
        value = -127;
    }
    mTargetVelocity = value;
    if (mMotor)
    {
        *mMotor = value;
    }
}

Motor::Motor(const char *name, int defaultPort, bool reversed)
{
    mName = name;
    mPortNumber = defaultPort;
    mReversed = reversed;
    mMotor = new pros::Motor(mPortNumber, mReversed);
    setupMotor();
}

void Motor::setupMotor()
{
    if (mMotor)
    {
        mMotor->set_encoder_units(pros::motor_encoder_units_e_t::E_MOTOR_ENCODER_ROTATIONS);
    }
    mTargetVelocity = 0;
}

Motor::~Motor()
{
    if (mMotor)
    {
        delete mMotor;
    }
}

void Motor::restartMotor()
{
    if (mMotor)
    {
        delete mMotor;
    }
    mMotor = new pros::Motor(mPortNumber, mReversed);
    setupMotor();
}

void Motor::restartAllMotors()
{ // Ensures no motors are on the same port during restart
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        if (motors[i]->mMotor != nullptr)
        {
            delete motors[i]->mMotor;
        }
    }
    for (int i = 0; i < MOTOR_NUM; i++)
    {
        motors[i]->mMotor = new pros::Motor(motors[i]->mPortNumber, motors[i]->mReversed);
        motors[i]->setupMotor();
    }
}

void Motor::saveMotorsToFile()
{
    FILE *motorFile = fopen("/usd/motors.txt", "w");
    if (motorFile == nullptr)
    {
        pros::screen::print(pros::text_format_e_t::E_TEXT_MEDIUM, 0, "Motor Save File Failed!");
        return; // Likely no sd card or a filesystem error
    }

    for (int i = 0; i < MOTOR_NUM; i++)
    {
        fprintf(motorFile, "%s %d %d\n", motors[i]->mName, motors[i]->mPortNumber, motors[i]->mReversed);
    }

    fclose(motorFile);
}

void Motor::loadMotorsFromFile()
{
    FILE *motorFile = fopen("/usd/motors.txt", "r");

    if (motorFile == nullptr)
    {
        pros::delay(500);
        pros::screen::print(pros::text_format_e_t::E_TEXT_MEDIUM, 0, "Motor Read File Failed!");
        saveMotorsToFile();
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), motorFile))
    {
        char *token = strtok(line, " ");
        if (token == nullptr)
        {
            break;
        }

        Motor *matchingMotor = nullptr;
        for (int i = 0; i < MOTOR_NUM; i++)
        {
            if (strcmp(motors[i]->mName, token) == 0)
            {
                matchingMotor = motors[i];
                break;
            };
        }
        if (matchingMotor == nullptr)
        {
            break;
        }
        token = strtok(NULL, " ");
        if (token == nullptr)
        {
            break;
        }
        matchingMotor->mPortNumber = atoi(token);
        token = strtok(NULL, " ");
        if (token == nullptr)
        {
            break;
        }
        matchingMotor->mReversed = atoi(token);
    }

    restartAllMotors();
    fclose(motorFile);
}

#ifdef TEAM1
Motor topSpinner("topSpinner", 1, false);
Motor bottomSpinner("bottomSpinner", 2, false);
Motor leftWheel1("leftWheel1", 3, false);
Motor rightWheel1("rightWheel1", 4, false);
Motor leftWheel2("leftWheel2", 8, false);
Motor rightWheel2("rightWheel2", 9, false);
Motor intakeMotor("feedTape", 5, false);
Motor releaseMotor("releaseMotor", 6, false);

Motor *motors[MOTOR_NUM]{
    &topSpinner, &bottomSpinner, &leftWheel1, &rightWheel1,&leftWheel2, &rightWheel2, &intakeMotor, &releaseMotor};

#endif

#ifdef TEAM2

Motor leftSpinner("leftSpinner", 17, true);
Motor rightSpinner("rightSpinner", 16, false);
Motor feedTape("feedTape", 12, true);
Motor intakeMotor("intakeMotor", 20, true);
Motor leftWheel("leftWheel", 5, false);
Motor rightWheel("rightWheel", 18, true);
Motor leftWheel2("leftWheel2", 15, true);
Motor rightWheel2("rightWheel2", 2, false);

pros::Vision visionSensor(1, pros::E_VISION_ZERO_CENTER);
pros::vision_signature_s_t blueGoalSig = pros::Vision::signature_from_utility(BLUEGOAL_SIGNATURE_ID, -2443, -63, -1253, 2029, 10445, 6237, 0.800f, 0);
pros::vision_signature_s_t redGoalSig = pros::Vision::signature_from_utility(REDGOAL_SIGNATURE_ID, 1543, 5439, 3491, -903, 461, -221, 1.000, 0);

pros::Distance leftDistanceSensor(8);
pros::Distance backDistanceSensor(4);

pros::Imu intertialSensor(14);

pros::ADIDigitalOut releasePort('A', false);

Motor *motors[MOTOR_NUM]{
    &leftSpinner, &rightSpinner, &feedTape, &intakeMotor, &leftWheel, &rightWheel, &leftWheel2, &rightWheel2};

#endif

#ifdef TEAM3

Motor leftWheel1("leftWheel1", 1, false);
Motor rightWheel1("rightWheel1", 2, false);
Motor leftWheel2("leftWheel2", 3, false);
Motor rightWheel2("rightWheel2", 4, false);
Motor leftWheel3("leftWheel3", 5, false);
Motor rightWheel3("rightWheel3", 6, false);
Motor spinnerMotor("spinnerMotor", 7, false);
Motor spoolMotor("spoolMotor", 8, false);

pros::ADIDigitalOut releasePort('A', false);

Motor *motors[MOTOR_NUM]{
    &leftWheel1, &rightWheel1, &leftWheel2, &rightWheel2, &leftWheel3, &rightWheel3, &spinnerMotor, &spoolMotor};

#endif

#ifdef TEAM4

Motor leftWheel1("leftWheel1", 1, false);
Motor leftWheel2("leftWheel2", 2, false);
Motor rightWheel1("rightWheel1", 3, false);
Motor rightWheel2("rightWheel2", 4, false);
Motor leftSpinner("leftSpinner", 5, false);
Motor rightSpinner("rightSpinner", 6, false);
Motor intakeMotor1("intakeMotor1", 7, false);
Motor intakeMotor2("intakeMotor2", 8, false);

Motor *motors[MOTOR_NUM]{&leftWheel1, &leftWheel2, &rightWheel1, &rightWheel2, &leftSpinner, &rightSpinner, &intakeMotor1, &intakeMotor2};

#endif

#ifdef TEAM5

Motor leftWheel1("leftWheel1", 1, false);
Motor rightWheel1("rightWheel1", 2, false);
Motor leftWheel2("leftWheel2", 3, false);
Motor rightWheel2("rightWheel2", 4, false);
Motor catapultMotor1("catapultMotor1", 5, false);
Motor catapultMotor2("catapultMotor2", 6, false);
Motor intakeMotor1("intakeMotor1", 7, false);
Motor intakeMotor2("intakeMotor2", 8, false);

pros::Rotation catapultRotationSensor(13);
pros::ADIDigitalOut releasePort('A', false);

Motor *motors[MOTOR_NUM]{
    &leftWheel1, &rightWheel1, &leftWheel2, &rightWheel2, &catapultMotor1,&catapultMotor2,&intakeMotor1,&intakeMotor2};


#endif

#ifdef TEAM6

Motor leftWheel("leftWheel",1,false);
Motor rightWheel("rightWheel",2,false);
Motor catapultMotor1("catapultMotor1",3,false);
Motor catapultMotor2("catapultMotor2",4,false);

Motor *motors[MOTOR_NUM]{&leftWheel,&rightWheel,&catapultMotor1,&catapultMotor2};


#endif
