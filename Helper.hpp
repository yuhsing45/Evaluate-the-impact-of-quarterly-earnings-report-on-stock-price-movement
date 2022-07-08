
#ifndef Helper_hpp
#define Helper_hpp

#include <algorithm>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include "Stock.hpp"

using std::sort, std::min, std::round, std::ifstream, std::getline, std::pair, std::stringstream, std::ios, std::map, std::set, std::stod, std::cin, std::cout;
using std::chrono::high_resolution_clock, std::chrono::duration_cast, std::chrono::duration, std::chrono::milliseconds;


// randomly select NUM stocks for each group (each round)
const int NUM = 80;

// A struct that stores all global variables
struct _MEMO_ {
    map<string, Stock> StockList;           // a map containing all stocks information
    VectorS Beat, Meet, Miss, All;          // three vectors storing lists of tickers for each group
    int N = 0, N_MAX;                       // N -> user input, N_MAX -> Largest valid N
    Matrix3D Result;                        // A 3 x 4 x 2N matrix storing bootstrapping result ( group x metrics x t )
    int _cnt;
};
extern _MEMO_ MEMO;



// split all the stocks into 3 groups ( Beat / Meet / Miss ) with relatively equivalent numbers
void classify();


// find the largest N that fits all stocks
void calculate_max_N();


// select n strings without replacement
VectorS random_select(const VectorS& vt, int n = NUM);


// Calculate AAR, AAR-STD, CAAR, CAAR-STD for all three groups
void bootstrap(int N);


// check whether a string can be converted to integer
bool is_number(const string& s);


void draw_head();


#endif /* Helper_hpp */
