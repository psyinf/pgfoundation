#pragma once
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <conio.h>
#define NOMINMAX

#include <windows.h>
#include <iostream>
#endif
// for now cheap parlor trick to enable VT100 on windows
namespace pg::foundation::console {

void setupConsole()
{
#ifdef _WIN32
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    // enable VT100
    std::cout << "\033[?1049h";
}

void setCursorVisibility(bool visible)
{
    std::cout << "\033[?25" << (visible ? "h" : "l");
}

// clear screen
inline void clearScreen()
{
    std::cout << "\033[2J";
}

// move cursor to x,y
inline void moveCursor(int x, int y)
{
    std::cout << "\033[" << y << ";" << x << "H";
}
} // namespace pg::foundation::console
