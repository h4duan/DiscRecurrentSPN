#include "RSPN.h"
#include "assert.h"
#include "SPN.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <random> 

using namespace std;



ContRspn::ContRspn(int num_variables, int num_class, const vector<double> &prior):
    num_variables{num_variables}, num_class{num_class}{

        this->prior = prior;

        template_spn = new SPN{num_variables, 0, num_class}; 

    
        for(int i = 0; i < num_class; i++){
            double sum = 0.0;
            //cout << num_class << endl;
            vector<double> p(num_class, 0.0);
            for(int i = 0; i < num_class; i++){
               p[i] = (double)rand() / RAND_MAX;           
                sum += p[i];
            }
            for(int i = 0; i < num_class; i++){
                p[i] = p[i]/sum;
            }
            interface_weight.push_back(p);
        }

        //cout << interface_weight.size() << endl;

        //cout << "printing interface weight" << endl;

        /*for(int i =0; i< num_class; i++){
            for(auto &j : interface_weight[i]){
                cout << j << "  ";
            }
            cout << endl;
        }*/
    }


ContRspn::~ContRspn(){
    delete template_spn;
}



void ContRspn::putSequence(const vector<vector<double>> &sequence, int category){

    this->category = category;
    
    this->sequence = sequence;

    int n = sequence.size();

    int variables_template = template_spn->nodes.size();

    int total = n * variables_template + (num_class - 1) * (n - 1);

    dns.clear();

    fns.clear();

    vector<double> f(total, 0.0);

    fns = f;

    //cout << total << endl;

    int times = 0;

    vector<Node *> rspn_nodes(total, nullptr);

    int increase = variables_template + num_class - 1;


    for(int i = 0; i < n; i++){
        int leaf_times = 0;

        if(i == 0){
            rspn_nodes[times] = new SumNode{*((SumNode *)(template_spn->nodes[0]))}; 
            rspn_nodes[times]->weight = prior;
            rspn_nodes[times]->set_id(times);
            times++;            
        } else{
            for(int k = 0; k < num_class; k++){
                rspn_nodes[times] = new SumNode{*((SumNode *)(template_spn->nodes[0]))}; 
                //times++;
                rspn_nodes[times]->weight = interface_weight[k];
                rspn_nodes[times]->set_id(times);
                int num_children = rspn_nodes[times]->children.size();
                for(int h = 0; h < num_children; h++){
                    rspn_nodes[times]->children[h] += i * increase;
                }
                rspn_nodes[times-variables_template+1]->add_child(rspn_nodes[times]);
                times++;
            }
        }

        for(int j = 1; j < variables_template; j++){
            Node *temp;
            temp = template_spn->nodes[times - i*increase];
            
            if(temp->leaves){
                rspn_nodes[times] = new ContVarNode{*((ContVarNode *)(temp))};
                ((ContVarNode *)(rspn_nodes[times]))->observe = sequence[i][leaf_times/2];
                leaf_times++;
                //cout << leaf_times << endl;
            }else if(temp->weight.empty()){
                rspn_nodes[times] = new ProductNode{*((ProductNode *)(temp))};
               //cout << "product " << times << end; 
            } else{
                rspn_nodes[times] = new SumNode{*((SumNode *)(temp))}; 
            }

            int num_children = rspn_nodes[times]->children.size();
            
            if(i != 0){
                for(int h = 0; h < num_children; h++){
                    rspn_nodes[times]->children[h] += i * increase;
                }
            }

            rspn_nodes[times]->set_id(times);

            times++;
        }        
    }

 
    vector<double> fn(total, 0.0);
    
    for(int i = 0; i < total; i++){
        //cout << total-1-i << endl;
        rspn_nodes[total-1-i]->value(fns);
        //cout << fns[total-1-i] << endl;
    }  

    //printVector(fns);

    vector<double> dn_root(total, 0.0);
        
    dn_root[0] = 1;
        
    for(int y = 0; y < total; y++){
         if(rspn_nodes[y]->leaves){
             continue;
         }

         if(rspn_nodes[y]->weight.empty()){
             int num_children = rspn_nodes[y]->children.size();
             for(int j = 0; j < num_children; j++){
                dn_root[rspn_nodes[y]->children[j]] += exp(log(dn_root[rspn_nodes[y]->id])+log(fns[rspn_nodes[y]->id])-log(fns[rspn_nodes[y]->children[j]]));
             }
         } else{
             int num_children = rspn_nodes[y]->children.size();
             for(int j =0; j < num_children;j++){
                 dn_root[rspn_nodes[y]->children[j]] += exp(log(rspn_nodes[y]->weight[j]) + log(dn_root[rspn_nodes[y]->id]));
            }
         }
    }

    vector<double> dn_sub(total, 0.0);

    dn_sub[category+1] = 1;

    for(int y = category+1; y < total; y++){
         if(rspn_nodes[y]->leaves){
             continue;
         }

         if(rspn_nodes[y]->weight.empty()){
             int num_children = rspn_nodes[y]->children.size();
             for(int j = 0; j < num_children; j++){
                dn_sub[rspn_nodes[y]->children[j]] += exp(log(dn_sub[rspn_nodes[y]->id])+log(fns[rspn_nodes[y]->id])-log(fns[rspn_nodes[y]->children[j]]));
             }
         } else{
             int num_children = rspn_nodes[y]->children.size();
             for(int j =0; j < num_children;j++){
                 dn_sub[rspn_nodes[y]->children[j]] += exp(log(rspn_nodes[y]->weight[j]) + log(dn_sub[rspn_nodes[y]->id]));
            }
         }
    }

    //printVector(dn_root);
    //printVector(dn_sub);

    dns.push_back(dn_root);
    dns.push_back(dn_sub);

    for(auto &node: rspn_nodes){
        delete node;
    }
   
}


