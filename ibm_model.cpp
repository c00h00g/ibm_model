#include "ibm_model.h"
#include<stdio.h>

IBM_Model_One::
IBM_Model_One(int max_num) {
    f.clear();
    e.clear();
    f_e_co_occur_count.clear();
    f_e_count.clear();
    _max_iter_num = max_num;
}

/**
 * @brief : read in f
 **/
bool IBM_Model_One::
load_data(const string& f_name) {
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
        for (int j = 0; j < e_len; ++j) {
        
        }
    }

    return true;
}

bool IBM_Model_One::
_calc_increment(
        const vector<string>& one_f_sen,
        const vector<string>& one_e_sen) {
    for (int i = 0; i < one_f_sen.size(); ++i) {
        for (int j = 0; j < one_e_sen.size(); ++j) {
            string one_f_word = one_f_sen[i];
            string one_e_word = one_e_sen[j];
            double incre = _calc_one_increment(one_f_word,
                                     one_e_word, one_e_sen);
            pair<string, string> one_pair = make_pair(one_f_word, one_e_word);

            //map<pair<string, string>, double>::const_iterator iter = 
            //    f_e_co_occur_count.find(one_pair);
            
            f_e_co_occur_count[one_pair] = incre;
            f_e_count[one_e_word] += incre;
        }
    }
    return true;
}

double IBM_Model_One::
_calc_one_increment(
        const string& one_f_word,
        const string& one_e_word,
        const vector<string>& one_e_sen) {
    double f_e_cnt = f_e_co_occur_count[make_pair(one_f_word, one_e_word)];
    double f_e_sen_cnt = 0;
    for (int i = 0; i < one_e_sen.size(); ++i) {
        string one_e_word_i = one_e_sen[i];
        double f_e_one_cnt = f_e_co_occur_count[make_pair(one_f_word, one_e_word_i)];
        f_e_sen_cnt += f_e_one_cnt;
    }

    return f_e_cnt * 1.0 / f_e_sen_cnt;
}


int main() {
    printf("chg start to ibm model one!\n");
    return 0;
}
