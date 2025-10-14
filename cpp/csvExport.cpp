// csvExport.cpp
#include "csvExport.h"
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

void exportToCSV(const vector<string>& dates,
                 const vector<int>& signal,
                 const vector<double>& pnl,
                 const vector<double>& cumulative)
{
    // Served at https://<site>/trades.csv
    ofstream out("wwwroot/trades.csv");
    out << "Date,Signal,DailyPNL,CumulativePNL\n";

    // Keep indices safe across possibly different lengths
    size_t n = min({ dates.size(), signal.size(), pnl.size(), cumulative.size() });
    for (size_t i = 0; i < n; ++i) {
        out << dates[i] << "," << signal[i] << "," << pnl[i] << "," << cumulative[i] << "\n";
    }
}
