#include "utility.h"

#include <math.h>

#include <fstream>
#include <sstream>
#include <string>

#include "config.h"


BenchmarkStats::BenchmarkStats(std::string stats)
{
    int j = 0;
    std::string value = "";
    for (int i = 0; i < stats.length(); i++) {
        if (stats[i] == ',') {
            switch (j) {
                case 0: average = std::stod(value); break;
                case 1: deviation = std::stod(value); break;
                case 2: min = std::stod(value); break;
                case 3: max = std::stod(value); break;
            }

            value = "";
            j++;
        }
        else if (std::isdigit(stats[i]) || stats[i] == '.') {
            value += stats[i];
        }
    }

    count = std::stoi(value);
    isAveraged = true;
}


void BenchmarkStats::addFramedata(double frametime)
{
    average += frametime;
    deviation += frametime * frametime;

    if (frametime < min || min == -1) {
        min = frametime;
    }

    if (frametime > max || max == -1) {
        max = frametime;
    }

    count++;
}


std::string BenchmarkStats::toString(unsigned int decimals)
{
    if (!isAveraged) {
        average = average / count;
        deviation = deviation / count - average * average;
        deviation = std::sqrt(deviation);
    }

    return "avg=" + roundedString(average, decimals) +
           ", std=" + roundedString(deviation, decimals) + ", min=" + roundedString(min, decimals) +
           ", max=" + roundedString(max, decimals) + ", N=" + std::to_string(count);
}


std::string roundedString(double value, unsigned int decimals)
{
    bool positive = true;
    if (value < 0) {
        value *= -1;
        positive = false;
    }

    int v = (int)value;    // cutoff decimals
    unsigned int pow10 = std::pow(10, decimals);

    int decimalValues = (int)(value * pow10 - v * pow10);

    return (positive ? "" : "-") + std::to_string(v) + "." + std::to_string(decimalValues);
}


std::string readFile(const char* fpath)
{
    std::string content = "";
    std::ifstream Stream(fpath, std::ios::in);
    if (Stream.is_open()) {
        std::stringstream sstr;
        sstr << Stream.rdbuf();
        content = sstr.str();
    }
    else {
        printf("Could not open file '%s'", fpath);
    }

    return content;
}


void writeStats(const char* fpath, BenchmarkStats stats)
{
    std::ofstream file;
    file.open(fpath, std::ofstream::out);

    std::string contents = stats.toString(DECIMALS) + "\n";
    file.write(contents.c_str(), contents.length());

    file.close();
}


std::string readStats(const char* fpath)
{
    std::ifstream file;
    file.open(fpath, std::ifstream::in);

    char* contents = new char[MAX_LINE_LENGTH];
    file.getline(contents, MAX_LINE_LENGTH, '\n');

    return std::string(contents);
}
