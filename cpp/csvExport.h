//
// Created by henry on 7/22/2025.
//

#ifndef CSVEXPORT_H
#define CSVEXPORT_H
#include <fstream>
#include <vector>

using namespace std;

void exportToCSV(const vector<string>& dates, const vector<int>& signal, const vector<double>& pnl, const vector<double>& cumulative);
#endif //CSVEXPORT_H
