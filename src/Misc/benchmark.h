#pragma once
#include <windows.h>
#include <chrono>
#include <vector>
#include <iostream>

#define DEBUG(x) do { std::cout << x << '\n'; } while (0)

void saveOnExit();

/*
Add to the end of your main function

Must have called setupBenchmark at the start
*/
BOOL WINAPI writeDataOnExit(_In_ DWORD dwCtrlType);

/*
If you wish to write all your data to a text file then call this function in the beggining of the main function

If you already have a Close App Callback function then you can call saveOnExit there.
*/
void setupBenchmark();


/*
    If you get a very high maxTime right after starting the program then do a Sleep(5000)
    in your main function right before the function you want to benchmark
    so your other threads have time to finish their work

    Time unit:
    1 - Seconds
    2 - Milliseconds
    3 - Nanoseconds
    4 - Microseconds
    */
class Timer {
public:

    Timer() = delete;

    
    Timer(int ratio);

    void Stop();

    ~Timer() {
        Stop();
    }

private:
    std::chrono::time_point< std::chrono::high_resolution_clock> m_StartTimepoint;
    int timeMeasure;
};
