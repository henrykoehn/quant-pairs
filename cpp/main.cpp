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

vector<double> loadClose(const string& path)
{
    ifstream in(path);
    if(!in)
    {
        cout << "failed opening " << path << "\n" ;
        return {};
    }

    string line;
    getline(in, line);
    vector<double> closes;
    while(getline(in, line))
    {
        auto pos = line.find_last_of(",");
        if(pos != string::npos)
        {
            closes.push_back(stod(line.substr(pos + 1)));
        }
    }
    return closes;
}

struct DatedClose {
    string date;
    double close;
};

vector<DatedClose> loadDatedCloses(const string& path) {
    ifstream in(path);
    if (!in) {
        cerr << "Failed opening " << path << "\n";
        return {};
    }

    string line;
    if (!getline(in, line)) return {}; // header missing

    // Parse header to find indices
    vector<string> headers;
    {
        istringstream hs(line);
        string h;
        while (getline(hs, h, ',')) headers.push_back(h);
    }

    // Find "Date" and "Close" columns by name
    int dateIdx = -1, closeIdx = -1;
    for (int i = 0; i < (int)headers.size(); ++i) {
        if (headers[i] == "Date")  dateIdx  = i;
        if (headers[i] == "Close") closeIdx = i;
    }
    if (dateIdx < 0 || closeIdx < 0) {
        cerr << "CSV missing Date or Close columns: " << path << "\n";
        return {};
    }

    vector<DatedClose> out;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string field;
        vector<string> cols;
        while (getline(ss, field, ',')) cols.push_back(field);
        if ((int)cols.size() <= max(dateIdx, closeIdx)) continue;

        try {
            out.push_back({ cols[dateIdx], stod(cols[closeIdx]) });
        } catch (...) {
            // skip bad rows
        }
    }
    return out;
}


int main(int argc, char* argv[])
{
    if (argc < 3) {
        cout << "Usage: pairs_backtester <TICKER1> <TICKER2>\n";
        return 1;
    }

    string ticker1 = argv[1];
    string ticker2 = argv[2];

    if (dataScrape(ticker1, ticker2) != 0) {
        return 1;
    }

    auto data1 = loadClose("data/" + ticker1 + ".csv");
    auto data2 = loadClose("data/" + ticker2 + ".csv");

    if (data1.empty() || data2.empty()) {
        cout << "Failed to load price data.\n";
        return 1;
    }

    cout << "\nLoaded " << data1.size() << " " << ticker1 << " closes and " << data2.size() << " " << ticker2 << " closes." << endl;

    vector<double> spread;
    for(size_t i = 0; i < data1.size(); i++)
    {
        spread.push_back(data1[i] - data2[i]);
    }

    //generate signal using rolling std dev and mean
    const int window = 20;
    vector<int> signal(spread.size(), 0);

    for(size_t t = window; t < spread.size(); t++)
    {
        double m = mean(spread, t - window, window);
        double s = stdDev(spread, t - window, window, m);

        if(spread[t] > m + 2 * s)
        {
            signal[t] = -1;
        }
        else if(spread[t] < m - 2 * s)
        {
            signal[t] = +1;
        }
        else
        {
            signal[t] = 0;
        }
    }

    //simulate
    vector<double> pnl(spread.size(), 0.0);
    for(size_t t = window + 1; t < spread.size(); ++t)
    {
        double retdata1 = (data1[t] - data1[t-1]) / data1[t - 1];
        double retdata2 = (data2[t] - data2[t-1]) / data2[t - 1];

        if(signal[t - 1] == +1)
        {
            pnl[t] = +retdata1 - retdata2;
        }
        else if(signal[t - 1] == -1)
        {
            pnl[t] = -retdata1 + retdata2;
        }
        else
        {
            pnl[t] = 0;
        }
    }

    //relative return
    vector<double> cumulative(pnl.size(), 0.0);
    cumulative[0] = pnl[0];
    for (size_t i = 1; i < pnl.size(); ++i)
    {
        cumulative[i] = cumulative[i - 1] + pnl[i];
    }
    cout << "Total return: " << cumulative.back() * 100 << "%" << endl;

    auto d1 = loadDatedCloses("data/" + ticker1 + ".csv");
    auto d2 = loadDatedCloses("data/" + ticker2 + ".csv");

    unordered_map<string, double> map2;
    for (auto& dc : d2)
        map2[dc.date] = dc.close;

    // Now align the two on shared dates:
    vector<double> aapl, msft;
    vector<string> dates;

    for (auto& dc1 : d1) {
        if (map2.count(dc1.date)) {
            dates.push_back(dc1.date);
            aapl.push_back(dc1.close);
            msft.push_back(map2[dc1.date]);
        }
    }

    exportToCSV(dates, signal, pnl, cumulative);

    return 0;
}

