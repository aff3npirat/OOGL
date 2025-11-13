#pragma once

#include <string>


class BenchmarkStats {
  public:
    BenchmarkStats()
    {
        isAveraged = false;
        count = 0;
        average = 0.0;
        deviation = 0.0;
        min = -1.0;
        max = -1.0;
    }
    BenchmarkStats(std::string stats);

    void addFramedata(double frametime);

    std::string toString(unsigned int decimals);

    static void compare(BenchmarkStats& a, BenchmarkStats& b, double* avgFactor)
    {
        *avgFactor = a.average / b.average;
    }

  private:
    bool isAveraged;
    unsigned int count;
    double average;
    double deviation;
    double min;
    double max;
};


std::string roundedString(double value, unsigned int decimals);

std::string readFile(const char* fpath);

void writeStats(const char* fpath, BenchmarkStats stats);

std::string readStats(const char* fpath);
