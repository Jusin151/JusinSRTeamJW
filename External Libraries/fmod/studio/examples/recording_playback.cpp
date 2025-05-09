﻿/*==============================================================================
API Recording Example
Copyright (c), Firelight Technologies Pty, Ltd 2012-2025.

This example shows recording and playback functionality, allowing the user to
trigger some sounds and then play back what they have recorded.  The provided
functionality is intended to assist in debugging.

For information on using FMOD example code in your own programs, visit
https://www.fmod.com/legal
==============================================================================*/
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "common.h"

const int SCREEN_WIDTH = NUM_COLUMNS;
const int SCREEN_HEIGHT = 10;

int currentScreenPosition = -1;
char screenBuffer[(SCREEN_WIDTH + 1) * SCREEN_HEIGHT + 1] = {0};

void initializeScreenBuffer();
void updateScreenPosition(const FMOD_VECTOR& worldPosition);

static const char* RECORD_FILENAME = "playback.cmd.txt";

enum State
{
    State_Selection,
    State_Record,
    State_Playback,
    State_Quit
};

State executeSelection(FMOD::Studio::System* system);
State executeRecord(FMOD::Studio::System* system);
State executePlayback(FMOD::Studio::System* system);
    
int FMOD_Main()
{
    void *extraDriverData = 0;
    Common_Init(&extraDriverData);

    FMOD::Studio::System* system = NULL;
    ERRCHECK( FMOD::Studio::System::create(&system) );

    // The example Studio project is authored for 5.1 sound, so set up the system output mode to match
    FMOD::System* coreSystem = NULL;
    ERRCHECK( system->getCoreSystem(&coreSystem) );
    ERRCHECK( coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0) );

    ERRCHECK( system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData) );

    State state = State_Selection;
    while (state != State_Quit)
    {
        switch (state)
        {
            case State_Selection:
                state = executeSelection(system);
                break;
            case State_Record:
                state = executeRecord(system);
                break;
            case State_Playback:
                state = executePlayback(system);
                break;
            case State_Quit:
                break;
        };
    };

    ERRCHECK( system->release() );

    Common_Close();

    return 0;
}

// Show the main selection menu
State executeSelection(FMOD::Studio::System* system)
{
    for (;;)
    {
        Common_Update();

        if (Common_BtnPress(BTN_ACTION1))
        {
            return State_Record;
        }
        if (Common_BtnPress(BTN_ACTION2))
        {
            return State_Playback;
        }
        if (Common_BtnPress(BTN_QUIT))
        {
            return State_Quit;
        }

        ERRCHECK( system->update() );
        
        Common_Draw("==================================================");
        Common_Draw("Recording and playback example.");
        Common_Draw("Copyright (c) Firelight Technologies 2012-2025.");
        Common_Draw("==================================================");
        Common_Draw("");
        Common_Draw("Waiting to start recording");
        Common_Draw("");
        Common_Draw("Press %s to start recording", Common_BtnStr(BTN_ACTION1));
        Common_Draw("Press %s to play back recording", Common_BtnStr(BTN_ACTION2));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
        Common_Draw("");
        Common_Sleep(50);
    }
}