vector<double> ContRspn::classProbability(const vector<vector<double>> &sequence){
    //vector<double> f(num_class, 0.0);
    
    int n = sequence.size();

    int variables_template = template_spn->nodes.size();

    int total = n * variables_template + (num_class - 1) * (n - 1);

    vector<double> f(total, 0.0);

    //fns = f;

    //cout << total << endl;

    int times = 0;

    vector<Node *> rspn_nodes(total, nullptr);

    int increase = variables_template + num_class - 1;


    for(int i = 0; i < n; i++){
        int leaf_times = 0;

        if(i == 0){
            rspn_nodes[times] = new SumNode{*((SumNode *)(template_spn->nodes[0]))}; 
            rspn_nodes[times]->weight = prior;
            rspn_nodes[times]->set_id(times);
            times++;            
        } else{
            for(int k = 0; k < num_class; k++){
                rspn_nodes[times] = new SumNode{*((SumNode *)(template_spn->nodes[0]))}; 
                //times++;
                rspn_nodes[times]->weight = interface_weight[k];
                rspn_nodes[times]->set_id(times);
                int num_children = rspn_nodes[times]->children.size();
                for(int h = 0; h < num_children; h++){
                    rspn_nodes[times]->children[h] += i * increase;
                }
                rspn_nodes[times-variables_template+1]->add_child(rspn_nodes[times]);
                times++;
            }
        }

        for(int j = 1; j < variables_template; j++){
            Node *temp;
            temp = template_spn->nodes[times - i*increase];
            
            if(temp->leaves){
                rspn_nodes[times] = new ContVarNode{*((ContVarNode *)(temp))};
                ((ContVarNode *)(rspn_nodes[times]))->observe = sequence[i][leaf_times/2];
                leaf_times++;
                //cout << leaf_times << endl;
            }else if(temp->weight.empty()){
                rspn_nodes[times] = new ProductNode{*((ProductNode *)(temp))};
               //cout << "product " << times << end; 
            } else{
                rspn_nodes[times] = new SumNode{*((SumNode *)(temp))}; 
            }

            int num_children = rspn_nodes[times]->children.size();
            
            if(i != 0){
                for(int h = 0; h < num_children; h++){
                    rspn_nodes[times]->children[h] += i * increase;
                }
            }

            rspn_nodes[times]->set_id(times);

            times++;
        }        
    }

    for(int i = 0; i < total; i++){
        //cout << total-1-i << endl;
        rspn_nodes[total-1-i]->value(f);
        //cout << fns[total-1-i] << endl;
    }  

    vector<double> fn(f.begin()+1, f.begin()+num_class+1);

    for(auto &node: rspn_nodes){
        delete node;
    }

    return fn;
}


vector<double> ContRspn::eval(){
    vector<double> values{fns[0], fns[category+1]};
    return values;
}


vector<double> ContRspn::differentiate_interior(int i, int j){
    vector<double> f(2, 0.0);

    int n = sequence.size();

    int variables_template = template_spn->nodes.size();

    int increase = variables_template + num_class - 1;

    //cout << increase << endl;

    double total = 0.0;

    double sub = 0.0;

    //printVector(dns[1]);

    for(int step = 0; step < n; step++){
        //cout << dns[1][step*increase+i] << endl;
        //cout << step*increase+i << endl;
        if(dns[1][step*increase+i] != 0.0){
             sub+= exp(log(fns[step*increase+j])+log(dns[1][step*increase+i]));
        }
    }

    for(int step = 0; step < n; step++){
        if(dns[0][step*increase+i] != 0.0){
              total+= exp(log(fns[step*increase+j])+log(dns[0][step*increase+i]));
        }
    }

    f[0] = total;
    f[1] = sub; 

    return f;
}



