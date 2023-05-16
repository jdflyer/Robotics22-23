#include "robotmenu.h"

Menu::Menu()
{
    mCurrentFrame = &mInputFrame1;
    mPreviousFrame = &mInputFrame2;
    handleInput(); // Fill both input frames with current data
    handleInput();
    setupScreen();
    clearScreen();
}

void Menu::handleInput()
{
    input_frame *temp = mCurrentFrame;
    mCurrentFrame = mPreviousFrame;
    mPreviousFrame = temp;
    populateInputFrame(mCurrentFrame);
}

void Menu::setupScreen()
{
    pros::screen::set_eraser(COLOR_BLACK);
    pros::screen::set_pen(COLOR_WHITE);
}

void Menu::clearScreen()
{
    pros::screen::erase();
}

void Menu::wait()
{
    pros::delay(30);
}

FILE *Menu::getDirectoryFile(const char *directoryPath)
{
    FILE *directoryList = fopen(directoryPath, "r");
    if (directoryList == nullptr)
    {
        directoryList = fopen(directoryPath, "w");
        if (directoryList == nullptr)
        {
            clearScreen();
            PRINT(0, "Sd Card Failed :(", 0);
            while (true)
            {
                handleInput();
                if (MENUPRESS(bPressed))
                {
                    return nullptr;
                }
                wait();
            }
        }
        else
        {
            fclose(directoryList);
            directoryList = fopen(directoryPath, "r");
        }
    }
    return directoryList;
}

static char *readDirectoryBuffer(FILE *directoryList, int *directoryLength)
{
    fseek(directoryList, 0, SEEK_END);
    size_t directoryFileSize = ftell(directoryList);
    rewind(directoryList);
    if (directoryFileSize == 0)
    {
        *directoryLength = 0;
        return nullptr;
    }
    char *directoryFileBuffer = (char *)malloc(directoryFileSize);
    fread(directoryFileBuffer, directoryFileSize, 1, directoryList);
    if (directoryFileBuffer[0] == '\0')
    {
        directoryFileBuffer = directoryFileBuffer + 1;
    }

    for (int i = 0; i < directoryFileSize; i++)
    {
        if (directoryFileBuffer[i] == '\n')
        {
            *directoryLength = *directoryLength + 1;
            directoryFileBuffer[i] = '\0';
        }
    }
    return directoryFileBuffer;
}

static void updateDirectoryScreen(int directoryLength, char *directoryFileBuffer)
{
    int offset = 0;
    for (int i = 0; i < directoryLength; i++)
    {
        PRINT(i + 1, " %s", directoryFileBuffer + offset);
        while (true)
        {
            offset++;
            if (directoryFileBuffer[offset] == '\0')
            {
                offset++;
                break;
            }
        }
    }
}

