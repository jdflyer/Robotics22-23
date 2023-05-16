#ifndef AUTON_PLAYER_H
#define AUTON_PLAYER_H
#include <stdio.h>
#include <vector>

#include "api.h"
#include "hardware.h"
#include "robotmenu.h"

#define COMMAND_NUMBER 6

enum CommandEnum {
    START_POS,
    START_HEADING,
    DRIVE,
    SET_HEADING,
    SET_POSITION,
    WAIT
};

struct ParsedCommand {
    CommandEnum command;
    std::vector<float> params;
    ParsedCommand(int argNum) : params(argNum) {

    }
    ParsedCommand(CommandEnum command_, int argNum) : params(argNum) {
        command = command_;
    }
};

struct Command
{
    const char* commandString;
    const CommandEnum commandEnum;
    const int numParams;
};

extern const Command commands[COMMAND_NUMBER];

extern std::vector<ParsedCommand> commandVector;
const char *load_auton_file(const char *name);

#define MACRO_VERSION 1

struct MacroHeader
{
    char ident[4]; // MACR
    uint32_t version;
    uint32_t numFrames;
    bool recordMotors;
    uint8_t numMotors;
    uint8_t padding[2];
};

struct SetAuton {
    bool set; //set True when auton is setup
    bool isMacro; //true For Macro, false for script
    union {
        struct {
            uint8_t* buffer;
        }macroInfo;
        struct {

        }scriptInfo;
    }info;
};

extern SetAuton mainAutonSet;

uint8_t * autonRecordMacroFile(size_t *dataSize, int lengthInSeconds, bool recordMotors);
void autonRunMacroInstant(const char *fileName);
void autonSetMacro(const char* fileName);
void autonPlayMacro(uint8_t* buffer);

#endif