
#ifndef LoadData_hpp
#define LoadData_hpp

#include <map>
#include <set>
#include <sstream>
#include <thread>
#include "curl/curl.h"
#include "Stock.hpp"
using std::map, std::set, std::stringstream;


// CHANGE TO YOUR OWN PATH & TOKEN
const char* filepath = "Russell3000EarningsAnnouncements.csv";
const string api_token = "62686613762699.37705291";



// convert date strings to a uniform form
map<string, string> _DICT = {
    {"MAY", "05"},
    {"JUN", "06"},
    {"JUL", "07"},
    {"AUG", "08"},
    {"SEP", "09"},
    {"OCT", "10"}
};
string date_convert(const string& s) {
    return "2021-" + _DICT[s.substr(3, 3)] + "-" + s.substr(0, 2);
}


// populate a list of tickers
void populateSymbolVector() {
    
    ifstream fin;
    fin.open(filepath, ios::in);
    if (fin) {
        string line, ticker, date, period_ending, estimate, reported, surprise, surprise_pct;
        getline(fin, line);
        while (!fin.eof()) {
            getline(fin, line);
            stringstream sin(line);
            getline(sin, ticker, ',');
            if (ticker != "") {
                getline(sin, date, ',');
                getline(sin, period_ending, ',');
                getline(sin, estimate, ',');
                getline(sin, reported, ',');
                getline(sin, surprise, ',');
                getline(sin, surprise_pct, ',');
                MEMO.StockList.emplace(ticker, Stock(date_convert(date), period_ending, stod(estimate), stod(reported), stod(surprise), stod(surprise_pct)));
                MEMO.All.push_back(ticker);
            }
        }
    } else {
        cout << " Please make sure the csv file is in the correct path ! " << endl;
        std::exit(0);
    }
}


// download price data using libcurl and store them into map<string, Stock>
struct MemoryStruct {
    char* memory;
    size_t size;
    size_t total_size;
};
int write_data(void* ptr, int size, int nmemb, FILE* stream);
void* myrealloc(void* ptr, size_t size);
int write_data2(void* ptr, size_t size, size_t nmemb, void* data);


bool download(const VectorS* group, map<string, Stock>* pstocklist) {

    CURL* handle;
    CURLcode result;

    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();
    
    cout << fixed << setprecision(1);
    
    if (handle) {
        string url_common = "https://eodhistoricaldata.com/api/eod/";
        string start_date = "2021-01-01";
        string end_date = "2022-03-31";
    
        struct MemoryStruct data;
        data.memory = NULL;
        data.size = 0;
        data.total_size = 0;
        
//        int N = (int)pstocklist->size() / 10;
        double N = pstocklist->size() / 100.0;
        
        for (const auto& symbol: *group) {

            data.size = 0;
            memset(data.memory, '\0', data.total_size);
            string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
            
            curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
             
            result = curl_easy_perform(handle);
            
            stringstream sData;
            sData.str(data.memory);
            string sValue, sDate, line, prev{};
            double dValue = 0;
            
            while (getline(sData, line)) {
                size_t found = line.find('-');
                if (found != std::string::npos) {
                    sDate = line.substr(0, line.find_first_of(','));
                    line.erase(line.find_last_of(','));
                    sValue = line.substr(line.find_last_of(',') + 1);
                    dValue = stod(sValue);
                    if (symbol == "IWV")
                        (*pstocklist)[symbol].date_.push_back(sDate);
                    if (prev == "2021-12-30" && sDate != "2021-12-31")
                        (*pstocklist)[symbol].daily_price_.push_back((*pstocklist)[symbol].daily_price_.back());
                    (*pstocklist)[symbol].daily_price_.push_back(dValue);
                    if (sDate == (*pstocklist)[symbol].earn_ann_date_) {
                        (*pstocklist)[symbol].earn_ann_date_index_ = (int)(*pstocklist)[symbol].daily_price_.size();
                    }
                    prev = sDate;
                }
            }
            if ((*pstocklist)[symbol].daily_price_.size() != 314 || (*pstocklist)[symbol].earn_ann_date_index_ < 60)
                (*pstocklist)[symbol].valid_ = false;
            else
                (*pstocklist)[symbol].valid_ = true;
            
            MEMO._cnt += 1;
//            if (MEMO._cnt % N == 0)
//                 cout << "  ------------------ " << setw(2) << MEMO._cnt / N << "0% --------------------" << endl;
            cout << "\r  ------------------" << setw(6) << MEMO._cnt / N << "% ------------------\r" << std::flush;
        }
        free(data.memory);
        data.size = 0;
    } else
        return false;

    curl_easy_cleanup(handle);
    curl_global_cleanup();
    
    return true;
}


void multithread_download(int n) {
    
    MEMO.StockList.insert({"IWV", Stock()});
    MEMO.All.push_back("IWV");
    
    // divide stocks into n subgroups
    int bin = (int)MEMO.All.size() / n;
    vector<vector<string>> subgroup(n);
    for (int i = 0; i < n - 1; i++)
        subgroup[i] = vector<string>(MEMO.All.begin() + i * bin, MEMO.All.begin() + (1 + i) * bin);
    subgroup[n-1] = vector<string>(MEMO.All.begin() + (n - 1) * bin, MEMO.All.end());
    
    MEMO._cnt = 0;
    cout << endl << "  ---------  Downloading Price Data  --------- " << endl << endl;
    
    // add n threads
    vector<std::thread> threadv;
    for (int i = 0; i < n; i++)
        threadv.push_back( std::thread(download, &subgroup[i], &MEMO.StockList) );
    for (auto& thread: threadv)
        thread.join();
}

// Auxiliary functions
int write_data(void* ptr, int size, int nmemb, FILE* stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return (int)written;
}

void* myrealloc(void* ptr, size_t size) {
    if (ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}

int write_data2(void* ptr, size_t size, size_t nmemb, void* data) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)data;
    if ((mem->size + realsize) >= mem->total_size)
        mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
        if (mem->size > mem->total_size)
            mem->total_size = mem->size;
    }
    return (int)realsize;
}

#endif /* LoadData_hpp */
