#include "ibm_model.h"
#include<stdio.h>

IBM_Model_One::
IBM_Model_One(int max_num) {
    f.clear();
    e.clear();
    f_e_co_occur_count.clear();
    e_count.clear();
    _max_iter_num = max_num;
}

/**
 * @brief : 整体初始化
 **/
void IBM_Model_One::
init() {
    int f_size = f.size();
    int e_size = e.size();
    assert(f_size == e_size);

    for (int i = 0; i < f_size; ++i) {
        init_term_freq(e[i], f[i]);
    }
    return;
}

/**
 * @brief : 初始化t概率，表示p(f|e)的概率
 **/
void IBM_Model_One::
init_term_freq(const vector<string>& e_sen, 
               const vector<string>& f_sen) {
    for (int i = 0; i < e_sen.size(); ++i) {
        for (int j = 0; j < f_sen.size(); ++j) {
            term_prob[make_pair(e_sen[i], f_sen[j])] = init_prob;
        }
    }
}

void IBM_Model_One::
init_term_count_freq(const vector<string>& e_sen, 
                     const vector<string>& f_sen) {
    for (int i = 0; i < e_sen.size(); ++i) {
        e_count[e_sen[i]] = 0;
        for (int j = 0; j < f_sen.size(); ++j) {
            f_e_co_occur_count[make_pair(e_sen[i], f_sen[j])] = 0.0;
        }
    }
    return;
}

/**
 * @brief : 每一轮之前的初始化
 **/
void IBM_Model_One::
init_one_step() {
    f_e_co_occur_count.clear();
    e_count.clear();

    //初始化计数
    for (int i = 0; i < f.size(); ++i) {
        init_term_count_freq(e[i], f[i]);
    }
}

/**
 * @brief : read in f
 **/
bool IBM_Model_One::
load_data(const string& f_name) {
    ifstream fin(f_name);  
    string input;
    while (getline(fin, input)) {
        cout << input << endl;
        deal_data(input);
    }

    //debug
    for (int i = 0 ; i < f.size(); ++i) {
        for (int j = 0; j < f[i].size(); ++j) {
            cout << f[i][j] << endl;
        }
        cout << "=====" << endl;
    }

    for (int i = 0 ; i < e.size(); ++i) {
        for (int j = 0; j < e[i].size(); ++j) {
            cout << e[i][j] << endl;
        }
        cout << "=====" << endl;
    }

    return true;
}

void IBM_Model_One::
deal_data(const string& input) {
    vector<string> e_f_vec;
    string split_one_sep = "\t";
    split_string(input, e_f_vec, split_one_sep);

    //debug
    cout << e_f_vec[0] << endl;;

    string split_two_sep = " ";

    //split f
    vector<string> f_vec;
    split_string(e_f_vec[0], f_vec, split_two_sep);

    for (int i = 0; i < f_vec.size(); ++i) {
        cout << f_vec[i] << endl;
    }

    f.push_back(f_vec);

    cout << e_f_vec[1] << endl;
    //split e
    vector<string> e_vec;
    split_string(e_f_vec[1], e_vec, split_two_sep);

    for (int i = 0; i < e_vec.size(); ++i) {
        cout << e_vec[i] << endl;
    }

    e.push_back(e_vec);

    return;
}

void IBM_Model_One::
train() {
    for (int i = 0; i < _max_iter_num; ++i) {
        cout << "start iter:" << i << "-------" << endl;
        //初始化
        init_one_step();

        //e step
        _e_step();

        //m step
        _m_step();
    }
}

bool IBM_Model_One::
_m_step() {
    return true;
}

/**
 * @brief : execute e_step
 **/
bool IBM_Model_One::
_e_step()  {
    int f_len = f.size();
    int e_len = e.size();

    //for each sentence
    for (int i = 0; i < f_len; ++i) {
        vector<string> f_sen = f[i];
        vector<string> e_sen = e[i];
        _calc_sen_increment(f_sen, e_sen);
    }

    return true;
}

bool IBM_Model_One::
_calc_sen_increment(const vector<string>& f_sen,
                    const vector<string>& e_sen) {
    for (int i = 0; i < f_sen.size(); ++i) {
        const string f_term = f_sen[i];
        const string e_term = e_sen[i];
        double one_incre = _calc_one_increment(f_term, e_term, e_sen);
        f_e_co_occur_count[make_pair(f_term, e_term)] += one_incre;
        e_count[e_term] += one_incre;
    }
    return true;
}

/**
 * @brief : 
 **/
bool IBM_Model_One::
_calc_one_increment(
        const string f_term,
        const string e_term,
        const vector<string>& one_e_sen) {

    double f_e_prob = term_prob[make_pair(e_term, f_term)];

    double sum_prob = 0.0;
    for (int i = 0; i < one_e_sen.size(); ++i) {
        string one_e_term = one_e_sen[i];
        sum_prob += term_prob[make_pair(one_e_term, f_term)];
    }

    return f_e_prob / sum_prob;
}

