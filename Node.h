#include <vector>
using namespace std;


#ifndef _NODE_H
#define _NODE_H

const double PI = 3.141592653589793138463;


class Node{

   public:        
      int id = 0;

       bool leaves;

        vector<int> parents;
        
        vector<int> children;

        vector<double> weight;

        vector<double> new_weight;

        ~Node();
        
        virtual void value(vector<double> &fn) = 0;

        virtual void add_child(Node *child) = 0;

        //virtual void remove_child(Node *child) = 0;

        void set_parent(Node *parent);

        virtual void set_observe(double observe) = 0;

        void set_id(int id);

        int get_id();
};


class SumNode : public Node{

   public:

        //vector<double> weight;

        void add_child(Node *child);

        int y = -1;

        void value(vector<double> &fn);

        void set_observe(double observe);

        void set_class(int y);

        SumNode();
};


class ProductNode : public Node{

   public:

          void value(vector<double> &fn);

          void add_child(Node *child); 

          void set_observe(double observe);

          ProductNode();
};


class DiscVarNode : public Node{

   public:

        int observe;

        vector<double> p;

        DiscVarNode(int level);

        void set_observe(double observe);

        void value(vector<double> &fn);

       void add_child(Node *child); 


};


class ContVarNode : public Node{

    public:

        double observe;

        double mean = 0;

        double sigma = 1;

        void set_observe(double observe);

        void value(vector<double> &fn);

        void add_child(Node *child); 

        ContVarNode();

};


double normal_distribution(double mean, double sigma, double observe);

#endif
