#include "Node.h"
#include "SPN.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "RSPN.h"
#include <vector>
#include <ctime>
#include <random>
#include <string>
#include "assert.h"
using namespace std;

vector<double> stringToDoubles(string s){
    vector<double> vect;
    stringstream mystring(s);
    double num;

    //cout << s << endl;

    while(mystring >> num){
        //mystring >> num;
        vect.push_back(num);
    }

    //printVector(vect);

    return vect;

}


int main(){
    srand(time(0));

    //SPN *spn = new SPN{5, 0, 2};

    //delete spn;

    string s;

    ifstream myfile;

    ifstream myfile2;

    myfile.open("ae_train.txt");

    vector<vector<vector<double>>> train_points;

    vector<vector<double>> blocks;

    //int i = 0;

    while(!myfile.eof()){
        getline(myfile, s);
        if(!s.empty()){
            blocks.push_back(stringToDoubles(s));
        } else{
            train_points.push_back(blocks);
            blocks.clear();
        }
    }

    string s2;

    myfile2.open("ae_test.txt");

    vector<vector<vector<double>>> test_points;

    vector<vector<double>> block;

    //int i = 0;

    while(!myfile2.eof()){
        getline(myfile2, s2);
        if(!s2.empty()){
            block.push_back(stringToDoubles(s2));
        } else{
            test_points.push_back(block);
            block.clear();
        }
    }
    vector<int> ys(270, 0);


    for(int i = 0; i < 270; i++){
        ys[i] = i / 30;
    }

    
    vector<int> result(370, 0);

    for(int i = 0; i < 370; i++){
        if(i < 31){
            result[i] = 0;
        } else if(i < 31+35){
            result[i] = 1;
        } else if(i < 31+35+88){
            result[i] = 2;
        } else if(i < 31+35+88+44){
            result[i] = 3;
        } else if(i < 31+35+88+44+29){
            result[i] = 4;
        } else if(i < 31+35+88+44+29+24){
            result[i] = 5;
        } else if(i < 31+35+88+44+29+24+40){
            result[i] = 6;
        } else if(i < 31+35+88+44+29+24+40+50){
            result[i] = 7;
        } else if(i < 31+35+88+44+29+24+40+50+29){
            result[i] = 8;
        } else{
            assert(0 == 0);
        }
    }

    vector<double> p(9, 1.0/9.0);

    //printVectorInt(result);
    
    
    ContRspn *rspn = new ContRspn{12, 9, p};

    rspn->update_ebw(train_points, 500, ys, test_points, result);  

    delete rspn;

}