// Start recording, load banks and then let the user trigger some sounds
State executeRecord(FMOD::Studio::System* system)
{
    FMOD::Studio::Bank* masterBank = NULL;
    ERRCHECK( system->loadBankFile(Common_MediaPath("Master.bank"), FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &masterBank) );

    FMOD::Studio::Bank* stringsBank = NULL;
    ERRCHECK( system->loadBankFile(Common_MediaPath("Master.strings.bank"), FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &stringsBank) );

    FMOD::Studio::Bank* vehiclesBank = NULL;
    ERRCHECK( system->loadBankFile(Common_MediaPath("Vehicles.bank"), FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &vehiclesBank) );

    FMOD::Studio::Bank* sfxBank = NULL;
    ERRCHECK( system->loadBankFile(Common_MediaPath("SFX.bank"), FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &sfxBank) );

    // Wait for banks to load
    ERRCHECK( system->flushCommands() );

    // Start recording commands - it will also record which banks we have already loaded by now
    ERRCHECK( system->startCommandCapture(Common_WritePath(RECORD_FILENAME), FMOD_STUDIO_COMMANDCAPTURE_NORMAL) );

    FMOD_GUID explosionID = {0};
    ERRCHECK( system->lookupID("event:/Weapons/Explosion", &explosionID) );

    FMOD::Studio::EventDescription* engineDescription = NULL;
    ERRCHECK( system->getEvent("event:/Vehicles/Ride-on Mower", &engineDescription) );

    FMOD::Studio::EventInstance* engineInstance = NULL;
    ERRCHECK( engineDescription->createInstance(&engineInstance) );

    ERRCHECK( engineInstance->setParameterByName("RPM", 650.0f) );
    ERRCHECK( engineInstance->start() );

    // Position the listener at the origin
    FMOD_3D_ATTRIBUTES attributes = { { 0 } };
    attributes.forward.z = 1.0f;
    attributes.up.y = 1.0f;
    ERRCHECK( system->setListenerAttributes(0, &attributes) );

    // Position the event 2 units in front of the listener
    attributes.position.z = 2.0f;
    ERRCHECK( engineInstance->set3DAttributes(&attributes) );

    initializeScreenBuffer();
    
    bool wantQuit = false;

    for (;;)
    {
        Common_Update();

        if (Common_BtnPress(BTN_MORE))
        {
            break;
        }

        if (Common_BtnPress(BTN_QUIT))
        {
            wantQuit = true;
            break;
        }

        if (Common_BtnPress(BTN_ACTION1))
        {
            // One-shot event
            FMOD::Studio::EventDescription* eventDescription = NULL;
            ERRCHECK( system->getEventByID(&explosionID, &eventDescription) );

            FMOD::Studio::EventInstance* eventInstance = NULL;
            ERRCHECK( eventDescription->createInstance(&eventInstance) );
            for (int i=0; i<10; ++i)
            {
                ERRCHECK( eventInstance->setVolume(i / 10.0f) );
            }

            ERRCHECK( eventInstance->start() );

            // Release will clean up the instance when it completes
            ERRCHECK( eventInstance->release() );
        }

        if (Common_BtnPress(BTN_LEFT))
        {
            attributes.position.x -= 1.0f;
            ERRCHECK( engineInstance->set3DAttributes(&attributes) );
        }
        
        if (Common_BtnPress(BTN_RIGHT))
        {
            attributes.position.x += 1.0f;
            ERRCHECK( engineInstance->set3DAttributes(&attributes) );
        }
        
        if (Common_BtnPress(BTN_UP))
        {
            attributes.position.z += 1.0f;
            ERRCHECK( engineInstance->set3DAttributes(&attributes) );
        }
        
        if (Common_BtnPress(BTN_DOWN))
        {
            attributes.position.z -= 1.0f;
            ERRCHECK( engineInstance->set3DAttributes(&attributes) );
        }

        if (Common_BtnPress(BTN_MORE))
        {
            break;
        }
        if (Common_BtnPress(BTN_QUIT))
        {
            wantQuit = true;
            break;
        }

        ERRCHECK(system->update());
        
        updateScreenPosition(attributes.position);
        Common_Draw("==================================================");
        Common_Draw("Recording and playback example.");
        Common_Draw("Copyright (c) Firelight Technologies 2012-2025.");
        Common_Draw("==================================================");
        Common_Draw("");
        Common_Draw("Recording!");
        Common_Draw("");
        Common_Draw(screenBuffer);
        Common_Draw("");
        Common_Draw("Press %s to finish recording", Common_BtnStr(BTN_MORE));
        Common_Draw("Press %s to play a one-shot", Common_BtnStr(BTN_ACTION1));
        Common_Draw("Use the arrow keys (%s, %s, %s, %s) to control the engine position", 
            Common_BtnStr(BTN_LEFT), Common_BtnStr(BTN_RIGHT), Common_BtnStr(BTN_UP), Common_BtnStr(BTN_DOWN));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));

        Common_Sleep(50);
    }

    // Unload all the banks
    ERRCHECK( masterBank->unload() );
    ERRCHECK( stringsBank->unload() );
    ERRCHECK( vehiclesBank->unload() );
    ERRCHECK( sfxBank->unload() );

    // Finish recording
    ERRCHECK( system->flushCommands() );
    ERRCHECK( system->stopCommandCapture() );

    return (wantQuit ? State_Quit : State_Selection);
}