void Menu::runMacroRecordMenu()
{
    clearScreen();

    int cursorPos = 0;
    bool updateMenu = true;
    int macroLength = 15; // In seconds
    char name[30];
    name[0] = '\0';
    bool recordMotors = false;

    uint8_t *macroData = nullptr;
    size_t macroDataSize = 0;

    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            if (macroData != nullptr)
            {
                free(macroData);
                macroData = nullptr;
            }
            break;
        }

        if (MENUPRESS(upPressed) && cursorPos != 0)
        {
            cursorPos = cursorPos - 1;
            updateMenu = true;
        }
        else if (MENUPRESS(downPressed) && cursorPos != 6)
        {
            cursorPos = cursorPos + 1;
            updateMenu = true;
        }

        if (MENUPRESS(aPressed))
        {
            switch (cursorPos)
            {
            case 0:
                getUserString("Select Macro File Name", name, 30);
                updateMenu = true;
                break;
            case 1:
                if (macroLength == 15)
                {
                    macroLength = 60;
                }
                else
                {
                    macroLength = 15;
                }
                updateMenu = true;
                break;
            case 2:
                recordMotors = !recordMotors;
                updateMenu = true;
                break;
            case 3:
                clearScreen();
                if (macroData != nullptr)
                {
                    free(macroData);
                    macroData = nullptr;
                }
                macroData = autonRecordMacroFile(&macroDataSize, macroLength, recordMotors);
                cursorPos = 4;
                updateMenu = true;
                break;
            case 4:
                clearScreen();
                PRINT(0, "Driving... (B to exit)", 0);
                driveRobot(true);
                handleInput();
                handleInput();
                cursorPos = 3;
                updateMenu = true;
                break;
            case 5:
                if (macroData != nullptr) {
                    clearScreen();
                    PRINT(0,"Testing...",0);
                    autonPlayMacro(macroData);
                    updateMenu = true;
                }
                break;
            case 6:
                if (strlen(name) != 0 && macroData != nullptr)
                {
                    // save macro and append _macroFiles.txt

                    std::string fileName("/usd/");
                    fileName = fileName + name;
                    FILE *macroFile = fopen(fileName.c_str(), "w");
                    fwrite(macroData, macroDataSize, 1, macroFile);
                    fclose(macroFile);

                    FILE *macroDirList = fopen("/usd/_MacroFiles.txt", "a");
                    fprintf(macroFile, "%s\n", name);
                    fclose(macroDirList);

                    free(macroData);
                    macroData = nullptr;
                    return;
                }
            }
        }

        if (updateMenu)
        {
            clearScreen();
            PRINT(0, " File Name: %s", name);
            PRINT(1, " Length: %d", macroLength);
            char recordMotorChar = ' ';
            if (recordMotors)
            {
                recordMotorChar = 'X';
            }
            PRINT(2, " Record Motors [%c]", recordMotorChar);
            if (macroData == nullptr)
            {
                PRINT(3, " Record", 0);
            }
            else
            {
                PRINT(3, " Re-Record", 0);
            }
            PRINT(4, " Drive", 0);
            PRINT(5, " Test",0);
            PRINT(6, " Confirm", 0);
            PRINT(cursorPos, ">", 0);
            updateMenu = false;
        }

        wait();
    }

    clearScreen();
}

void Menu::removeMacroFile(const char *macroName, const char *fileName)
{
    FILE *directoryList = getDirectoryFile("/usd/_MacroFiles.txt");
    if (directoryList == nullptr)
    {
        return;
    }

    int directoryLength = 0;
    char *directoryFileBuffer = readDirectoryBuffer(directoryList, &directoryLength);
    fclose(directoryList);
    directoryList = fopen("/usd/_MacroFiles.txt", "w");

    int offset = 0;
    for (int i = 0; i < directoryLength; i++)
    {
        if (strcmp(macroName, directoryFileBuffer + offset) != 0)
        {
            fprintf(directoryList, "%s\n", directoryFileBuffer + offset);
        }
        while (true)
        {
            offset++;
            if (directoryFileBuffer[offset] == '\0')
            {
                offset++;
                break;
            }
        }
    }

    fclose(directoryList);

    remove(fileName);
}

