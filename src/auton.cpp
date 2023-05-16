#include "auton.h"

SetAuton mainAutonSet;

const Command commands[COMMAND_NUMBER] = {
    {"START_POS",START_POS,2},
    {"START_HEADING",START_HEADING,1},
    {"DRIVE",DRIVE,1},
    {"SET_HEADING",SET_HEADING,1},
    {"SET_POSITION",SET_POSITION,3},
    {"WAIT",WAIT,1}
    };

static const Command* getCommandFromString(std::string& command) {
    for (int i = 0; i < (sizeof(commands) / sizeof(Command)); i++)
    {
        if (command.compare(commands[i].commandString) == 0)
        {
            return &(commands[i]);
        }
    }
    return nullptr;
}

static const char* getStringFromCommand(CommandEnum command) {
    return commands[command].commandString;
}

std::vector<ParsedCommand> commandVector;

const char *load_auton_file(const char *name)
{
    commandVector.clear();

    const char *error = nullptr;
    FILE *autonFile = fopen(name, "r");

    char line[256]; //line limit is technically 255 characters for now
    char* linep = line;

    uint32_t lineNum = 0;
    if (autonFile == nullptr){
        error = "File failed to load!";
        goto end;
    }
    while (fgets(line,sizeof(line),autonFile)) {
        lineNum++;
        linep = line;

        while (*linep == ' ' || *linep == '\t') {
            linep++; //flush whitespace
        }
        if (*linep == '\n' || *linep == '\r' || *linep == '#') {
            continue;
        }
        std::string commandString;
        while (true) { //Get command type
            commandString = (commandString + *linep);
            linep++;
            if (*linep == ' ' || *linep == '\n' || *linep == '\r') {
                break;
            }
        }
        const Command* currentCommand = getCommandFromString(commandString);
        if (currentCommand == nullptr) {
            error = "Command Parse Failed!";
            goto end;
        }
        while (*linep == ' ' || *linep == '\t') {
            linep++; //flush whitespace
        }
        ParsedCommand parsedCommand(currentCommand->commandEnum,currentCommand->numParams);
        if (currentCommand->numParams != 0) {
            for (int i = 0; i<currentCommand->numParams; i++) {
                parsedCommand.params[i] = strtof(linep,&linep);
                while (*linep == ',' || *linep == ' ' || *linep == '\t') {
                   linep++; //flush whitespace
                }
            }
        }
        commandVector.push_back(parsedCommand);
    }

end:
    fclose(autonFile);
    return error;
}

void autonPlayMacro(uint8_t* buffer) {
    pros::screen::erase();
    PRINT(0, "Playing...", 0);

    uint8_t *data = buffer + sizeof(MacroHeader);
    MacroHeader* header = (MacroHeader*)buffer;

    driveState_t driveState;
    driveState.init();
    if (header->recordMotors)
    {
        for (int i = 0; i < header->numFrames; i++)
        {
            for (int j = 0; j < MOTOR_NUM; j++)
            {
                *(motors[j]) = *((int8_t *)(data + (i * MOTOR_NUM * sizeof(int8_t)) + (j * sizeof(int8_t))));
            }
            pros::delay(10);
        }
    }
    else
    {
        input_frame *currentFrame = (input_frame *)data;
        input_frame *previousFrame = &nullInputFrame;
        for (int i = 0; i < header->numFrames - 1; i++)
        {
            previousFrame = currentFrame;
            currentFrame = currentFrame + 1;
            driveLoop(currentFrame, previousFrame, &driveState);
            pros::delay(10);
        }
    }

    for (int i = 0; i < MOTOR_NUM; i++)
    {
        *motors[i] = 0;
    }
}

void autonRunMacroInstant(const char *fileName)
{
    FILE *file = fopen(fileName, "r");

    fseek(file, 0, SEEK_END);
    size_t dataSize = ftell(file);
    rewind(file);

    uint8_t *buffer = (uint8_t *)malloc(dataSize);
    fread(buffer, dataSize, 1, file);

    autonPlayMacro(buffer);

    free(buffer);
    fclose(file);
}

uint8_t * autonRecordMacroFile(size_t *dataSize, int lengthInSeconds, bool recordMotors)
{
    uint32_t numFrames = lengthInSeconds * 100;
    if (recordMotors == false)
    {
        *dataSize = sizeof(MacroHeader) + (numFrames * sizeof(input_frame));
    }
    else
    {
        *dataSize = sizeof(MacroHeader) + ((numFrames * sizeof(uint8_t) * MOTOR_NUM));
    }
    uint8_t *buffer = (uint8_t *)malloc(*dataSize);
    memset(buffer, 0, *dataSize);

    uint8_t *data = buffer + sizeof(MacroHeader);
    MacroHeader *head = (MacroHeader *)buffer;
    head->ident[0] = 'M';
    head->ident[1] = 'A';
    head->ident[2] = 'C';
    head->ident[3] = 'R';
    head->numFrames = numFrames;
    head->version = MACRO_VERSION;
    head->recordMotors = recordMotors;
    if (recordMotors == false)
    {
        head->numMotors = 0;
    }
    else
    {
        head->numMotors = MOTOR_NUM;
    }

    controller.clear();
    pros::delay(500);
    for (int i = 3; i != 0; i--)
    {
        controller.print(0, 0, "%d", i);
        pros::delay(100);
        controller.rumble("-");
        pros::delay(900);
    }
    controller.clear();
    pros::delay(100);
    controller.rumble(".");

    driveState_t driveState;
    driveState.init();

    if (recordMotors == false)
    {
        // Record controller inputs into buffer
        input_frame *currentFrame = (input_frame *)data;
        input_frame *previousFrame = &nullInputFrame;

        for (int i = 0; i < numFrames - 1; i++)
        {
            previousFrame = currentFrame;
            currentFrame = currentFrame + 1;
            populateInputFrame(currentFrame);
            driveLoop(currentFrame, previousFrame, &driveState);
            pros::delay(10);
        }
    }
    else
    {
        // Record Motor Velocities (and eventually positions) to buffer instead of controller inputs
        input_frame frame1 = nullInputFrame;
        input_frame frame2 = nullInputFrame;
        input_frame *currentFrame = &frame1;
        input_frame *previousFrame = &frame2;
        for (int i = 0; i < numFrames; i++)
        {
            input_frame *temp = previousFrame;
            previousFrame = currentFrame;
            currentFrame = temp;
            populateInputFrame(currentFrame);
            driveLoop(currentFrame, previousFrame, &driveState);
            for (int j = 0; j < MOTOR_NUM; j++)
            {
                *((int8_t *)(data + (i * MOTOR_NUM * sizeof(int8_t)) + (j * sizeof(int8_t)))) = motors[j]->mTargetVelocity; // re-evaluate
            }
            pros::delay(10);
        }
    }

    for (int i = 0; i < MOTOR_NUM; i++)
    {
        *motors[i] = 0;
    }

    return buffer;
}

void autonSetMacro(const char* fileName) {
    mainAutonSet.isMacro = true;
    mainAutonSet.set = true;
    FILE* macroFile = fopen(fileName,"r");
    fseek(macroFile,0,SEEK_END);
    size_t macroFileSize = ftell(macroFile);
    rewind(macroFile);
    mainAutonSet.info.macroInfo.buffer = (uint8_t*)malloc(macroFileSize);
    //make sure to free!
    fread(mainAutonSet.info.macroInfo.buffer,macroFileSize,1,macroFile);
    pros::screen::erase();
    PRINT(0,"Auton Set To %s",fileName);
    return;
}
