
#include <iostream>
#include "Stock.hpp"
#include "Helper.hpp"
#include "LoadData.hpp"
#include "Gnuplot.hpp"

//  # threads to use for downloading price data
//  const int N_THREAD = 5;

// wrap all global variables in a struct
_MEMO_ MEMO;


int main() {
    
    draw_head();    
    srand((unsigned)time(NULL));
    

    // read earning csv file and save all information into the map ( MEMO.StockList )
    populateSymbolVector();

    
    // divide all stocks into three vector ( MEMO.Beat / MEMO.Meet / MEMO.Miss )
    classify();
    
    
    
    int N_THREAD;
    cout << "  The number of concurrent threads supported by this computer is " << std::thread::hardware_concurrency() << "." << endl << endl;
    cout << "  # of threads to use to download : ";
    cin >> N_THREAD;
    
    // use multi-thread to download price data of all stocks + IWV
    auto t1 = high_resolution_clock::now();
    multithread_download(N_THREAD);
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> dt = t2 - t1;
    cout << endl << endl << "  Downloading took "  << static_cast<int>(dt.count()) / 1000 << " seconds !" << endl << endl;
    
    
    // calculate the largest valid N
    calculate_max_N();
    
    
    // calculate the index returns
    MEMO.StockList["IWV"].calculate_returns();
    

    bool state = true;
    string input;
    int option;
    
    while (state) {

        cout << "  ----------------------------------------------- " << endl;
        cout << "  | 1. Set N                                    | " << endl;
        cout << "  | 2. Get information of a single stock        | " << endl;
        cout << "  | 3. Display AAR, AAR-STD, CAAR and CAAR-STD  | " << endl;
        cout << "  | 4. Plot CAAR                                | " << endl;
        cout << "  | 5. Plot CAAR of Stock(s) or Group(s)        | " << endl;
        cout << "  | 6. Exit the program                         | " << endl;
        cout << "  ----------------------------------------------- " << endl << endl;
        cout << "  Choose one option : ";
        cin >> input;
        cout << endl;
        
        while ( input.size() != 1 || input[0] < '1' || input[0] > '6') {
            cout << "  Invalid Option ! Please enter again : ";
            cin >> input;
            cout << endl;
        }
        
        option = std::stoi(input);
        string ticker, n_;

        switch (option) {
            
            case 1: {
                cout << "  Enter N (60 <= N <= " << MEMO.N_MAX << "): ";
                cin >> n_;
                cout << endl;
                while (!is_number(n_) || std::stoi(n_) < 60 || std::stoi(n_) > MEMO.N_MAX) {
                    cout << "  Invalid N ! Please enter again : ";
                    cin >> n_;
                    cout << endl;
                }
                MEMO.N = std::stoi(n_);
                cout << "  -------------   Bootstrapping   -------------- " << endl ;
                
                t1 = high_resolution_clock::now();
                bootstrap(MEMO.N);
                t2 = high_resolution_clock::now();
                duration<double, std::milli> dt = t2 - t1;
                cout << endl << "  Bootstrapping took " << static_cast<int>(dt.count()) / 1000 << " seconds !" << endl;
                break;
            }
            
            case 2:
                cout << "  Enter the stock ticker or EXIT back to the menu : ";
                cin >> ticker;
                while ( MEMO.StockList.find(ticker) == MEMO.StockList.end() && ticker != "EXIT" ) {
                    cout << endl << "  \"" << ticker << "\" not found ! Please enter ticker again : ";
                    cin >> ticker;
                }
                if (ticker != "EXIT")
                    cout << endl << endl << MEMO.StockList[ticker] << endl;
                break;

            case 3:
                if (MEMO.N < 60 || MEMO.N > MEMO.N_MAX) {
                    cout << "  Please enter a valid N first ! "<< endl;
                    break;
                }
                cout << "  Choose one group ? ( 0 : MISS, 1 : MEET, 2 : BEAT ) ";
                cin >> n_;
                while (n_.size() != 1 || n_[0] < '0' || n_[0] > '2') {
                    cout << endl << "  Invalid Input ! Enter again : ";
                    cin >> n_;
                }
                
                cout << "  -----------------------------------------------------------------" << endl;
                cout << "  |   t   |     AAR     |   AAR-STD   |    CAAR     |   CAAR-STD  |" << endl;
                cout << "  -----------------------------------------------------------------" << endl;
                
                cout << fixed << setprecision(4);
                for (int i=0; i<2*MEMO.N; i++)
                    cout << "  | " << setw(4) << i - MEMO.N + 1 << "  |   " << setw(7) << MEMO.Result[std::stoi(n_)][0][i] << "   |   " << setw(7) << MEMO.Result[std::stoi(n_)][1][i] << "   |   " << setw(7) << MEMO.Result[std::stoi(n_)][2][i] << "   |   " << setw(7) << MEMO.Result[std::stoi(n_)][3][i] << "   |" << endl;
                
                cout << "  -----------------------------------------------------------------" << endl;
                break;
                
            case 4:
                if (MEMO.N < 60 || MEMO.N > MEMO.N_MAX)
                    cout << "  Please enter a valid N first ! "<< endl;
                else
                    plotResults(MEMO.Result[MISS][2], MEMO.Result[MEET][2], MEMO.Result[BEAT][2], MEMO.N);
                break;
                
            case 5:
                if (MEMO.N < 60 || MEMO.N > MEMO.N_MAX)
                    cout << "  Please enter a valid N first ! "<< endl;
                else
                    plotResults(MEMO.N);
                break;
                
            case 6:
                state = false;
                break;
        }
        
        cout << endl;
    }
    
    return 0;
}
