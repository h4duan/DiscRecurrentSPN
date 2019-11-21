#include "SPN.h"
#include <assert.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "stdlib.h"
#include <random>
using namespace std;



int total = 0;

Node *SPN::initialSpn(vector<int> scope, vector<Node *> leaves, int y){
    //printVectorInt(scope);

    int n = scope.size();

    int times = min(3, n);

    if(n == 1){
        int pos = rand()%2;
        //cout << scope[0] << endl;
        return leaves[(scope[0]+1)*2-pos-1];
    } else if(n == 2){
        SumNode *root = new SumNode{};
        root->set_class(y);
        total++;
        root->set_id(total);
        nodes.push_back(root);

        for(int i = 0; i < 2; i++){
            ProductNode *second = new ProductNode{};
            total++;
            second->set_id(total);
            nodes.push_back(second);
            root->add_child(second);
        }
        
        int pos_left = rand()%2;
        int pos_right = rand()%2;


        nodes[total]->add_child(leaves[(scope[0]+1)*2-1-pos_left]);
        //cout << (scope[0]+1)*2-1-pos_left << endl;
        nodes[total]->add_child(leaves[(scope[1]+1)*2-1-pos_right]);
        //cout << (scope[1]+1)*2-1-pos_right << endl;
        //cout << "left end" << endl;
 
        nodes[total-1]->add_child(leaves[(scope[0]+1)*2-1-(1-pos_left)]);
        nodes[total-1]->add_child(leaves[(scope[1]+1)*2-1-(1-pos_right)]);

        /*cout << (scope[0]+1)*2-1-(1-pos_left) << endl;
        cout << (scope[1]+1)*2-1-(1-pos_right) << endl;
        cout << "left end" << endl;*/

        return root;

    } else{
        total++;
        SumNode *root = new SumNode{};
        root->set_id(total);
        root->set_class(y);
        nodes.push_back(root);

        for(int i = 0; i < times; i++){
            ProductNode *second = new ProductNode{};
            total++;
            second->set_id(total);
            //second->set_class(y);
            nodes.push_back(second);
            root->add_child(second);
        }

        //cout << times << endl;


        for(int i = 0; i < times; i++){
              int m = rand()%(n-1) + 1;
              vector<int> left(scope.begin(), scope.begin()+m);
              vector<int> right(scope.begin()+m, scope.end());
              //cout << n << endl;
              //cout << right.size() << endl;
              Node *leftChild = initialSpn(left, leaves, y);
              Node *rightChild = initialSpn(right, leaves, y);
              //printVectorInt(root->children);
              //cout << rightChild->id << endl;
              nodes[root->children[i]]->add_child(leftChild);
              nodes[root->children[i]]->add_child(rightChild);
          }

        return root;
    }
}


void SPN::initialModel(){
    vector<Node *> leaves;
    
    vector<int> scope(number_of_variables, 0);

    for(int i = 0; i < number_of_variables; i++){
        scope[i] = i;
    }

    for(int i = 0; i < number_of_variables*2; i++){
       leaves.push_back(new ContVarNode{});
       leaves[i]->set_id(-(i+1));
    }

    root = new SumNode{};

    root->set_id(0);

    //total++;

    nodes.push_back(root);

    for(int i = 0; i < second_layer; i++){
        ProductNode *second = new ProductNode{};
        total++;
        second->set_id(total);
        nodes.push_back(second);
        root->add_child(second);
    }
       
    //int num_interior = total;

    for(int i = 0; i < second_layer; i++){
        int pos_cut = rand()%(number_of_variables-1)+1;
        vector<int> left(scope.begin(), scope.begin()+pos_cut);
        vector<int> right(scope.begin()+pos_cut, scope.end());
        //printVectorInt(left);
        //printVectorInt(right);
        //cout << left.size() << "  " << right.size() << endl;
        Node *left_child = initialSpn(left, leaves, i);
        Node *right_child = initialSpn(right, leaves, i);
        nodes[i+1]->add_child(left_child);
        nodes[i+1]->add_child(right_child);
    }

    int num_interior = total;

    for(int i = 0; i < 2*number_of_variables; i++){
        total++;
        leaves[i]->set_id(total);
        nodes.push_back(leaves[i]);
    }


    for(auto &node : nodes){
        for(auto &child : node->children){
            if (child < 0){
               child = -child+num_interior;
            }
        }
    }
    //cout << nodes.size() << endl;
}



void SPN::initialWeight(){
       for(auto& node : nodes){
        if(!node->weight.empty()){
          double sum = 0.0;
          int num_children = node->children.size();
          //vector<double> p{num_children, 1.0/num_children};
          for(int i = 0; i < num_children; i++){
             //srand(time(NULL));
             node->weight[i] = (double)rand() / RAND_MAX;          
             sum += node->weight[i];
          }
          for(int i = 0; i < num_children; i++){
             node->weight[i] = node->weight[i]/sum;
          }
         } else if(node->leaves){
             ((ContVarNode *)node)->mean =(double)rand() / RAND_MAX; 
             ((ContVarNode *)node)->sigma =(double)rand() / RAND_MAX; 
         }
    }
}