vector<double> ContRspn::differentiate_interface(int i, int j){
    vector<double> f(2, 0.0);

    int n = sequence.size();

    int variables_template = template_spn->nodes.size();

    int increase_i = variables_template + i;

    int increase_j = variables_template + num_class - 1;

    double total = 0.0;

    double sub = 0.0;

    for(int step = 1; step < n; step++){
        if(dns[1][step*increase_i] != 0.0){
               sub+= exp(log(fns[step*increase_j+j+1])+log(dns[1][step*increase_i]));
        }
    }

    for(int step = 1; step < n; step++){
         if(dns[0][step*increase_i] != 0.0){
             total+= exp(log(fns[step*increase_j+j+1])+log(dns[0][step*increase_i]));
         }
    }

    f[0] = total;
    f[1] = sub;

    return f;
}



double ContRspn::differentiate_discriminate(bool interior, int i, int j){
      if(interior){
          vector<double> values = eval();
          vector<double> differentiates = differentiate_interior(i, j);
          double F_y = values[1];
          double F_root = values[0];
          double differ_y = differentiates[1];
          double differ_root = differentiates[0];
          return exp(log(differ_y) - log(F_y)) - exp(log(differ_root)-log(F_root));
      } else{
          vector<double> values = eval();
          vector<double> differentiates = differentiate_interface(i, j);
          double F_y = values[1];
          double F_root = values[0];
          double differ_y = differentiates[1];
          double differ_root = differentiates[0];
          return exp(log(differ_y) - log(F_y)) - exp(log(differ_root)-log(F_root));
      }
}



void ContRspn::normalize(vector<double> &f){
    double sum = 0.0;
    for(auto &i : f){
        sum += i;
    }

    for(auto &i : f){
        i = exp(log(i) - log(sum));
    }
}

double SafeMultiply(double x, double y){
     if(x == 0.0 || y == 0.0){
        return 0.0;
    } else if(x > 0.0 && y > 0.0){
        return exp(log(x) + log(y));
    } else if(x > 0.0 && y < 0.0){
        return -exp(log(x) + log(-y));
    } else if(x < 0.0 && y < 0.0){
        return exp(log(-x) + log(-y));
    } else{
        return -exp(log(-x) + log(y));
    }
}



void ContRspn::add_differentiates_ebw(vector<double> &means, vector<double> &sigmas, vector<double> &denominator, vector<vector<double>> &new_interface){
   int variables_template = template_spn->nodes.size();

   for(int i = 1 + num_class; i < variables_template; i++){
       if(!template_spn->nodes[i]->weight.empty()){
           int num_children = template_spn->nodes[i]->children.size();
           for(int j = 0; j < num_children; j++){
               template_spn->nodes[i]->new_weight[j] += differentiate_discriminate(true, i, template_spn->nodes[i]->children[j]);
            }
        }
   }

   
   for(int i = 0; i < num_class; i++){
      for(int j = 0; j < num_class; j++){
           new_interface[i][j] +=  differentiate_discriminate(false, i, j);
       }
   }

   int n = sequence.size();

   int increase = variables_template + num_class - 1;

   double F_y = eval()[1];

   double F_root = eval()[0];
   
   int first_leaf_id = variables_template-2*num_variables;

   //cout << first_leaf_id << endl;

   //cout << "here" << endl;

   for(int i = 0; i < 2*num_variables; i++){
       int id_leaf = variables_template-i-1;
       int id_var = (id_leaf - first_leaf_id)/2;
        for(int j = 0; j < n; j++){
           double x = sequence[j][id_var];
           int id = id_leaf + j*increase;
           //cout << id << endl;
           double differ = exp(log(dns[1][id])-log(F_y)) - exp(log(dns[0][id])-log(F_root));
           double result = SafeMultiply(fns[id], differ);
           means[i] += SafeMultiply(result, x);
           sigmas[i] += SafeMultiply(SafeMultiply(result, x), x);
           denominator[i] += result;
       }
   }

   //cout << "end" << endl;
}



