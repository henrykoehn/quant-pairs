//
// Created by henry on 7/22/2025.
//

#include "dataScraper.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace std;

int dataScrape(const string& ticker1, const string& ticker2)
{
    string command = "python \"C:\\Users\\henry\\MySideProjects\\quant-pairs\\downloadData.py\" " + ticker1 + " " + ticker2;

    int result = system(command.c_str());

    if (result != 0) {
        cout << "Failed to run Python script.\n";
        return 1;
    }

    return 0;
}
