//
//  Problem.cpp
//  StockOptimization
//
//  Created by Nghia on 3/31/16.
//  Copyright © 2016 Nghia. All rights reserved.
//

#include "Problem.hpp"

Problem::Problem(string input)
{
    ifstream is;
    is.open(input);
    
    if(is.is_open())
    {
        string line;
        int i = 1;
        while (getline(is, line))
        {
            if(i == 1)
            {
                unsigned long index = line.find_first_of(" ");
                
                if(index < line.length())
                {
                    string temp  = line.substr(0, line.find_first_of(" "));
                    startTime = atoi(temp.c_str());
                }
                else
                {
                    startTime = atoi(line.c_str());
                }
            }
            else if(i == 2)
            {
                unsigned long index = line.find_first_of(" ");
                
                if(index < line.length())
                {
                    string temp  = line.substr(0, line.find_first_of(" "));
                    captital = atof(temp.c_str());
                }
                else
                {
                    captital = atof(line.c_str());
                }
            }
            else if(i == 3)
            {
                unsigned long index = line.find_first_of(" ");
                
                if(index < line.length())
                {
                    string temp  = line.substr(0, line.find_first_of(" "));
                    bankRate = atof(temp.c_str()) / 100.0 + 1;
                }
                else
                {
                    bankRate = atof(line.c_str()) / 100.0 + 1;
                }
            }
            else if(i == 4)
            {
                unsigned long index = line.find_first_of(" ");
                
                if(index < line.length())
                {
                    string temp  = line.substr(0, line.find_first_of(" "));
                    timeInvest = atoi(temp.c_str());
                }
                else
                {
                    timeInvest = atoi(line.c_str());
                }
                
            }
            else if(i > 5)
            {
                Stock s;
                
                unsigned long i = line.find(" ", 0);
                s.name = line.substr(0, i);
                line = line.substr(i + 1);
                
                i = line.find(" ", 0);
                s.cost = atof(line.substr(0, i).c_str());
                line = line.substr(i + 1);
                
                i = line.find(" ", 0);
                s.limit = atof(line.substr(0, i).c_str());
                line = line.substr(i + 1);
                
                int start = 0;
                int m = 0;
                
                for(int i = 0; i < 12; ++i)
                {
                    s.dividend[i] = 0;
                }
                
                for(int i = 0; i < line.length(); ++i)
                {
                    if(line[i] == '(')
                    {
                        m = atoi(line.substr(start, i).c_str());
                        m %= 12;
                        
                        start = i + 1;
                    }
                    else if(line[i] == ')')
                    {
                        s.dividend[m] = atoi(line.substr(start, i).c_str());
                        start = i + 1;
                    }
                    else if(line[i] < '0' && line[i] < '9')
                    {
                        start++;
                    }
                }
                
                listOfStock.push_back(s);
            }
            ++i;
        }
    }
}

