#include "pch.h"
#include "stopwatch.h"
#include <iostream>

Stopwatch::Stopwatch(std::string stopwatch_name)
    :name_(stopwatch_name)
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&ticks_per_second_);
}

void Stopwatch::Reset()
{
    QueryPerformanceCounter((LARGE_INTEGER*)&start_);
}

void Stopwatch::PrintAndReset(char* identifier)
{
    Print(identifier);
    Reset();
}

float Stopwatch::OutputAndReset()
{
    float output = Output();
    Reset();
    return output;
}

void Stopwatch::Print(char* identifier)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    uint current_color;
    if (hConsole != nullptr)
    {
        _CONSOLE_SCREEN_BUFFER_INFO console_info;
        GetConsoleScreenBufferInfo(hConsole, &console_info);
        current_color = console_info.wAttributes;
        SetConsoleTextAttribute(hConsole, 13);
    }
    if (identifier == nullptr)
    {
        identifier = "";
    }
    std::cout << "stopwatch "<< name_.c_str()<< " - " << identifier << " :" << Output() << std::endl;
    if (hConsole != nullptr)
    {
        SetConsoleTextAttribute(hConsole, current_color);
    }
}

float Stopwatch::Output()
{
    unsigned long long end;
    QueryPerformanceCounter((LARGE_INTEGER*)&end);
    elapsed = (end - start_) / (double)ticks_per_second_;
    return elapsed;
}