void SPN::initialFn(){

    //cout << "initial" << endl;
   
    int size =  nodes.size();

    vector<double> vect(size, 0.0);
    fn = vect;
        
    //int height = nodes.size();
   
    for(int i = 0; i < size; i++){
        nodes[size-1-i]->value(fn);
    }     

    //cout << "This is value of each node" << endl;
    //for(int i = 0; i < size; i++){
        //cout << i << " , " << fn[i] << "     ";
   // }    
    //cout << endl;    
}



void SPN::initialDn(){
    int n = nodes.size();

     vector<double> vect(n, 0.0);
     dn = vect;
     dn[0] = 1.0;
     
     /*for(int i = 1; i < 5; i++){
         int n = nodes[i].size();
         if(i % 2 == 0){
             for(int j = 0; j < n; j++){
                Node *pare = nodes[i][j];
                int num_children = pare->children.size();
                for(int k = 0; k < num_children; k++){
                    Node *child = pare->children[k];                    
                    dn[child->id] += exp(log(((SumNode *)pare)->weight[k]) + log(dn[pare->id])); 
                }
             }
         }else{
           for(int j = 0; j < n; j++){
                Node *pare = nodes[i][j];
                int num_children = pare->children.size();
                for(int k = 0; k < num_children; k++){
                    Node *child = pare->children[k];                    
                    //dn[child->id] += exp(log(((SumNode *)pare)->weight[k]) + log(dn[pare->id]); 
                    double log_product = log(dn[pare->id]);
                    for(int m = 0; m < num_children; m++){
                        if(m != k){
                           log_product += log(fn[pare->children[m]->id]);
                        }
                    }
                    dn[child->id] += exp(log_product);
                }
             }
        }
     }*/
     
     for(int i = 0; i < n-number_of_variables; i++){
         if((nodes[i]->weight).empty()){
             int num_children = nodes[i]->children.size();
             for(int j = 0; j < num_children; j++){
                dn[nodes[i]->children[j]] += exp(log(dn[nodes[i]->id])+log(fn[nodes[i]->id])-log(fn[nodes[i]->children[j]]));
             }
         } else{
             int num_children = nodes[i]->children.size();
             for(int j =0; j < num_children;j++){
                 dn[nodes[i]->children[j]] += exp(log(nodes[i]->weight[j]) + log(dn[nodes[i]->id]));
            }
         }
     }
}



double SPN:: derivative(int i, int j){
    assert(i >= 0);
    assert(j > 0);
    assert(i < j);

    return exp(log(fn[j]) + log(dn[i]));
}


void printVector(vector<double> vect){
    for(auto &i : vect){
        cout << i << "  ";
    }
    cout << endl;
}

void printVectorInt(vector<int> vect){
    for(auto &i : vect){
        cout << i << "  ";
    }
    cout << endl;
}


void SPN::putInput(vector<double> observe){
    //cout << "here2: " << endl;
    //

    int n = nodes.size();

    int m = observe.size();

    int k = 0;

    //cout << n << endl;

    for(int i = 0; i < n; i++){
        //cout << observe[i-(n-number_of_variables)] << endl; 
        if(nodes[i]->leaves){
            nodes[i]->set_observe(observe[k]);
            k++;
        }
    }
    
    //initialWeight();

    initialFn();
    initialDn();

    //printVector(fn);

}

double SPN::eval(){
    return fn[0];
}


SPN::SPN(int number_of_variables, int level, int second_layer):
   number_of_variables{number_of_variables}, level{level}, second_layer{second_layer}{
        initialModel();
        initialWeight();
        //initialFn();
        //initialDn();
     /*int m = nodes.size();
     for(int i = 0; i < m; i++){
            cout << nodes[i]->id << ":  ";
            printVector(nodes[i]->weight);
            if(!nodes[i]->weight.empty()){
                cout << "class:  " << ((SumNode *)(nodes[i]))->y << endl;
            } else if(nodes[i]->leaves){
                cout << "mean: " << ((ContVarNode *)(nodes[i]))->mean << endl;
                cout << "sigma: " << ((ContVarNode *)(nodes[i]))->sigma << endl;
            }
            cout << "children:  ";
            printVectorInt(nodes[i]->children);
    }*/

        root = nodes[0];
    }


SPN::~SPN(){
    int n = nodes.size();

    for(int i = 0; i < n; i++){
        //cout << i << endl;
        delete nodes[i];
    }
}








