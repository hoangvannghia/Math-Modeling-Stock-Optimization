//
//  main.cpp
//  StockOptimization
//
//  Created by Nghia on 3/31/16.
//  Copyright © 2016 Nghia. All rights reserved.
//

#include "Problem.hpp"

int main(int argc, const char * argv[])
{
    Problem p1("input.txt");             //Read input.txt
    Problem p3("input.txt");            //Can't use p1.Solve2 after p1.Solve1
    Problem p4("input.txt");
    p.Solve1("output-mh1.txt");         //Using math model 1 solve, result in is stored output-mh1.txt
    p.Solve3("output-mh3.txt");         //Using math model 3 solve, result in is stored output-mh3.txt
    p.Solve4("output-mh4.txt");         //Using math model 3 solve, result is stored in output-mh3.txt
    return 0;
}
