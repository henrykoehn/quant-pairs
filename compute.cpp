//
// Computes meand and std dev
//
#include <vector>
#include <iostream>
#include <cmath>
#include "compute.h"

using namespace std;

double mean(const vector<double>& data, int start, int window)
{
    double sum = 0.0;
    for(int i = start; i < start + window; i++)
    {
        sum += data[i];
    }

    return sum/window;
}

double stdDev(const vector<double>& data, int start, int window, int meanVal)
{
    double sum = 0.0;
    for(int i = start; i < start + window; i++)
    {
        sum += (data[i] - meanVal) * (data[i] - meanVal);
    }

    return sqrt(sum/window);
}