int Menu::runMacroViewMenu(const char *macroName)
{
    // If return is true exit the macro list menu
    clearScreen();

    std::string fileName("/usd/");
    fileName = fileName + macroName;
    FILE *macroFile = fopen(fileName.c_str(), "r");
    if (macroFile == nullptr)
    {
        PRINT(0, "File Read Failed!", 0);
        while (true)
        {
            handleInput();
            if (MENUPRESS(bPressed))
            {
                return 0;
            }
            wait();
        }
    }

    MacroHeader header;
    fread(&header, sizeof(MacroHeader), 1, macroFile);
    fclose(macroFile);

    {
        char *ident = header.ident;
        bool error = false;
        if (ident[0] != 'M' && ident[1] != 'A' && ident[2] != 'C' && ident[3] != 'R')
        {
            PRINT(0, "File Identifier Invalid!", 0);
            error = true;
        }
        if (header.version != MACRO_VERSION)
        {
            PRINT(2, "Version Mismatch!", 0);
            PRINT(3, "Current is %d!", MACRO_VERSION);
            PRINT(4, "File Has %d!", header.version);
            error = true;
        }
        if (error)
        {
            while (true)
            {
                handleInput();
                if (MENUPRESS(bPressed))
                {
                    clearScreen();
                    return 0;
                }
                wait();
            }
        }
    }

    if (header.recordMotors && header.numMotors != MOTOR_NUM)
    {
        PRINT(0, "Warn: Number of Motors don't match!", 0);
        while (true)
        {
            handleInput();
            if (MENUPRESS(bPressed))
            {
                clearScreen();
                break;
            }
            wait();
        }
    }

    int cursorPos = 0;
    bool updateScreen = true;

    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            break;
        }

        if (MENUPRESS(upPressed) && cursorPos != 0)
        {
            cursorPos = cursorPos - 1;
            updateScreen = true;
        }
        else if (MENUPRESS(downPressed) && cursorPos != 2)
        {
            cursorPos = cursorPos + 1;
            updateScreen = true;
        }

        if (MENUPRESS(aPressed))
        {
            switch (cursorPos)
            {
            case 0:
                // Set as current auton
                autonSetMacro(fileName.c_str());
                return 1;
            case 1:
                // Test Run
                autonRunMacroInstant(fileName.c_str());
                updateScreen = true;
                break;
            case 2:
                // Delete
                PRINT(8, "Are you sure?? (a/b)", 0);
                while (true)
                {
                    handleInput();
                    if (MENUPRESS(aPressed))
                    {
                        removeMacroFile(macroName, fileName.c_str());
                        clearScreen();
                        return 2;
                    }
                    if (MENUPRESS(bPressed))
                    {
                        updateScreen = true;
                        break;
                    }
                    wait();
                }
            }
        }

        if (updateScreen)
        {
            clearScreen();
            PRINT(0, "Macro: %s", macroName);
            PRINT(1, "Time: %d Seconds", header.numFrames / 100);
            char motorChar = ' ';
            if (header.recordMotors)
            {
                motorChar = 'X';
                PRINT(3, "Num Motors: %d", header.numMotors);
            }
            PRINT(2, "Motor Recording [%c]", motorChar);

            PRINT(4, " Set As Auton", 0);
            PRINT(5, " Test Run", 0);
            PRINT(6, " Delete Macro", 0);
            PRINT(cursorPos + 4, ">", 0);
            updateScreen = false;
        }
        wait();
    }
    clearScreen();
    return 0;
}

int Menu::runMacrosMenu()
{
    clearScreen();

    FILE *directoryList = getDirectoryFile("/usd/_MacroFiles.txt");
    if (directoryList == nullptr)
    {
        return false;
    }

    int directoryLength = 0;
    char *directoryFileBuffer = readDirectoryBuffer(directoryList, &directoryLength);

    int exit = 0;

    int cursorPos = 0;
    bool updateScreen = true;
    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            break;
        }

        if (MENUPRESS(upPressed) && cursorPos != 0)
        {
            cursorPos = cursorPos - 1;
            updateScreen = true;
        }
        else if (MENUPRESS(downPressed) && cursorPos != directoryLength)
        {
            cursorPos = cursorPos + 1;
            updateScreen = true;
        }

        if (MENUPRESS(aPressed))
        {
            if (cursorPos == 0)
            {
                runMacroRecordMenu();
                break;
            }
            else
            {
                int offset = 0;
                for (int i = 0; i < directoryLength; i++)
                {
                    if (i + 1 == cursorPos)
                    {
                        exit = runMacroViewMenu(directoryFileBuffer + offset);
                        if (exit)
                        {
                            goto cleanup;
                        }
                        updateScreen = true;
                        break;
                    }
                    while (true)
                    {
                        offset++;
                        if (directoryFileBuffer[offset] == '\0')
                        {
                            offset++;
                            break;
                        }
                    }
                }
            }
        }

        if (updateScreen)
        {
            PRINT(0, " Record New Macro", 0);
            updateDirectoryScreen(directoryLength, directoryFileBuffer);
            PRINT(cursorPos, ">", 0);
            updateScreen = false;
        }

        wait();
    }

cleanup:
    free(directoryFileBuffer);
    fclose(directoryList);
    return exit;
}

