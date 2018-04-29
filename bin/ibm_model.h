#include<string>
#include<iostream>
#include<vector>
#include<map>
#include<utility> // pair

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;

//ibm model 1
class IBM_Model_One {
public:
    IBM_Model_One(int max_num);

    void init();

    void init_one_step();

    void init_term_freq(const vector<string>& e_sen, 
                        const vector<string>& f_sen);

    void init_term_count_freq(const vector<string>& e_sen, 
                              const vector<string>& f_sen);

    bool load_data(const string& f_name);

    void train();
    bool _e_step();
    bool _m_step();

    bool _calc_increment(
           const vector<string>& one_f_sen,
           const vector<string>& one_e_sen);

    bool _calc_sen_increment(const vector<string>& f_sen,
                             const vector<string>& e_sen);

    bool _calc_one_increment(const string f_term,
                             const string e_term,
                             const vector<string>& one_e_sen);

private:
    vector<vector<string> >  f; //france segment
    vector<vector<string> > e; //english segment

    map<pair<string, string>, double> f_e_co_occur_count; //f and e co occur times
    map<string, double> e_count; // e occur times

    map<pair<string, string>, double> term_prob; //term f and term e parameters, 最终输出
    double init_prob; // term map start probability

    int _max_iter_num;
};