void Problem::Solve1(string outPutName)
{
    unsigned long startT = time(NULL);
    
    glp_prob *lp;
    lp = glp_create_prob();
    glp_create_index(lp);     //for use glp_find_col function
    
    //Add variable and set name;
    int colums = 4 * int(listOfStock.size()) * (timeInvest + 1);
    glp_add_cols(lp, colums);
    int ic = 0;
    
    for(int i = 0; i <= timeInvest; ++i)
    {
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            stringstream name1;
            name1 << "x1_" << j << "_" << i;
            glp_set_col_name(lp, ++ic, name1.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            
            if(i != 0)
                glp_set_col_bnds(lp, ic, GLP_DB, 0, 1);
            else
                glp_set_col_bnds(lp, ic, GLP_FX, 0, 0);
            
            stringstream name0;
            name0 << "x0_" << j << "_" << i;
            glp_set_col_name(lp, ++ic, name0.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            glp_set_col_bnds(lp, ic, GLP_DB, 0, 1);
            
            
            stringstream buyName;
            buyName << "m_" << j << "_" << i;
            stringstream sellName;
            sellName << "b_" << j << "_" << i;
            
            glp_set_col_name(lp, ++ic, buyName.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            glp_set_col_bnds(lp, ic, GLP_LO, 0, 0);
            
            glp_set_col_name(lp, ++ic, sellName.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            glp_set_col_bnds(lp, ic, GLP_LO, 0, 0);
        }
    }
    
    
    //Constraint 1 is satisfied
    //Constraint 2 is satisfied
    //Constraint 3
    for(int i = 0; i < listOfStock.size(); ++i)
    {
        for(int k = 1; k <= timeInvest; ++k)
        {
            int count = 1;
            int *ind = new int[2 * k + 1 + 1];
            double *val = new double[2 * k + 1 + 1];
            
            for(int j = 0; j < k; ++j)
            {
                stringstream name;
                name << "b_" << i << "_" << j;
                string xx = name.str();
                ind[count] = glp_find_col(lp, name.str().c_str());
                val[count++] = 1;
                
                stringstream mName;
                mName << "m_" << i << "_" << j;
                ind[count] = glp_find_col(lp, mName.str().c_str());
                val[count++] = -1;
            }
            stringstream sName;
            sName << "b_" << i << "_" << k;
            ind[count] = glp_find_col(lp, sName.str().c_str());
            val[count++] = 1;
            
            int index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2 * k + 1, ind, val);
            glp_set_row_bnds(lp, index, GLP_UP, 0, 0);
            
            delete [] ind;
            delete [] val;
        }
    }
    
    //Constraint 4
    for(int i = 0; i <= timeInvest; ++i)
    {
        vector<int> ind;
        vector<double> val;
        ind.push_back(0);  //igrored ind[0], val[0]
        val.push_back(0);
        
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            double *arr0 = new double[i + 1];
            double *arr1 = new double[i + 1];
            
            for(int k = 0; k < i; ++k)
            {
                arr0[k] = listOfStock[j].cost * pow(bankRate, i - k);
            }
            
            arr1[0] = 0;
            for(int k = 1; k < i; ++k)
            {
                arr1[k] = - listOfStock[j].cost * pow(bankRate, i - k - 1);
            }
            
            for(int k = 1; k < i; ++k)
            {
                int time = (k + startTime) % 12;
                
                for(int h = 0; h < k; ++h)
                {
                    arr0[h] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                    
                    arr1[h] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                }
                arr1[k] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
            }
            
            for(int n = 0; n < i; ++n)
            {
                
                stringstream mName;
                stringstream bName;
                mName << "m_" << j << "_" << n;
                bName << "b_" << j << "_" << n;
                
                ind.push_back(glp_find_col(lp, mName.str().c_str()));
                val.push_back(arr0[n]);
                ind.push_back(glp_find_col(lp, bName.str().c_str()));
                val.push_back(arr1[n]);
            }
            
            delete [] arr1;
            delete [] arr0;
        }
        
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            stringstream name;
            name << "m_" << j << "_" << i;
            ind.push_back(glp_find_col(lp, name.str().c_str()));
            val.push_back(listOfStock[j].cost);
        }
        
        int index = glp_add_rows(lp, 1);
        glp_set_mat_row(lp, index, (int)ind.size() - 1, ind.data(), val.data());
        stringstream rowName;
        rowName << "T" << i;
        glp_set_row_name(lp, index, rowName.str().c_str());        //use in writeOuPut
        
        
        double temp = captital * pow(bankRate, i);
        glp_set_row_bnds(lp, index, GLP_UP, 0, temp);
    }
    
    //Constraint 5 is satisfied by 7
    
    //Constraint 6
    for(int j = 0; j < listOfStock.size(); ++j)
    {
        for(int t = 1; t <= timeInvest; ++t)
        {
            stringstream x0;
            stringstream x1;
            x0 << "x0_" << j << "_" << t;
            x1 << "x1_" << j << "_" << t;
            
            int ind[3];
            double val[3];
            
            ind[1] = glp_find_col(lp, x0.str().c_str());
            val[1] = 1;
            ind[2] = glp_find_col(lp, x1.str().c_str());
            val[2] = 1;
            
            int index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2, ind, val);
            glp_set_row_bnds(lp, index, GLP_UP, 0, 1);
        }
    }
    
    //Constraint 7
    for(int j = 0; j < listOfStock.size(); ++j)
    {
        for(int t = 0; t <= timeInvest; ++t)
        {
            vector<int> ind0;
            vector<double> val0;
            ind0.push_back(0);         //ignored ind[0], val[0] - GLPK requirement
            val0.push_back(0);
            
            vector<int> ind1;
            vector<double> val1;
            ind1.push_back(0);         //ignored ind[0], val[0] - GLPK requirement
            val1.push_back(0);
            
            stringstream x0Name, x1Name, y0Name, y1Name;
            x0Name << "x0_" << j << "_" << t;
            x1Name << "x1_" << j << "_" << t;
            y0Name << "m_" << j << "_" << t;
            y1Name << "b_" << j << "_" << t;
            
            ind0.push_back(glp_find_col(lp, x0Name.str().c_str()));
            val0.push_back(-listOfStock[j].limit);
            ind0.push_back(glp_find_col(lp, y0Name.str().c_str()));
            val0.push_back(1);
            
            ind1.push_back(glp_find_col(lp, x1Name.str().c_str()));
            val1.push_back(-listOfStock[j].limit);
            ind1.push_back(glp_find_col(lp, y1Name.str().c_str()));
            val1.push_back(1);
            
            int index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2, ind0.data(), val0.data());
            glp_set_row_bnds(lp, index, GLP_UP, 0, 0);
            
            index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2, ind1.data(), val1.data());
            glp_set_row_bnds(lp, index, GLP_UP, 0, 0);
        }
    }
    
    //Objective func
    int i = timeInvest + 1;
    for(int j = 0; j < listOfStock.size(); ++j)
    {
        double *arr0 = new double[i + 1];
        double *arr1 = new double[i + 1];
        vector<int> ind;
        vector<double> val;
        ind.push_back(0);  //igrored ind[0], val[0]
        val.push_back(0);
        
        for(int k = 0; k < i; ++k)
        {
            arr0[k] = - listOfStock[j].cost * pow(bankRate, i - k);
        }
        
        arr1[0] = 0;
        for(int k = 1; k < i; ++k)
        {
            arr1[k] = + listOfStock[j].cost * pow(bankRate, i - k - 1);
        }
        
        for(int k = 1; k < i; ++k)
        {
            int time = (k + startTime) % 12;
            
            for(int h = 0; h < k; ++h)
            {
                arr0[h] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                
                arr1[h] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
            }
            arr1[k] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
        }
        
        for(int n = 0; n < i; ++n)
        {
            stringstream mName;
            stringstream bName;
            mName << "m_" << j << "_" << n;
            bName << "b_" << j << "_" << n;
            
            glp_set_obj_coef(lp, glp_find_col(lp, mName.str().c_str()), arr0[n]);
            glp_set_obj_coef(lp, glp_find_col(lp, bName.str().c_str()), arr1[n]);
        }
        
        delete [] arr1;
        delete [] arr0;
    }
    
    glp_set_obj_dir(lp, GLP_MAX);
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    parm.gmi_cuts = GLP_ON;
    parm.mir_cuts = GLP_ON;
    parm.cov_cuts = GLP_ON;
    parm.clq_cuts = GLP_ON;
    
    glp_intopt(lp, &parm);
    //glp_print_mip(lp, "solve1.txt");
    
    timeSolve = time(NULL) - startT;
    WriteResult(outPutName, lp);
    
    glp_delete_prob(lp);
}

