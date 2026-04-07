#include "system_analizer.h"
#include <Windows.h>
#include <string>

using part = SystemAnalizer::DevicePart;

class GuiManager
{
public:
    enum AttributesType
    {
        _default,
        gpu_fan
    };
    void start();

private:
    void showStats(SystemAnalizer, COORD);
    void buildTopConsoleWindow();
    void hideCursor();
    void setBoldFont();
    void show(SystemAnalizer::DevicePart, string);
    void checkIsAchtung(string, string, string);
    void clean(COORD coord);
    int calculateNextStepValue(int &start, int finish);
    int convertToInt(string);
    string buildProgressBar(string);
    string rpm(string);
    WORD getWAttributes(string);
    COORD buildCoord();

    string ram_cache;
    string gpu_cache;
    string gpu_temp_cache;
    string fan_usage_cache;
    string cpu_cache;
};