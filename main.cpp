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

enum TestMode
{
    TESTMODE_NONE = 0x0,
    TESTMODE_PROGRAMMER = 0x1,
    TESTMODE_CALL = 0x2,
};

int gTestMode;

int gFlags = 0;

char gStartLevel[256];

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

char* getCmdArg(char* s1, char* s2)
{
    char* result;
    unsigned int len1;
    unsigned int len2;
    unsigned int i;

    result = s1;
    len1 = strlen(s1) + 1;
    len2 = strlen(s2) + 1;

    if (len1 == 1 || len2 == 1)
        return 0;

    i = 0;
    while (_strnicmp(result, s2, len2 - 1))
    {
        ++i;
        ++result;
        if (i >= len1 - 1)
            return 0;
    }

    return result;
}

void processCmdArgs(char* argBuffer, bool *noSound, bool *requireCD)
{
    if (getCmdArg(argBuffer, "-insistOnCD"))
        *requireCD = true;
    if (getCmdArg(argBuffer, "-nosound"))
        *noSound = true;

    if (getCmdArg(argBuffer, "-debug"))
        gGameFlags |= GFLAG_DEBUG;
    if (getCmdArg(argBuffer, "-nm"))
        gGameFlags |= GFLAG_NM;
    if (getCmdArg(argBuffer, "-ftm"))
        gGameFlags |= GFLAG_FTM;
    if (getCmdArg(argBuffer, "-fvf"))
        gGameFlags |= GFLAG_FVF;

    if (getCmdArg(argBuffer, "-best"))
        gGameFlags |= GFLAG_BEST;
    if (getCmdArg(argBuffer, "-window"))
        gGameFlags |= GFLAG_WINDOW;

    if (getCmdArg(argBuffer, "-dualmouse"))
        gGameFlags |= GFLAG_DUALMOUSE;

    if (getCmdArg(argBuffer, "-debugcomplete"))
        gGameFlags |= GFLAG_DEBUG_COMPLETE;

    if (getCmdArg(argBuffer, "-testercall"))
    {
        gTestMode = TESTMODE_CALL;
        gGameFlags |= GFLAG_TESTER_CALL;
    }

    if (getCmdArg(argBuffer, "-testlevels"))
        gGameFlags |= GFLAG_TEST_LEVELS;

    if (getCmdArg(argBuffer, "-reducesamples"))
        gGameFlags |= GFLAG_REDUCE_SAMPLES;

    if (getCmdArg(argBuffer, "-showversion"))
        gGameFlags |= GFLAG_SHOW_VERSION;

    if (getCmdArg(argBuffer, "-reduceanimation"))
        gGameFlags |= GFLAG_REDUCE_ANIMATION;

    if (getCmdArg(argBuffer, "-reducepromeshes"))
        gGameFlags |= GFLAG_REDUCE_PROMESHES;

    if (getCmdArg(argBuffer, "-reduceflics"))
        gGameFlags |= GFLAG_REDUCE_FLICS;

    if (getCmdArg(argBuffer, "-reduceimages"))
        gGameFlags |= GFLAG_REDUCE_IMAGES;

    char* startlvl = getCmdArg(argBuffer, "-startlevel");
    if (startlvl)
    {
        char* v10 = startlvl + 11;
        int v11 = 0;
        char i;
        for (i = startlvl[11]; i; i = *++v10)
        {
            if (i != ' ')
                break;
        }

        char j;
        for (j = *v10; j; ++v10)
        {
            if (j == ' ')
                break;
            gStartLevel[v11] = j;
            j = v10[1];
            ++v11;
        }
        gStartLevel[v11] = 0;
        gGameFlags |= GFLAG_START_LEVEL;
    }

    char* flags = getCmdArg(argBuffer, "-flags");
    if (flags)
        gFlags = atoi(flags + 6);

    char* fpslock = getCmdArg(argBuffer, "-fpslock");
    if (fpslock)
    {
        int fps = atoi(fpslock + 8);
        if (fps)
            gTargetDelta = 25.0f / (double)(unsigned int)fps;
    }

    char* programmer = getCmdArg(argBuffer, "-programmer");
    if (programmer)
    {
        gTestMode = atoi(programmer + 11);
        if (!gTestMode)
            gTestMode = TESTMODE_PROGRAMMER;
    } else {
        gTestMode = TESTMODE_NONE;
    }

    if (getCmdArg(argBuffer, "-cleansaves"))
        gGameFlags |= GFLAG_CLEAN_SAVES;
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