void ContRspn::update_one_epoch_ebw(const vector<vector<vector<double>>> &sequences, double &ll, const vector<int> &classes, double D){
    assert(sequences.size() == classes.size());

    int n = classes.size();

    vector<double> means(2*num_variables, 0.0);
    vector<double> sigmas(2*num_variables, 0.0);
    vector<double> denominator(2*num_variables, 0.0);
    vector<double> vect(num_class, 0.0);
    vector<vector<double>> new_interface(num_class, vect);

    int variables_template = template_spn->nodes.size();

    for(int i = num_class+1; i < variables_template; i++){
       if(!template_spn->nodes[i]->weight.empty()){
           int num_children = template_spn->nodes[i]->weight.size();
           vector<double> f(num_children, 0.0);
           template_spn->nodes[i]->new_weight = f;
       }
    }



    for(int i = 0; i < n; i++){
        putSequence(sequences[i], classes[i]);
        ll += log(prior[classes[i]]) + log(fns[classes[i]+1]) - log(fns[0]);
        add_differentiates_ebw(means, sigmas, denominator, new_interface);
    }

    for(int i = num_class+1; i < variables_template; i++){
       if(!template_spn->nodes[i]->weight.empty()){
           //cout << "weight" << endl;
           //printVector(template_spn->nodes[i]->weight);
           int num_children = template_spn->nodes[i]->weight.size();
           bool valid = true;
           for(int j = 0; j < num_children; j++){
               double new_weight = template_spn->nodes[i]->new_weight[j]; 
      
               template_spn->nodes[i]->new_weight[j] = SafeMultiply(new_weight + D, template_spn->nodes[i]->weight[j]);

            if(template_spn->nodes[i]->new_weight[j] <= 0.0){
                   //cout << "invalid weight" << endl;
                   valid = false;
                   break;
               }
           }

           if(valid){
               normalize(template_spn->nodes[i]->new_weight);
               //printVector(template_spn->nodes[i]->new_weight);
               template_spn->nodes[i]->weight = template_spn->nodes[i]->new_weight;
           }
           //printVector(template_spn->nodes[i]->weight);
       }    
    } 


    for(int i = 0; i < num_class; i++){
       //cout << "interface weight" << endl;
       //printVector(interface_weight[i]);
       bool valid = true;
       for(int j = 0; j < num_class; j++){
           new_interface[i][j] += SafeMultiply(interface_weight[i][j], new_interface[i][j] +D);
           if(new_interface[i][j] <= 0.0){
               //cout << "invalid interface" << endl;
               valid = false;
               break;
           }
       }
      if(valid){
            normalize(new_interface[i]);
            interface_weight[i] = new_interface[i];
      }
    }  

    for(int i = 0; i < 2*num_variables; i++){
        int id_leaf = variables_template-i-1;
        //cout << id_leaf << endl; 
        double denom = denominator[i]; 
        //cout << "here2" << endl;
        //cout << id_leaf << endl;
        if(denom > 0.0){
           double old_mean = ((ContVarNode *)(template_spn->nodes[id_leaf]))->mean;
           double old_sigma = ((ContVarNode *)(template_spn->nodes[id_leaf]))->sigma;
           double new_mean = (means[i] + D *old_mean) / (denom + D);
           double new_sigma =  ((sigmas[i] + D * (old_mean*old_mean + old_sigma)) / (denom + D)) - new_mean*new_mean;
           ((ContVarNode *)(template_spn->nodes[id_leaf]))->mean = new_mean;
           ((ContVarNode *)(template_spn->nodes[id_leaf]))->sigma = new_sigma;
          //cout << "new mean" << new_mean << endl;
          //cout << "new sigma" << new_sigma << endl;
        }
    }
}


void ContRspn::update_ebw(const vector<vector<vector<double>>> &sequences,  int epochs, const vector<int> &classes, const vector<vector<vector<double>>> &test_sequences,const vector<int> &result){
    double D = 60;

    //double ll = 0.0;
 
    for(int i = 0; i < epochs; i++){
        double ll = 0.0;
        update_one_epoch_ebw(sequences,ll, classes, D);
        D += 0.001;
        cout << "likelihood: " <<  -ll << endl;
        cout << "accuracy:  " << test_accuracy(test_sequences, result) << endl;
    }


}

int max_pos(vector<double> y){
    int pos = 0;
    double previous = y[0];
    int length = y.size();
    for(int i = 1; i < length; i++){
        if(y[i] > previous){
            pos = i;
            previous = y[i];
        }
    }
    return pos;
}


double ContRspn::test_accuracy(const vector<vector<vector<double>>> &test_sequences,const vector<int> &result){
    assert(test_sequences.size() == result.size());
    int length = test_sequences.size();
    vector<int> ys(length, 0);

    for(int i = 0; i < length; i++){
       vector<double> y = classProbability(test_sequences[i]);
       //vector<double> y(fns.begin()+1, fns.begin()+num_class);
       assert(y.size() == num_class);
       //printVector(y);
       ys[i] = max_pos(y);
       //cout << ys[i] << endl;
    }

    int correct = 0;

    for(int i = 0; i < length; i++){
        if(ys[i] == result[i]){
            correct++;
        }
    }

    return double(correct)/double(length);
}