void Menu::runScriptsMenu()
{
    clearScreen();

    FILE *directoryList = getDirectoryFile("/usd/_ScriptFiles.txt");
    if (directoryList == nullptr)
    {
        return;
    }

    int directoryLength = 0;
    char *directoryFileBuffer = readDirectoryBuffer(directoryList, &directoryLength);

    int cursorPos = 0;
    bool updateScreen = true;
    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            break;
        }

        if (MENUPRESS(upPressed) && cursorPos != 0)
        {
            cursorPos = cursorPos - 1;
            updateScreen = true;
        }
        else if (MENUPRESS(downPressed) && cursorPos != directoryLength - 1)
        {
            cursorPos = cursorPos + 1;
            updateScreen = true;
        }

        if (updateScreen)
        {
            PRINT(0, " Create New Script", 0);
            updateDirectoryScreen(directoryLength, directoryFileBuffer);
            PRINT(cursorPos, ">", 0);
            updateScreen = false;
        }

        wait();
    }

    free(directoryFileBuffer);
    fclose(directoryList);
}

void Menu::runMotorConfigMenu(Motor *motor)
{
    clearScreen();
    int cursorPos = 0;
    bool updateMenu = true;
    bool changePortNum = false;

    while (true)
    {
        handleInput();
        if (changePortNum == false)
        {
            if (MENUPRESS(bPressed))
            {
                motor->restartMotor();
                break;
            }

            if (MENUPRESS(upPressed) && cursorPos != 0)
            {
                cursorPos = cursorPos - 1;
                updateMenu = true;
            }
            else if (MENUPRESS(downPressed) && cursorPos != 1)
            {
                cursorPos = cursorPos + 1;
                updateMenu = true;
            }

            if (MENUPRESS(aPressed))
            {
                if (cursorPos == 0)
                {
                    changePortNum = true;
                }
                else
                {
                    motor->mReversed = !motor->mReversed;
                    updateMenu = true;
                }
            }
        }
        else
        {
            if (MENUPRESS(bPressed))
            {
                changePortNum = false;
            }

            if (MENUPRESS(downPressed) && motor->mPortNumber != 1)
            {
                motor->mPortNumber = motor->mPortNumber - 1;
                updateMenu = true;
            }
            else if (MENUPRESS(upPressed) && motor->mPortNumber != 21)
            {
                motor->mPortNumber = motor->mPortNumber + 1;
                updateMenu = true;
            }
        }

        if (updateMenu)
        {
            clearScreen();
            PRINT(0, "%s", motor->mName);
            PRINT(1, " Port: %d", motor->mPortNumber);
            const char *reversed = "false";
            if (motor->mReversed)
            {
                reversed = "true";
            }
            PRINT(2, " Reversed: %s", reversed);
            PRINT(cursorPos + 1, ">", 0);
            updateMenu = false;
        }
        wait();
    }
}

void Menu::scrollTest()
{
    clearScreen();
    int cursorPos = 0;
    int startDisplayLine = 0;
    bool updateMenu = true;

    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            break;
        }

        if (MENUPRESS(upPressed))
        {
            cursorPos = cursorPos - 1;
            if (cursorPos < startDisplayLine)
            {
                startDisplayLine = cursorPos;
            }
            updateMenu = true;
        }
        else if (MENUPRESS(downPressed))
        {
            cursorPos = cursorPos + 1;
            if (cursorPos > startDisplayLine + 11)
            {
                startDisplayLine = cursorPos - 11;
            }
            updateMenu = true;
        }

        if (MENUPRESS(aPressed))
        {
        }

        if (updateMenu)
        {
            clearScreen();
            for (int i = 0; i < 12; i++)
            {
                PRINT(i, " %d", i + startDisplayLine);
            }
            PRINT(cursorPos - startDisplayLine, ">", 0);
        }

        wait();
    }
}

void Menu::runMotorMenu()
{
    clearScreen();
    int cursorPos = 0;
    bool updateMenu = true;

    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            Motor::saveMotorsToFile();
            break;
        }

        if (MENUPRESS(upPressed) && cursorPos != 0)
        {
            cursorPos = cursorPos - 1;
            updateMenu = true;
        }
        else if (MENUPRESS(downPressed) && cursorPos < MOTOR_NUM - 1)
        {
            cursorPos = cursorPos + 1;
            updateMenu = true;
        }

        if (MENUPRESS(aPressed))
        {
            runMotorConfigMenu(motors[cursorPos]);
            clearScreen();
            updateMenu = true;
        }

        if (updateMenu)
        {
            for (int i = 0; i < MOTOR_NUM; i++)
            {
                PRINT(i, " %s (%d)", motors[i]->mName, motors[i]->mPortNumber);
            }
            PRINT(cursorPos, ">", 0);
        }

        wait();
    }
}

