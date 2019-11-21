#include "Node.h"
#include <vector>
using namespace std;

#ifndef _SPN_H
#define _SPN_H

class SPN{

   public:

       int number_of_variables;

       int level;

       int second_layer;

       //bool root_sum;

       int number_of_nodes;

       vector<Node *> nodes;

       Node *initialSpn(vector<int> scope, vector<Node *> leaves, int y);

       void initialModel();

       void initialWeight();

       void initialFn();

       void initialDn();

       vector<double> fn;

       vector<double> dn;
        
       Node *root;

       void putInput(vector<double> observe);

       double derivative(int i, int j);

       double eval();

       SPN(int number_of_variables, int level,  int second_layer);

       ~SPN();
        
};

void printVector(vector<double> vect);


void printVectorInt(vector<int> an);




#endif