void Problem::Solve3(string outPutName)
{
    unsigned long startT = time(NULL);
    
    glp_prob *lp;
    lp = glp_create_prob();
    glp_create_index(lp);     //for use glp_find_col function
    
    //Add variable and set name;
    int colums = 3 * int(listOfStock.size()) * (timeInvest + 1);
    glp_add_cols(lp, colums);
    int ic = 0;
    
    for(int i = 0; i <= timeInvest; ++i)
    {
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            stringstream name;
            name << "x_" << j << "_" << i;
            glp_set_col_name(lp, ++ic, name.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            
            if(i != 0)
                glp_set_col_bnds(lp, ic, GLP_DB, 0, 1);
            else
                glp_set_col_bnds(lp, ic, GLP_FX, 0, 0);
            
            stringstream buyName;
            buyName << "m_" << j << "_" << i;
            stringstream sellName;
            sellName << "b_" << j << "_" << i;
            
            glp_set_col_name(lp, ++ic, buyName.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            glp_set_col_bnds(lp, ic, GLP_LO, 0, 0);
            
            glp_set_col_name(lp, ++ic, sellName.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            glp_set_col_bnds(lp, ic, GLP_LO, 0, 0);
        }
    }
    
    
    //Constraint 20 is satisfied
    //Constraint 21
    for(int i = 0; i < listOfStock.size(); ++i)
    {
        for(int k = 1; k <= timeInvest; ++k)
        {
            int count = 1;
            int *ind = new int[2 * k + 1 + 1];
            double *val = new double[2 * k + 1 + 1];
            
            for(int j = 0; j < k; ++j)
            {
                stringstream name;
                name << "b_" << i << "_" << j;
                string xx = name.str();
                ind[count] = glp_find_col(lp, name.str().c_str());
                val[count++] = 1;
                
                stringstream mName;
                mName << "m_" << i << "_" << j;
                ind[count] = glp_find_col(lp, mName.str().c_str());
                val[count++] = -1;
            }
            stringstream sName;
            sName << "b_" << i << "_" << k;
            ind[count] = glp_find_col(lp, sName.str().c_str());
            val[count++] = 1;
            
            int index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2 * k + 1, ind, val);
            glp_set_row_bnds(lp, index, GLP_UP, 0, 0);
            
            delete [] ind;
            delete [] val;
        }
    }
    
    //Constraint 22
    for(int i = 0; i <= timeInvest; ++i)
    {
        vector<int> ind;
        vector<double> val;
        ind.push_back(0);  //igrored ind[0], val[0]
        val.push_back(0);
        
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            double *arr0 = new double[i + 1];
            double *arr1 = new double[i + 1];
            
            for(int k = 0; k < i; ++k)
            {
                arr0[k] = listOfStock[j].cost * pow(bankRate, i - k);
            }
            
            arr1[0] = 0;
            for(int k = 1; k < i; ++k)
            {
                arr1[k] = - listOfStock[j].cost * pow(bankRate, i - k - 1);
            }
            
            for(int k = 1; k < i; ++k)
            {
                int time = (k + startTime) % 12;
                
                for(int h = 0; h < k; ++h)
                {
                    arr0[h] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                    
                    arr1[h] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                }
                arr1[k] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
            }
            
            for(int n = 0; n < i; ++n)
            {
                
                stringstream mName;
                stringstream bName;
                mName << "m_" << j << "_" << n;
                bName << "b_" << j << "_" << n;
                
                ind.push_back(glp_find_col(lp, mName.str().c_str()));
                val.push_back(arr0[n]);
                ind.push_back(glp_find_col(lp, bName.str().c_str()));
                val.push_back(arr1[n]);
            }
            
            delete [] arr1;
            delete [] arr0;
        }
        
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            stringstream name;
            name << "m_" << j << "_" << i;
            ind.push_back(glp_find_col(lp, name.str().c_str()));
            val.push_back(listOfStock[j].cost);
        }
        
        int index = glp_add_rows(lp, 1);
        glp_set_mat_row(lp, index, (int)ind.size() - 1, ind.data(), val.data());
        stringstream rowName;
        rowName << "T" << i;
        glp_set_row_name(lp, index, rowName.str().c_str());        //use in writeOuput Function
        
        double temp = captital * pow(bankRate, i);
        glp_set_row_bnds(lp, index, GLP_UP, 0, temp);
    }
    
    //Constraint 23
    for(int i = 0; i < listOfStock.size(); ++i)
    {
        int *ind1 = new int[3];
        double *val1 = new double[3];
        int *ind2 = new int[3];
        double *val2 = new double[3];
        
        for(int k = 0; k <= timeInvest; ++k)
        {
            stringstream xName, yName1, yName2;
            xName << "x_" << i << "_" << k;
            yName1 << "m_" << i << "_" << k;
            yName2 << "b_" << i << "_" << k;
            
            ind1[1] = glp_find_col(lp, xName.str().c_str());
            val1[1] = - listOfStock[i].limit;
            ind1[2] = glp_find_col(lp, yName2.str().c_str());
            val1[2] = 1;
            
            int index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2, ind1, val1);
            glp_set_row_bnds(lp, index, GLP_UP, 0, 0);
            
            ind2[1] = ind1[1];
            val2[1] = - val1[1];
            ind2[2] = glp_find_col(lp, yName1.str().c_str());
            val2[2] = 1;
            index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2, ind2, val2);
            glp_set_row_bnds(lp, index, GLP_UP, 0, listOfStock[i].limit);
        }
        
        delete [] ind1;
        delete [] ind2;
        delete [] val1;
        delete [] val2;
    }
    
    //Constraints 23, 24 are satisfied in set_bdns
    //Objective func
    int i = timeInvest + 1;
    for(int j = 0; j < listOfStock.size(); ++j)
    {
        double *arr0 = new double[i + 1];
        double *arr1 = new double[i + 1];
        vector<int> ind;
        vector<double> val;
        ind.push_back(0);                //igrored ind[0], val[0]
        val.push_back(0);
        
        for(int k = 0; k < i; ++k)
        {
            arr0[k] = - listOfStock[j].cost * pow(bankRate, i - k);
        }
        
        arr1[0] = 0;
        for(int k = 1; k < i; ++k)
        {
            arr1[k] = + listOfStock[j].cost * pow(bankRate, i - k - 1);
        }
        
        for(int k = 1; k < i; ++k)
        {
            int time = (k + startTime) % 12;
            
            for(int h = 0; h < k; ++h)
            {
                arr0[h] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                
                arr1[h] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
            }
            arr1[k] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
        }
        
        for(int n = 0; n < i; ++n)
        {
            stringstream mName;
            stringstream bName;
            mName << "m_" << j << "_" << n;
            bName << "b_" << j << "_" << n;
            
            glp_set_obj_coef(lp, glp_find_col(lp, mName.str().c_str()), arr0[n]);
            glp_set_obj_coef(lp, glp_find_col(lp, bName.str().c_str()), arr1[n]);
        }
        
        delete [] arr1;
        delete [] arr0;
    }
    
    glp_set_obj_dir(lp, GLP_MAX);
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    parm.gmi_cuts = GLP_ON;
    parm.mir_cuts = GLP_ON;
    parm.cov_cuts = GLP_ON;
    parm.clq_cuts = GLP_ON;
    
    glp_intopt(lp, &parm);
    //glp_print_mip(lp, "solve3.txt");     //for debug
    
    timeSolve = time(NULL) - startT;
    WriteResult(outPutName, lp);
    
    glp_delete_prob(lp);
}

