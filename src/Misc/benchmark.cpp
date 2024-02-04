#include "benchmark.h"
#include <fstream>

int maxTime = 0;
std::vector<int> timePerCall{};
std::string units;

BOOL WINAPI writeDataOnExit(_In_ DWORD dwCtrlType) {

    saveOnExit();

    _exit(NULL);
    return TRUE;
}

void setupBenchmark() {

    SetProcessShutdownParameters(2, 0);
    SetConsoleCtrlHandler(writeDataOnExit, TRUE);
}

extern void saveOnExit() {
    //The reason we do it here instead of when we are pushing the data to the vector
    //is so we minimize the bloat inside our Timer Class
    if (timePerCall.size() == 0) return;

    unsigned long long int sumOfCallTimes = 0;

    for (int i = 1; i < timePerCall.size(); i++)
        sumOfCallTimes += timePerCall[i];

    std::ofstream saveData("benchmark.txt");
    if (saveData.is_open()) {

        saveData << "Average call time: " << sumOfCallTimes / timePerCall.size() << units <<'\n';
        saveData << "Calls to function: " << timePerCall.size() << '\n';

        for (int i = 1; i < timePerCall.size(); i++)
            saveData << timePerCall[i] << units << '\n';

        saveData.close();
    }
}

Timer::Timer(int ratio){
    timeMeasure = ratio;

    switch (ratio) {
    case 1: units = " s"; break;
    case 2: units = " ms"; break;
    case 3: units = " us"; break;
    case 4: units = " ns"; break;

    default:
        timeMeasure = 4;
        units = " ns";
        break;
    }

    m_StartTimepoint = std::chrono::high_resolution_clock::now();
}

void Timer::Stop() {
    auto endTimePoint = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTimepoint).time_since_epoch().count();
    auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTimePoint).time_since_epoch().count();

    int duration = end - start;

    switch (timeMeasure) {
    case 1:
        duration *= 0.000000001;
        break;
    case 2:
        duration *= 0.000001;
        break;
    case 3:
        duration *= 0.001;
        break;
    }


    //Delete this if you don't want to see the time spent In Real Time and for more accurate results
    
    //The first time this class is constructed the latency is really high so we ignore the first result

    if (duration > maxTime && timePerCall.size() != 0) maxTime = duration;

    timePerCall.push_back(duration); // Dont delete this one
   
    DEBUG(duration << units);
    DEBUG("Max time spent in 1 call: " << maxTime << units);
    Sleep(1); //Sleep before clearing so you can see it
    system("cls");
}