void Menu::getUserString(const char *message, char *buffer, size_t bufferMaxSize)
{
    static const char *upperCaseKeyboard = "!@#$5^&*()QWERTYUIOPASDFGHJKL:ZXCVBNM<>?";
    static const char *lowerCaseKeyboard = "1234567890qwertyuiopasdfghjkl;zxcvbnm,./";

    memset(buffer, '_', bufferMaxSize);
    buffer[bufferMaxSize - 1] = 0;

    const int rowLength = 10;

    int cursorPos = 0;
    int bufferPos = 0;
    bool update = true;
    bool caps = false;

    while (true)
    {
        handleInput();
        if (MENUPRESS(xPressed))
        {
            buffer[bufferPos] = '\0';
            break;
        }

        if (MENUPRESS(yPressed))
        {
            caps = !caps;
            update = true;
        }

        const char *Keyboard = lowerCaseKeyboard;
        if (caps)
        {
            Keyboard = upperCaseKeyboard;
        }

        if (MENUPRESS(bPressed) && bufferPos != 0)
        {
            bufferPos--;
            buffer[bufferPos] = '_';
            update = true;
        }

        if (MENUPRESS(aPressed) && bufferPos != bufferMaxSize - 1)
        {
            buffer[bufferPos] = Keyboard[cursorPos];
            bufferPos++;
            update = true;
        }

        if (MENUPRESS(upPressed) && cursorPos - 10 >= 0)
        {
            cursorPos = cursorPos - 10;
            update = true;
        }
        else if (MENUPRESS(downPressed) && cursorPos + 10 < 40)
        {
            cursorPos = cursorPos + 10;
            update = true;
        }
        else if (MENUPRESS(rightPressed) && cursorPos != 39)
        {
            cursorPos = cursorPos + 1;
            update = true;
        }
        else if (MENUPRESS(leftPressed) && cursorPos != 0)
        {
            cursorPos = cursorPos - 1;
            update = true;
        }

        if (update)
        {
            clearScreen();
            for (int i = 0; i < 40; i++)
            {
                PRINT(((i % 10) * 20) + 150, ((i / 10) * 20) + 100, "%c", Keyboard[i]);
            }

            PRINT(((cursorPos % 10) * 20) + 140, ((cursorPos / 10) * 20) + 100, ">", 0);

            PRINT(0, "%s", message);
            PRINT(1, "%s", buffer);
            PRINT(2, "Press A For Letter, B for Backspace", 0);
            PRINT(3, "Press X For Confirm, Y For Caps", 0);
            update = false;
        }

        wait();
    }
}

void Menu::runVisionMenu()
{
    int cursorPos = 0;
    bool updateMenu = true;

    while (true)
    {
        handleInput();

        if (MENUPRESS(bPressed))
        {
            break;
        }
        #ifdef TEAM2
        if (updateMenu)
        {
            // pros::delay(1000);
            clearScreen();
            // SIGNATURE BLUE_GOAL (1,-2443,-63,-1253,2029,10445,6237,0.800,0);
            // SIGNATURE RED_GOAL (2,1543,5439,3491,-903,461,-221,1.000,0);
            pros::vision_object_s_t object = visionSensor.get_by_size(0);
            if (errno == EDOM)
            {
                PRINT(0, "Error", 0);
            }
            PRINT(0, "Signature: %03d", object.signature);
            PRINT(1, "Angle: %d", object.angle);
            PRINT(2, "Width: %03d", object.width);
            PRINT(3, "Height: %03d", object.height);
            PRINT(4, "Pos: (%03d,%03d)", object.x_middle_coord, object.y_middle_coord);
            // updateMenu = false;
        }
        #endif

        wait();
    }
}

