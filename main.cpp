#include <Windows.h>
#include <iostream>

#include "registry.h"
#include "input.h"
#include "sound.h"
#include "directdraw.h"
#include "debug.h"

void processCmdArgs(char* argBuffer, bool *noSound, bool *requireCD);
void loadGameData(const char* gamePath, bool requireCD, const char* regkey);
bool createWindow(HINSTANCE hInstance);
void init_some_vars(HWND window);
bool initDisplayParams(int a1, bool debug, bool best, bool window, LPCSTR lpText);

const char* gMutexName = "Lego Rock Raiders";
char gGamePath[256];
const char* lpClassName;
bool gHasFocus;
bool gStopGame;
bool gInitializedGameFunctions;
HINSTANCE hInstance;
float gTargetDelta;
HWND hWndParent;

const char* root_regkey = "SOFTWARE\\LEGO Media\\Games\\Rock Raiders";

enum GameFlags
{
    GFLAG_NONE = 0x0,
    GFLAG_NM = 0x20,
    GFLAG_BEST = 0x40,
    GFLAG_WINDOW = 0x100,
    GFLAG_START_LEVEL = 0x200,
    GFLAG_CLEAN_SAVES = 0x400,
    GFLAG_DEBUG = 0x2000,
    GFLAG_DUALMOUSE = 0x4000,
    GFLAG_DEBUG_COMPLETE = 0x8000,
    GFLAG_TESTER_CALL = 0x10000,
    GFLAG_TEST_LEVELS = 0x20000,
    GFLAG_FTM = 0x40000,
    GFLAG_FVF = 0x80000,
    GFLAG_REDUCE_SAMPLES = 0x100000,
    GFLAG_SHOW_VERSION = 0x200000,
    GFLAG_REDUCE_ANIMATION = 0x400000,
    GFLAG_REDUCE_PROMESHES = 0x800000,
    GFLAG_REDUCE_FLICS = 0x1000000,
    GFLAG_REDUCE_IMAGES = 0x2000000,
};

int gGameFlags;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    bool noSound = false;
    bool requireCD = false;

    char name[128];
    if (!gMutexName || (sprintf(name, "%s Mutex", gMutexName),
                        CreateMutex(nullptr, true, name),
                        GetLastError() != ERROR_ALREADY_EXISTS))
    {
        LPSTR cmd = GetCommandLine();
        char c = *cmd;
        char* i;
        for (i = cmd; c; ++cmd)
        {
            if (c == '\\')
                i = cmd + 1;
            c = cmd[1];
        }

        strcpy(gGamePath, i);
        char* it = gGamePath;
        if (gGamePath[0])
        {
            do
            {
                if (*it == '"')
                    *it = 0;
            } while (*++it);
        }

        c = gGamePath[0];
        it = gGamePath;
        for (i = gGamePath; c; ++it)
        {
            if (c == '.')
                i = it + 1;
            c = it[1];
        }

        if (i != gGamePath)
        {
            _strupr(i);
            char* s = strstr(gGamePath, ".EXE");
            if (s)
                *s = 0;
        }
        lpClassName = gGamePath;
        gHasFocus = false;
        gStopGame = false;
        gInitializedGameFunctions = false;
        ::hInstance = hInstance;
        gTargetDelta = 0.0f;
        gGameFlags = GFLAG_NONE;

        BYTE Data[1024];
        char Buffer[1024];
        if (getOrCreateRegKeyLM(root_regkey, "StandardParameters", 0, Data, 1024))
            sprintf(Buffer, "%s %s", lpCmdLine, Data);
        else
            sprintf(Buffer, "%s", lpCmdLine);
        processCmdArgs(Buffer, &noSound, &requireCD);
        BYTE NoHALMessage[1024]; // No Hardware Acceleration Message
        if (!getOrCreateRegKeyLM(root_regkey, "NoHALMessage", 0, NoHALMessage, 1024))
            sprintf((char*)NoHALMessage, "No DirectX 3D accelerator could be found.");

        // sub_48B260();
        // sub_4893F0();
        loadGameData(gGamePath, requireCD, root_regkey);
        // sub_478DF0();
        initDirectInput();
        if (createWindow(hInstance))
        {
            init_some_vars(hWndParent);
            if (initSound(noSound))
            {
                if (initDisplayParams(1,
                                      gGameFlags & GFLAG_DEBUG,
                                      gGameFlags & GFLAG_BEST,
                                      gGameFlags & GFLAG_WINDOW,
                                      (LPCSTR)NoHALMessage))
                {
                    // TODO: Finish Implementing WinMain
                }
            }
            closeDirectDraw();
            DestroyWindow(hWndParent);
        }

        closeDirectInput();
        // sub_478E20();
        PrintErrors();
        // sub_48B290();
        // sub_48B2F0();
    }

    return 0;
}

void processCmdArgs(char* argBuffer, bool *noSound, bool *requireCD)
{
    // TODO: Implement processCmdArgs
}

void loadGameData(const char* gamePath, bool requireCD, const char* regkey)
{
    // TODO: Implement loadGameData
}

bool createWindow(HINSTANCE hInstance)
{
    // TODO: Implement createWindow
    return false;
}

void init_some_vars(HWND window)
{
    // TODO: Implement init_some_vars
}

bool initDisplayParams(int a1, bool debug, bool best, bool window, LPCSTR lpText)
{
    // TODO: Implement initDisplayParams

    return false;
}
