//
// Created by henry on 7/22/2025.
//

#include "csvExport.h"
#include <fstream>
#include <vector>

using namespace std;

void exportToCSV(const vector<string>& dates, const vector<int>& signal, const vector<double>& pnl, const vector<double>& cumulative)
{
    ofstream out("C:/Users/henry/MySideProjects/quant-pairs/docs/trades.csv");
    out << "Date,Signal,DailyPNL,CumulativePNL\n";

    for(size_t i = 0; i < dates.size(); i++)
    {
        out << dates[i] << "," << signal[i] << "," << pnl[i] << "," << cumulative[i] << "\n";
    }

    out.close();
}