void Menu::runDistanceTest()
{
    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            break;
        }
        #ifdef TEAM2

        int32_t leftDistance = leftDistanceSensor.get();
        int32_t backDistance = backDistanceSensor.get();

        PRINT(0, "Left Sensor Distance: %04d mm", leftDistance)
        PRINT(1, "Back Sensor Distance: %04d mm", backDistance)

        double angle = intertialSensor.get_heading();
        PRINT(2, "Angle: %f", angle);

        if (MENUPRESS(aPressed) && leftDistance != 9999 && backDistance != 9999)
        {

            const double distanceFromGoalToBackWall = 3200.4; // 126 inches; 3200.4mm
            const double distanceFromSideWallToGoal = 457.2;  // 18 inches; 457.2mm
            const double leftDistanceAdjust = 228.6;               // distance from left sensor to shooter
            const double backDistanceAdjust = 381.0;               // distance from back sensor to shooter

            
            // find nearest parallel angle
            double nearestAngle = round(angle / 90.0) * 90.0;
            PRINT(3, "Parallel Angle: %f", nearestAngle);

            double theta = fabs(angle - nearestAngle);
            PRINT(4, "Angle to Use to get real values %f", theta);

            double leftDistanceTrue = ((double)leftDistance) * cos(theta * (M_PI / 180.0));
            double backDistanceTrue = ((double)backDistance) * cos(theta * (M_PI / 180.0));
            leftDistanceTrue = leftDistanceTrue+leftDistanceAdjust-distanceFromSideWallToGoal;
            backDistanceTrue = distanceFromGoalToBackWall-(backDistanceTrue+backDistanceAdjust);

            double distanceFromGoal = sqrt((leftDistanceTrue*leftDistanceTrue)+(backDistanceTrue*backDistanceTrue));


            PRINT(5, "Left Distance True: %f", leftDistanceTrue);
            PRINT(6, "Back Distance True: %f", backDistanceTrue);
            PRINT(7, "Distance From Goal: %f",distanceFromGoal);
            PRINT(8, "Angle To Goal %f",atan(backDistanceTrue/leftDistanceTrue)*(M_PI/180.0));

        }
        #endif

        wait();
    }
}

bool Menu::runMainMenu()
{
    int cursorPos = 0;
    bool updateMenu = true;

    while (true)
    {
        handleInput();
        if (MENUPRESS(bPressed))
        {
            break;
        }

        if (MENUPRESS(upPressed) && cursorPos != 0)
        {
            cursorPos = cursorPos - 1;
            updateMenu = true;
        }
        else if (MENUPRESS(downPressed) && cursorPos != 7)
        {
            cursorPos = cursorPos + 1;
            updateMenu = true;
        }

        if (updateMenu)
        {
            clearScreen();
            PRINT(0, " Scripts", 0);
            PRINT(1, " Macros", 0);
            PRINT(2, " Motor Configuration", 0);
            PRINT(3, " Scroll Test", 0);
            PRINT(4, " Input Test", 0);
            PRINT(5, " Vision Menu", 0);
            PRINT(6, " Distance Test", 0);
            PRINT(7, " Speaker Test",0);
            // There is 12 rows of characters availiable
            PRINT(cursorPos, ">", 0);
            updateMenu = false;
        }

        int exit = 0;

        if (MENUPRESS(aPressed))
        {
            switch (cursorPos)
            {
            case 0:
                clearScreen();
                runScriptsMenu();
                updateMenu = true;
                break;
            case 1:
                exit = runMacrosMenu();
                if (exit == 1)
                {
                    return true;
                }
                updateMenu = true;
                break;
            case 2:
                runMotorMenu();
                clearScreen();
                updateMenu = true;
                break;
            case 3:
                scrollTest();
                clearScreen();
                updateMenu = true;
                break;
            case 4:
            {
                char inputBuffer[20];
                getUserString("Input Test", inputBuffer, 20);
            }
                updateMenu = true;
                break;
            case 5:
                clearScreen();
                runVisionMenu();
                updateMenu = true;
                break;
            case 6:
                clearScreen();
                runDistanceTest();
                updateMenu = true;
                break;
            case 7:
                uint8_t val = 0;
                while(true) {
                    if (val == 0) {
                        val = 1;
                    }else{
                        val = 0;
                    }
                    digitalOut.set_value(val);
                    pros::delay(7);
                }
                break;
            }
        }

        wait();
    }

    clearScreen();
    return false;
}