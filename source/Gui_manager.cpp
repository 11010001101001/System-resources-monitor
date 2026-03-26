#define RPM " RPM"
#define DEGREES " °C"
#define RTX_2060_MAX_RPM 3069
#define PROCENTS " %"
#define DELAY 3000
#define DIVIDER " / "
#define SPACER "                                   "
#define CRITICAL 80
#define BARLENGTH 10
#define OPEN "["
#define CLOSE "]"
#define BLOCK "█"
#define EMPTY "░"
#define SPACE " "
#define WHITE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define GREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define YELLOW FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define ORANGE FOREGROUND_RED | FOREGROUND_GREEN
#define RED FOREGROUND_RED | FOREGROUND_INTENSITY

#include "gui_manager.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <iomanip>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void GuiManager::hideCursor()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void GuiManager::setBoldFont()
{
    CONSOLE_FONT_INFOEX fontInfo = {sizeof(CONSOLE_FONT_INFOEX)};
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    fontInfo.FontWeight = FW_BOLD;
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
}

void GuiManager::buildTopConsoleWindow()
{
    HWND hwndConsole = GetConsoleWindow();
    RECT desktop;
    const HWND hdesktop = GetDesktopWindow();

    GetWindowRect(hdesktop, &desktop);

    long style = GetWindowLong(hwndConsole, GWL_EXSTYLE);
    style |= WS_EX_TOOLWINDOW;
    style &= ~WS_EX_APPWINDOW;

    SetWindowLong(hwndConsole, GWL_EXSTYLE, style);

    int right = desktop.right;
    int top = desktop.top;
    int width = 330;
    int height = 130;
    int x = right - width + 4;
    int y = top + 32;

    SetWindowPos(
        hwndConsole,
        HWND_TOPMOST,
        x,
        y,
        width,
        height,
        SWP_NOREDRAW
    );

    hideCursor();
    setBoldFont();
    SetConsoleOutputCP(CP_UTF8);
    SendMessage(hwndConsole, WM_SETTEXT, 0, (LPARAM)"Monitor");
}

COORD GuiManager::buildCoord()
{
    COORD coord;
    coord.X = 0;
    coord.Y = 0;
    return coord;
}

void GuiManager::showStats(SystemAnalizer analizer, COORD coord)
{
    string ram = analizer.analize(part::ram);
    string gpu = analizer.analize(part::gpu);
    string gpu_temp = analizer.analize(part::gpu_temp);
    string fan_usage = analizer.analize(part::gpu_fan_speed);
    string cpu = analizer.analize(part::cpu);

    // 'reset' old cout
    SetConsoleCursorPosition(hConsole, coord);

    for (int i = 0; i <= 5; i++)
        cout << SPACER << endl;

    // cout new values with no cls blinking
    SetConsoleCursorPosition(hConsole, coord);

    show(part::cpu, cpu);
    show(part::ram, ram);
    show(part::gpu, gpu);
    show(part::gpu_temp, gpu_temp);
    show(part::gpu_fan_speed, fan_usage);

    checkIsAchtung(ram, gpu, cpu);

    Sleep(DELAY);
}

void GuiManager::show(SystemAnalizer::DevicePart part, string param)
{
    SetConsoleTextAttribute(hConsole, WHITE);

    switch (part)
    {
    case part::cpu:
        cout << " CPU:  " << setw(20);
        SetConsoleTextAttribute(hConsole, getWAttributes(param));
        cout << buildProgressBar(param) << param + PROCENTS << endl;
        break;

    case part::ram:
        cout << " RAM:  " << setw(20);
        SetConsoleTextAttribute(hConsole, getWAttributes(param));
        cout << buildProgressBar(param) << param + PROCENTS << endl;
        break;

    case part::gpu:
        cout << " GPU:  " << setw(20);
        SetConsoleTextAttribute(hConsole, getWAttributes(param));
        cout << buildProgressBar(param) << param + PROCENTS;
        break;

    case part::gpu_temp:
        SetConsoleTextAttribute(hConsole, getWAttributes(param));
        cout << DIVIDER << param + DEGREES << endl;
        break;

    case part::gpu_fan_speed:
        cout << " GFAN: " << setw(19);
        SetConsoleTextAttribute(hConsole, getWAttributes(param));
        cout << buildProgressBar(param) << param + PROCENTS << DIVIDER << rpm(param) << endl;
        break;

    default:
        break;
    }
}

string GuiManager::rpm(string param)
{
    int i = convertToInt(param);
    int fan_speed = (float(i) / 100) * RTX_2060_MAX_RPM;
    return to_string(fan_speed) + RPM;
}

string GuiManager::buildProgressBar(string param)
{
    double devider;
    string blocks;
    string emptys;
    string bar;

    int i = convertToInt(param);
    double val = static_cast<double>(i);
    int rounded = static_cast<int>(round(val / 10.0)) * 10.0;
    int blocksNum = min(rounded / BARLENGTH, BARLENGTH);
    int emptyNum = min(BARLENGTH - blocksNum, BARLENGTH);

    for (i = 0; i < blocksNum; i++)
    {
        blocks += BLOCK;
    }

    for (i = 0; i < emptyNum; i++)
    {
        emptys += EMPTY;
    }

    bar = OPEN + blocks + emptys + CLOSE + SPACE;
    return bar;
}

WORD GuiManager::getWAttributes(string param)
{
    int i = convertToInt(param);
    int min = 25;
    int middle = 50;
    int max = 75;

    if (i < min)
    {
        return GREEN;
    }
    else if (i >= min && i < middle)
    {
        return YELLOW;
    }
    else if (i >= middle && i < max)
    {
        return ORANGE;
    }
    else if (i >= max)
    {
        return RED;
    }

    return WHITE;
}

int GuiManager::convertToInt(string ptr)
{
    try
    {
        return stoi(ptr);
    }
    catch (...)
    {
        return 0;
    }
}

void GuiManager::start()
{
    SystemAnalizer analizer;
    COORD coord = buildCoord();
    buildTopConsoleWindow();

    cout << " Loading...";

    while (true)
    {
        showStats(analizer, coord);
    }
}

void GuiManager::checkIsAchtung(string ram, string gpu, string cpu)
{
    int cpu_i = convertToInt(cpu);
    int ram_i = convertToInt(ram);
    int gpu_i = convertToInt(gpu);

    bool is_ahtung = cpu_i > CRITICAL && ram_i > CRITICAL && gpu_i > CRITICAL;

    if (is_ahtung)
    {
        SetConsoleTextAttribute(hConsole, RED);
        cout << " !!! ACHTUNG !!!";
    }
}