
#ifndef Gnuplot_hpp
#define Gnuplot_hpp

#include <vector>
#include <algorithm>
using std::vector;

extern _MEMO_ MEMO;

// plot CAAR for Group MISS, MEET, BEAT
void plotResults(const vector<double>& miss, const vector<double>& meet, const vector<double>& beat, int N){
    
    FILE *gnuplotPipe, *tempDataFile;
    
    const char *tempDataFileMiss = "Miss";
    const char *tempDataFileMeet = "Meet";
    const char *tempDataFileBeat = "Beat";
    
    double x1, y1, x2, y2, x3, y3;
    
    //set gnuplot enviorment
    gnuplotPipe = popen("/opt/local/bin/gnuplot", "w");
    if(gnuplotPipe){
        fprintf(gnuplotPipe, "plot \"%s\" with lines, \"%s\" with lines, \"%s\" with lines\n", tempDataFileBeat, tempDataFileMeet, tempDataFileMiss);
        fflush(gnuplotPipe);
        
        // miss data
        tempDataFile = fopen(tempDataFileMiss, "w");
        for (int i=0; i<2*N; i++) {
            x3 = i - N + 1;
            y3 = miss[i];
            fprintf(tempDataFile,"%lf %lf\n",x3,y3);
        }
        fclose(tempDataFile);
        
        // meet data
        tempDataFile = fopen(tempDataFileMeet,"w");
        for (int i=0; i<2*N; i++) {
            x2 = i - N + 1;
            y2 = meet[i];
            fprintf(tempDataFile,"%lf %lf\n",x2,y2);
        }
        fclose(tempDataFile);
        
        // beat data
        tempDataFile = fopen(tempDataFileBeat,"w");
        for (int i=0; i<2*N; i++) {
            x1 = i - N + 1;
            y1 = beat[i];
            fprintf(tempDataFile,"%lf %lf\n",x1,y1);
        }
        fclose(tempDataFile);
        
    }
    else
        printf("gnuplot not found...");
    
}


// plot CAAR for individual stock(s) or Group(s)
void plotResults(int N) {

    string ticker;
    VectorS vt;

    while (ticker != "DONE") {
        cout << endl << "  Enter stock ticker or DONE : ";
        cin >> ticker;
        if (MEMO.StockList.find(ticker) != MEMO.StockList.end() && std::find(vt.begin(), vt.end(), ticker) == vt.end())
            vt.push_back(ticker);
    }
    
    FILE *gnuplotPipe, *tempDataFile;
    double x, y;

    //set gnuplot enviorment
    if (vt.size() > 0) {
    gnuplotPipe = popen("/opt/local/bin/gnuplot", "w");
        if(gnuplotPipe){
            
            fprintf(gnuplotPipe,"plot ");
            for (size_t i=0; i<vt.size(); i++) {
                if (i == vt.size()-1)
                    fprintf(gnuplotPipe,"\"%s\" with lines\n",  vt[i].c_str());
                else
                    fprintf(gnuplotPipe,"\"%s\" with lines, ",  vt[i].c_str());
            }
            fflush(gnuplotPipe);
            
            
            for (size_t i=0; i<vt.size(); i++) {
                int start = MEMO.StockList[vt[i]].earn_ann_date_index_ - N;
                MEMO.StockList[vt[i]].calculate_returns(N);
                tempDataFile = fopen(vt[i].c_str(), "w");
                fprintf(tempDataFile,"%lf %lf\n", -(double)N, 0.0);
                
                for (int j=0; j < 2*N; j++) {
                    x = j - N + 1;
                    y = MEMO.StockList[vt[i]].cumulative_ret_[j] - MEMO.StockList["IWV"].cumulative_ret_[start + j] + (start > 0 ? MEMO.StockList["IWV"].cumulative_ret_[start-1] : 0);
                    fprintf(tempDataFile,"%lf %lf\n",x, y);
                }
                fclose(tempDataFile);
            }
        }
        else
            printf("gnuplot not found...");
    }
}

#endif /* Gnuplot_hpp */
