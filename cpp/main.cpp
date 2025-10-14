#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "compute.h"
#include "dataScraper.h"
#include "csvExport.h"

using namespace std;

static bool loadDatesAndCloses(const string& path, vector<string>& datesOut, vector<double>& closesOut) {
    ifstream in(path);
    if (!in) return false;
    string line;
    if (!getline(in, line)) return false;

    vector<string> headers;
    {
        istringstream hs(line);
        string h;
        while (getline(hs, h, ',')) headers.push_back(h);
    }

    int dateIdx = -1, closeIdx = -1;
    for (int i = 0; i < (int)headers.size(); ++i) {
        if (headers[i] == "Date")  dateIdx  = i;
        if (headers[i] == "Close") closeIdx = i;
    }
    if (dateIdx < 0 || closeIdx < 0) return false;

    vector<string> dates;
    vector<double> closes;
    dates.reserve(4096);
    closes.reserve(4096);

    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string field;
        vector<string> cols;
        while (getline(ss, field, ',')) cols.push_back(field);
        if ((int)cols.size() <= max(dateIdx, closeIdx)) continue;
        try {
            dates.push_back(cols[dateIdx]);
            closes.push_back(stod(cols[closeIdx]));
        } catch (...) {}
    }

    datesOut.swap(dates);
    closesOut.swap(closes);
    return !datesOut.empty() && datesOut.size() == closesOut.size();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: pairs_backtester <TICKER1> <TICKER2>\n";
        return 1;
    }

    string t1 = argv[1], t2 = argv[2];

    if (dataScrape(t1, t2) != 0) return 1;

    vector<string> d1_dates, d2_dates;
    vector<double> d1_close, d2_close;
    if (!loadDatesAndCloses("data/" + t1 + ".csv", d1_dates, d1_close) ||
        !loadDatesAndCloses("data/" + t2 + ".csv", d2_dates, d2_close)) {
        cout << "Failed to load price data.\n";
        return 1;
    }

    unordered_map<string, double> map2;
    map2.reserve(d2_dates.size() * 2);
    for (size_t i = 0; i < d2_dates.size(); ++i) map2[d2_dates[i]] = d2_close[i];

    vector<string> dates;
    vector<double> c1, c2;
    dates.reserve(d1_dates.size());
    c1.reserve(d1_dates.size());
    c2.reserve(d1_dates.size());
    for (size_t i = 0; i < d1_dates.size(); ++i) {
        auto it = map2.find(d1_dates[i]);
        if (it != map2.end()) {
            dates.push_back(d1_dates[i]);
            c1.push_back(d1_close[i]);
            c2.push_back(it->second);
        }
    }

    if (c1.size() < 25) {
        cout << "Failed to load price data.\n";
        return 1;
    }

    vector<double> spread;
    spread.reserve(c1.size());
    for (size_t i = 0; i < c1.size(); ++i) spread.push_back(c1[i] - c2[i]);

    const int window = 20;
    vector<int> signal(spread.size(), 0);
    for (size_t t = window; t < spread.size(); ++t) {
        double m = mean(spread, int(t) - window, window);
        double s = stdDev(spread, int(t) - window, window, m);
        if      (spread[t] > m + 2 * s) signal[t] = -1;
        else if (spread[t] < m - 2 * s) signal[t] = +1;
        else                            signal[t] = 0;
    }

    vector<double> pnl(spread.size(), 0.0);
    for (size_t t = window + 1; t < spread.size(); ++t) {
        double r1 = (c1[t] - c1[t - 1]) / c1[t - 1];
        double r2 = (c2[t] - c2[t - 1]) / c2[t - 1];
        if      (signal[t - 1] == +1) pnl[t] = +r1 - r2;
        else if (signal[t - 1] == -1) pnl[t] = -r1 + r2;
        else                          pnl[t] = 0.0;
    }

    vector<double> cumulative(pnl.size(), 0.0);
    for (size_t i = 1; i < pnl.size(); ++i) cumulative[i] = cumulative[i - 1] + pnl[i];

    cout << "Total return: " << cumulative.back() * 100 << "%\n";

    exportToCSV(dates, signal, pnl, cumulative);
    return 0;
}
