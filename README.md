# Evaluate-the-impact-of-quarterly-earnings-report-on-stock-price-movement
Analyze the impact of earning announcement from 2277 stocks, from January 2021 to March 2022, looking at the change in stock prices N days before and after the announcement. The impact is evaluated based on the surprise percentage calculated using predicted earning and actual earning.

The stocks and their corresponding price information for each group should be stored in a STL map, with stock symbol as its keys

The expected AAR, AAR STD, and expected CAAR and CAAR STD for 3 groups are presented in a matrix. The row of the matrix is the group#, matrix columns are for AAR, AAR-STD, CAAR, CAAR-STD

Use gnuplot to show the CAAR from all 3 groups in one graph

Your program should be able to:

Retrieve historical price data for all selected stocks. Parse the retrieved data for dates and adjusted closing prices
Calculate AAR, AAR-STD, CAAR, CAAR-STD for each group
Populate the stock maps and AAR/CAAR matrix
Show the gnuplot graph with CAAR for all 3 groups
Your program should have a menu of 5 options:

Enter N to retrieve 2N+1 days of historical price data for all stocks (you need to validate user input to make sure N >= 60)
Pull information for one stock from one group:
    (1) Daily Prices

    (2) Cumulative Daily Returns

    (3) The group the stock belongs to

    (4) Earning Announcement Date

    (5) Period Ending

    (6) Estimated

    (7) Reported Earnings

    (8) Surprise

    (9) Surprise %

Show AAR, AAR-STD, CAAR and CAAR-STD for one group
Show the gnuplot graph with CAAR for all 3 groups
Exit your program
