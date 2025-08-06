#ifndef DATASCRAPER_H
#define DATASCRAPER_H

#include <string>

// Returns the exit code from fetchData.py (0 on success)
int dataScrape(const std::string& ticker1, const std::string& ticker2);

#endif // DATASCRAPER_H