// Play back a previously recorded file
State executePlayback(FMOD::Studio::System* system)
{
    FMOD::Studio::CommandReplay* replay;
    ERRCHECK( system->loadCommandReplay(Common_WritePath(RECORD_FILENAME), FMOD_STUDIO_COMMANDREPLAY_NORMAL, &replay));
    int commandCount;
    ERRCHECK(replay->getCommandCount(&commandCount));
    float totalTime;
    ERRCHECK(replay->getLength(&totalTime));
    ERRCHECK(replay->start());
    ERRCHECK(system->update());

    for (;;)
    {
        Common_Update();

        if (Common_BtnPress(BTN_QUIT))
        {
            break;
        }

        if (Common_BtnPress(BTN_MORE))
        {
            bool paused;
            ERRCHECK(replay->getPaused(&paused));
            ERRCHECK(replay->setPaused(!paused));
        }

        FMOD_STUDIO_PLAYBACK_STATE state;
        ERRCHECK(replay->getPlaybackState(&state));
        if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            break;
        }

        int currentIndex;
        float currentTime;
        ERRCHECK(replay->getCurrentCommand(&currentIndex, &currentTime));

        ERRCHECK(system->update());
        
        Common_Draw("==================================================");
        Common_Draw("Recording and playback example.");
        Common_Draw("Copyright (c) Firelight Technologies 2012-2025.");
        Common_Draw("==================================================");
        Common_Draw("");
        Common_Draw("Playing back commands:");
        Common_Draw("Command = %d / %d\n", currentIndex, commandCount);
        Common_Draw("Time = %.3f / %.3f\n", currentTime, totalTime);
        Common_Draw("");
        Common_Draw("Press %s to pause/unpause recording", Common_BtnStr(BTN_MORE));
        Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));

        Common_Sleep(50);
    }


    ERRCHECK( replay->release() );
    ERRCHECK( system->unloadAll() );
    return State_Selection;
}

void initializeScreenBuffer()
{
    memset(screenBuffer, ' ', sizeof(screenBuffer));

    int idx = SCREEN_WIDTH;
    for (int i = 0; i < SCREEN_HEIGHT; ++i)
    {
        screenBuffer[idx] = '\n';
        idx += SCREEN_WIDTH + 1;
    }

    screenBuffer[(SCREEN_WIDTH + 1) * SCREEN_HEIGHT] = '\0';
}

int getCharacterIndex(const FMOD_VECTOR& position)
{
    int row = static_cast<int>(-position.z + (SCREEN_HEIGHT / 2));
    int col = static_cast<int>(position.x + (SCREEN_WIDTH / 2));
    
    if (0 < row && row < SCREEN_HEIGHT && 0 < col && col < SCREEN_WIDTH)
    {
        return (row * (SCREEN_WIDTH + 1)) + col;
    }
    
    return -1;
}

void updateScreenPosition(const FMOD_VECTOR& eventPosition)
{
    if (currentScreenPosition != -1)
    {
        screenBuffer[currentScreenPosition] = ' ';
        currentScreenPosition = -1;
    }

    FMOD_VECTOR origin = {0};
    int idx = getCharacterIndex(origin);
    screenBuffer[idx] = '^';
    
    idx = getCharacterIndex(eventPosition);    
    if (idx != -1)
    {
        screenBuffer[idx] = 'o';
        currentScreenPosition = idx;
    }
}