void Problem::Solve4(string outPutName)
{
    unsigned long startT = time(NULL);
    
    glp_prob *lp;
    lp = glp_create_prob();
    glp_create_index(lp);     //for use glp_find_col function
    
    //Add variable and set name;
    int colums = 4 * int(listOfStock.size()) * (timeInvest + 1);
    glp_add_cols(lp, colums);
    int ic = 0;
    
    for(int i = 0; i <= timeInvest; ++i)
    {
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            stringstream buyName;
            buyName << "m_" << j << "_" << i;
            stringstream sellName;
            sellName << "b_" << j << "_" << i;
            
            glp_set_col_name(lp, ++ic, buyName.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            glp_set_col_bnds(lp, ic, GLP_DB, 0, listOfStock[j].limit);
            
            glp_set_col_name(lp, ++ic, sellName.str().c_str());
            glp_set_col_kind(lp, ic, GLP_IV);
            glp_set_col_bnds(lp, ic, GLP_DB, 0, listOfStock[j].limit);
            
        }
    }
    
    //Constraint 26 is satisfied by 27
    //Constraint 27
    for(int i = 0; i < listOfStock.size(); ++i)
    {
        for(int k = 1; k <= timeInvest; ++k)
        {
            int count = 1;
            int *ind = new int[2 * k + 1 + 1];
            double *val = new double[2 * k + 1 + 1];
            
            for(int j = 0; j < k; ++j)
            {
                stringstream name;
                name << "b_" << i << "_" << j;
                string xx = name.str();
                ind[count] = glp_find_col(lp, name.str().c_str());
                val[count++] = 1;
                
                stringstream mName;
                mName << "m_" << i << "_" << j;
                ind[count] = glp_find_col(lp, mName.str().c_str());
                val[count++] = -1;
            }
            stringstream sName;
            sName << "b_" << i << "_" << k;
            ind[count] = glp_find_col(lp, sName.str().c_str());
            val[count++] = 1;
            
            int index = glp_add_rows(lp, 1);
            glp_set_mat_row(lp, index, 2 * k + 1, ind, val);
            glp_set_row_bnds(lp, index, GLP_UP, 0, 0);
            
            delete [] ind;
            delete [] val;
        }
    }
    
    //Constraint 28
    for(int i = 0; i <= timeInvest; ++i)
    {
        vector<int> ind;
        vector<double> val;
        ind.push_back(0);  //igrored ind[0], val[0]
        val.push_back(0);
        
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            double *arr0 = new double[i + 1];
            double *arr1 = new double[i + 1];
            
            for(int k = 0; k < i; ++k)
            {
                arr0[k] = listOfStock[j].cost * pow(bankRate, i - k);
            }
            
            arr1[0] = 0;
            for(int k = 1; k < i; ++k)
            {
                arr1[k] = - listOfStock[j].cost * pow(bankRate, i - k - 1);
            }
            
            for(int k = 1; k < i; ++k)
            {
                int time = (k + startTime) % 12;
                
                for(int h = 0; h < k; ++h)
                {
                    arr0[h] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                    
                    arr1[h] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                }
                arr1[k] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
            }
            
            for(int n = 0; n < i; ++n)
            {
                
                stringstream mName;
                stringstream bName;
                mName << "m_" << j << "_" << n;
                bName << "b_" << j << "_" << n;
                
                ind.push_back(glp_find_col(lp, mName.str().c_str()));
                val.push_back(arr0[n]);
                ind.push_back(glp_find_col(lp, bName.str().c_str()));
                val.push_back(arr1[n]);
            }
            
            delete [] arr1;
            delete [] arr0;
        }
        
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            stringstream name;
            name << "m_" << j << "_" << i;
            ind.push_back(glp_find_col(lp, name.str().c_str()));
            val.push_back(listOfStock[j].cost);
        }
        
        int index = glp_add_rows(lp, 1);
        glp_set_mat_row(lp, index, (int)ind.size() - 1, ind.data(), val.data());
        stringstream rowName;
        rowName << "T" << i;
        glp_set_row_name(lp, index, rowName.str().c_str());       //for using in WriteOutPut
        
        double temp = captital * pow(bankRate, i);
        glp_set_row_bnds(lp, index, GLP_UP, 0, temp);
    }
    
    //Constraint 29 is satisfied in in setting row_bdns
    //Objective func
    int i = timeInvest + 1;
    for(int j = 0; j < listOfStock.size(); ++j)
    {
        double *arr0 = new double[i + 1];
        double *arr1 = new double[i + 1];
        vector<int> ind;
        vector<double> val;
        ind.push_back(0);  //igrored ind[0], val[0]
        val.push_back(0);
        
        for(int k = 0; k < i; ++k)
        {
            arr0[k] = - listOfStock[j].cost * pow(bankRate, i - k);
        }
        
        arr1[0] = 0;
        for(int k = 1; k < i; ++k)
        {
            arr1[k] = + listOfStock[j].cost * pow(bankRate, i - k - 1);
        }
        
        for(int k = 1; k < i; ++k)
        {
            int time = (k + startTime) % 12;
            
            for(int h = 0; h < k; ++h)
            {
                arr0[h] += listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
                
                arr1[h] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
            }
            arr1[k] -= listOfStock[j].dividend[time] * .1 * pow(bankRate, i - k - 1);
        }
        
        for(int n = 0; n < i; ++n)
        {
            stringstream mName;
            stringstream bName;
            mName << "m_" << j << "_" << n;
            bName << "b_" << j << "_" << n;
            
            glp_set_obj_coef(lp, glp_find_col(lp, mName.str().c_str()), arr0[n]);
            glp_set_obj_coef(lp, glp_find_col(lp, bName.str().c_str()), arr1[n]);
        }
        
        delete [] arr1;
        delete [] arr0;
    }
    
    glp_set_obj_dir(lp, GLP_MAX);
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.presolve = GLP_ON;
    parm.gmi_cuts = GLP_ON;
    parm.mir_cuts = GLP_ON;
    parm.cov_cuts = GLP_ON;
    parm.clq_cuts = GLP_ON;
    //parm.fp_heur = GLP_ON;
    
    glp_intopt(lp, &parm);
    //glp_print_mip(lp, "solve1.txt");    //for debug
    
    timeSolve = time(NULL) - startT;
    WriteResult(outPutName, lp);
    
    glp_delete_prob(lp);
}

