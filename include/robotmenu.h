#ifndef ROBOTMENU_H
#define ROBOTMENU_H
#include <stdint.h>
#include <stdio.h>

#include "pros/screen.hpp"
#include "drive.h"
#include "auton.h"

#define MENUPRESS(buttonName) (mCurrentFrame->buttons.buttonName == true && mPreviousFrame->buttons.buttonName == false)
#define PRINT(line, format, ...) pros::screen::print(pros::text_format_e_t::E_TEXT_MEDIUM, line, format, __VA_ARGS__);

class Menu {
public:
    input_frame mInputFrame1;
    input_frame mInputFrame2;
    input_frame* mCurrentFrame;
    input_frame* mPreviousFrame;

    bool runMainMenu();
    void runScriptsMenu();
    int runMacrosMenu();
    void runVisionMenu();
    void runMotorMenu();
    void runMacroRecordMenu();
    int runMacroViewMenu(const char* macroName);
    void runMotorConfigMenu(Motor* motor);
    void handleInput();
    void scrollTest();
    void runDistanceTest();
    void getUserString(const char* message, char* buffer,size_t bufferMaxSize);
    FILE* getDirectoryFile(const char* directoryPath);
    void removeMacroFile(const char* macroName,const char* fileName);
    static void setupScreen();
    static void clearScreen();
    static void wait();
    Menu();
};

#endif