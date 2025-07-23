//
// Created by henry on 7/22/2025.
//

#ifndef COMPUTE_H
#define COMPUTE_H
using namespace std;

double mean(const vector<double>& data, int start, int window);

double stdDev(const vector<double>& data, int start, int window, int meanVal);
#endif //COMPUTE_H
