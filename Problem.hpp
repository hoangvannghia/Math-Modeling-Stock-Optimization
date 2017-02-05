//
//  Problem.hpp
//  StockOptimization
//
//  Created by Nghia on 3/31/16.
//  Copyright © 2016 Nghia. All rights reserved.
//

#ifndef Problem_hpp
#define Problem_hpp

#include <cmath>
#include <limits>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glpk.h>

using namespace std;

class Stock
{
public:
    string name;
    double cost;
    float dividend[12];
    double limit;
    
    vector<int> month;
    vector<long> change;
};

class Problem
{
public:
    int startTime;
    double captital;
    double bankRate;
    int timeInvest;
    vector<Stock> listOfStock;
    unsigned long timeSolve;

    Problem(string input);
    void Solve1(string outPutName);
    void Solve3(string outPutName);
    void Solve4(string outPutName);
private:
    void WriteResult(string fileName, glp_prob *lp);

};



#endif /* Problem_hpp */
