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
    std::string date;
    double close;
};

std::vector<DatedClose> loadDatedCloses(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Failed opening " << path << "\n";
        return {};
    }

    std::string line;
    std::getline(in, line); // skip header

    std::vector<DatedClose> data;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string date, close_str;

        std::getline(ss, date, ','); // Date
        for (int i = 0; i < 4; ++i) std::getline(ss, close_str, ','); // skip to Close

        double close = std::stod(close_str);
        data.push_back({ date, close });
    }

    return data;
}


int main()
{
    string ticker1, ticker2;
    cout << "Enter first ticker:" << endl;
    cin >> ticker1;
    cout << "Enter second ticker:" << endl;
    cin >> ticker2;

    if (dataScrape(ticker1, ticker2) != 0) {
        return 1;
    }

    auto data1 = loadClose("C:/Users/henry/MySideProjects/quant-pairs/data/" + ticker1 + ".csv");
    auto data2 = loadClose("C:/Users/henry/MySideProjects/quant-pairs/data/" + ticker2 + ".csv");

    if (data1.empty() || data2.empty()) {
        cout << "Failed to load price data.\n";
        return 1;
    }

    cout << "Loaded " << data1.size() << " " << ticker1 << " closes and " << data2.size() << " " << ticker2 << " closes." << endl;

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

    auto d1 = loadDatedCloses("C:/Users/henry/MySideProjects/quant-pairs/data/" + ticker1 + ".csv");
    auto d2 = loadDatedCloses("C:/Users/henry/MySideProjects/quant-pairs/data/" + ticker2 + ".csv");

    unordered_map<std::string, double> map2;
    for (auto& dc : d2)
        map2[dc.date] = dc.close;

    // Now align the two on shared dates:
    std::vector<double> aapl, msft;
    std::vector<std::string> dates;

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

