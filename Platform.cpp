#include "Dictionary.h"
#include <cstdlib>

// ============================================================================
// All OS-dependent code (Windows / Linux) is gathered ONLY in this single
// file. Other modules (Search.cpp, Quiz.cpp, main.cpp) only call
// readOneChar() / clearScreen() / sleepMs() without caring about which OS
// is running -> adhering to Modular Programming principles for future porting.
// ============================================================================

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>

    int readOneChar() {
        return _getch();
    }

    void clearScreen() {
        if (system("cls") != 0) { /* ignore error, does not affect main logic */ }
    }

    void sleepMs(int milliseconds) {
        Sleep(static_cast<DWORD>(milliseconds));
    }

#else
    #include <termios.h>
    #include <unistd.h>
    #include <cstdio>

    int readOneChar() {
        struct termios oldSettings, newSettings;

        if (tcgetattr(STDIN_FILENO, &oldSettings) != 0) {
            return getchar(); // Failed to get terminal attributes -> fallback
        }

        newSettings = oldSettings;
        newSettings.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode + disable echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

        int ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings); // Restore terminal
        return ch;
    }

    void clearScreen() {
        if (system("clear") != 0) { /* ignore error, does not affect main logic */ }
    }

    void sleepMs(int milliseconds) {
        usleep(static_cast<useconds_t>(milliseconds) * 1000);
    }
#endif
