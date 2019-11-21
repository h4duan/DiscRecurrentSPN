#include "Node.h"
#include <math.h>
#include <iostream>
using namespace std;


void Node::set_parent(Node *parent){
    parents.push_back(parent->id);
}


void ProductNode::add_child(Node *child){
    //cout << id << endl;
    //cout << child->id << endl;
    this->children.push_back(child->id);
    child->set_parent(this);
}



void Node::set_id(int id){
    this->id = id;
}


int Node::get_id(){
    return id;
}

void SumNode::set_observe(double observe){
   (void) observe;
    return;
}


void SumNode::set_class(int y){
    this->y = y;
}

SumNode::SumNode(){
    leaves = false;
}


void SumNode::value(vector<double> &fn){
    //cout << "evaluate sum: " << id << endl;

    double val = 0.0;

    //cout << "here" << endl;
    int size = children.size();

    //cout << "size of children:" << size << endl;

    for(int i = 0; i < size; i++){
        //cout << size << endl;
        //cout << "weight:  " << weight.size() << endl;
        val += exp(log(weight[i]) + log(fn[children[i]]));
    }

    //cout << val << endl;
    //return val;
    //
    fn[id] = val;
}

double epsilon = 0.01;

void SumNode::add_child(Node *child){
    //cout << "sum node add child" << endl;
    this->children.push_back(child->id);
    child->set_parent(this);
    int n = this->children.size();
    vector<double> w(n, 1.0/(double)n);
    vector<double> w2(n, 0.0);

    /*double sum = 0;

    for(int i = 0; i < n-1; i++){
        if(i%2 == 0){
            w[i] -= (i+1)*epsilon;
        } else{
            w[i] += (i+1)*epsilon;
        }
        sum += w[i];
    }

    w[n-1] = 1.0 - sum;*/


    this->weight = w;
    this->new_weight = w2;
}

ProductNode::ProductNode(){
    leaves = false;
}

void ProductNode::value(vector<double> &fn){
   //cout << "evaluate product" << endl;

   double val = 0.0;
   int size = this->children.size();
   for(int i = 0; i < size; i++){
        val += log(fn[children[i]]);
   }

   //cout << val << endl;
   fn[id] = exp(val);
   //return exp(val);
}


void ProductNode::set_observe(double observe){
    (void) observe;
    return;
}


DiscVarNode::DiscVarNode(int level):
    Node{}{
    vector<double> vect(level, 1.0/double(level));
    p = vect;
    leaves = true;
    //cout << p[0] << "   "  << p[1] << endl;
}

void DiscVarNode::add_child(Node *child){
    (void)child;
    return;
}


void DiscVarNode::value(vector<double> &fn){
    //cout << "Disc Evaluate:  " << id << "  " << p[observe] << endl;
    //return p[observe];
    fn[id] = p[observe];
}

void DiscVarNode::set_observe(double observe){
    this->observe = (int)observe;
}


double normal_distribution(double mean, double sigma, double observe){
    double x =  exp(-(observe - mean)*(observe - mean)/(2*sigma))/(sqrt(2*PI*sigma));
    return x;
}


void ContVarNode::value(vector<double> &fn){
    fn[id] =  normal_distribution(mean, sigma, observe);
}

void ContVarNode::set_observe(double observe){
    this->observe = observe;
}

void ContVarNode::add_child(Node *child){
    (void)child;
    return;
}

ContVarNode::ContVarNode(){
    leaves = true;
}

Node::~Node(){

}




