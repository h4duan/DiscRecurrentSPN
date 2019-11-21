#include "SPN.h"
#include "Node.h"
#include <random>


class ContRspn{

    public:

        SPN *template_spn;

        int category;

        vector<vector<double>> interface_weight;

        vector<double> prior;

        vector<vector<double>> sequence;

        vector<double> fns;

        vector<vector<double>> dns;

        vector<double> eval();

        double differentiate_discriminate(bool interior, int i, int j);

        void putSequence(const vector<vector<double>> &sequence, int category);

        void update_ebw(const vector<vector<vector<double>>> &sequences,  int epochs, const vector<int> &classes,const vector<vector<vector<double>>> &test_sequences,const vector<int> &result);

        double test_accuracy(const vector<vector<vector<double>>> &test_sequences,const vector<int> &result);

        ContRspn(int num_variables, int num_class, const vector<double> &prior);

        ~ContRspn();

        vector<double> differentiate_interior(int i, int j);

        vector<double> differentiate_interface(int i, int j);

        vector<double> classProbability(const vector<vector<double>> &sequence);


    private:

        int num_variables;

        int num_class;

                
        void normalize(vector<double> &f);

        void add_differentiates_ebw(vector<double> &means, vector<double> &sigmas, vector<double> &denominator, vector<vector<double>> &new_interface);

        void update_one_epoch_ebw(const vector<vector<vector<double>>> &sequences, double &ll, const vector<int> &classes, double D);
};
