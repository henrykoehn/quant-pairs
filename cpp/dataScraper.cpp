#include "dataScraper.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int dataScrape(const string& ticker1, const string& ticker2)
{
    // call the fetchData.py that lives in /app inside the container
    // using the explicit python3 binary
    string cmd = "/usr/bin/python3 /app/fetchData.py " + ticker1 + " " + ticker2;
    int result = system(cmd.c_str());
    if (result != 0) {
        cout << "Failed to run fetchData.py, exit code " << result << "\n";
    }
    return result;
}