void Problem::WriteResult(string fileName, glp_prob *lp)
{
    ofstream os;
    os.open(fileName);
    
    unsigned long colums = glp_get_num_cols(lp) + 1;
    
    for(int i = 1; i < colums; ++i)
    {
        long val = glp_mip_col_val(lp, i);
        if(val == 0)
            continue;
        
        string name = glp_get_col_name(lp, i);
        if(name[0] == 'm')
        {
            int index = atoi(name.substr(2, name.find("_", 3) - 1).c_str());
            int month = atoi(name.substr(name.find("_", 3) + 1).c_str());
            
            listOfStock[index].month.push_back(month);
            listOfStock[index].change.push_back(val);
            
        }
        else if(name[0] == 'b')
        {
            int index = atoi(name.substr(2, name.find("_", 3) - 1).c_str());
            int month = atoi(name.substr(name.find("_", 3) + 1).c_str());
            
            listOfStock[index].month.push_back(month);
            listOfStock[index].change.push_back(-val);
        }
    }
    
    os << timeSolve << "s" << endl;
    os << glp_mip_obj_val(lp) + captital * pow(bankRate, timeInvest + 1) << endl << endl;
    
    for(int i = 0; i <= timeInvest; i++)
    {
        int month = (startTime + i) % 12;
        month = !month ? 12 : month;
        
        stringstream rowName;
        rowName << "T" << i;
        double mn = glp_mip_row_val(lp, glp_find_row(lp, rowName.str().c_str()));
        mn = captital * pow(bankRate, i) - mn;
        
        if(month > 9)
        {
            os << "T" << month << " " << "M" << i << " ";
        }
        else
        {
            os << "T0" << month << " " << "M" << i << " ";
        }
        
        stringstream oss;
        
        for(int j = 0; j < listOfStock.size(); ++j)
        {
            for(int k = 0; k < listOfStock[j].month.size(); ++k)
            {
                if(listOfStock[j].month[k] == i)
                {
                    oss << listOfStock[j].change[k] << " " << listOfStock[j].name << " ";
                    
                    if(listOfStock[j].change[k] > 0)
                    {
                        mn += listOfStock[j].change[k] * listOfStock[j].cost;
                    }
                    break;
                }
            }
        }
        os << mn << " " << oss.str().c_str() << endl;
    }
    
    os.close();
}

