#include "ibm_model.h"
#include<stdio.h>

typedef pair<pair<string, string>, double> pair_map;

struct cmp {
    bool operator() (const pair_map& p1, const pair_map& p2) {
        return p1.second > p2.second;
    }
};

IBM_Model_One::
IBM_Model_One(int max_num) {
    f.clear();
    e.clear();
    f_e_co_occur_count.clear();
    e_count.clear();
    _max_iter_num = max_num;
}

/**
 * @brief : 整体初始化uniq级别term转移概率
 **/
void IBM_Model_One::
init() {
    int f_size = f.size();
    int e_size = e.size();
    assert(f_size == e_size);

    for (int i = 0; i < f_size; ++i) {
        //insert terms
        for (int j = 0; j < e[i].size(); ++j) {
            e_uniq_terms.insert(e[i][j]);
        }

        for (int j = 0; j < f[i].size(); ++j) {
            f_uniq_terms.insert(f[i][j]);
        }
    }

    //初始化转移矩阵
    init_term_freq(f_uniq_terms, e_uniq_terms);

    debug_info();
    return;
}

/**
 * @brief : 初始化t概率，表示p(f|e)的概率
 **/
void IBM_Model_One::
init_term_freq(const set<string>& f_terms, 
               const set<string>& e_terms) {
    init_prob = 0.25;
    set<string>::iterator it_f;
    set<string>::iterator it_e;
    for (it_f = f_uniq_terms.begin(); it_f != f_uniq_terms.end(); it_f++) {
        for (it_e = e_uniq_terms.begin(); it_e != e_uniq_terms.end(); it_e++) {
            string f_term = *it_f;
            string e_term = *it_e;
            term_prob[make_pair(f_term, e_term)] = init_prob;
        }
    }
    debug_info();
    return;
}

/**
 * @brief : 初始化e->f频次，以及e频次
 * @param e_sen : 英文分句
 * @param f_sen : 法文分句
 **/
void IBM_Model_One::
init_term_count_freq(const vector<string>& e_sen, 
                     const vector<string>& f_sen) {
    for (int i = 0; i < e_sen.size(); ++i) {
        e_count[e_sen[i]] = 0;
        for (int j = 0; j < f_sen.size(); ++j) {
            f_e_co_occur_count[make_pair(f_sen[j], e_sen[i])] = 0.0;
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
 * @param : 输入文件地址
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

/**
 * @brief : 处理一行数据
 * @param : 文件中一行
 **/
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

/**
 * @brief : 模型训练，输出是中间参数t(f|e)
 **/
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

        debug_info();
    }
}

void IBM_Model_One::
debug_info() {
    set<string>::iterator it_f;
    set<string>::iterator it_e;
    for (it_f = f_uniq_terms.begin(); it_f != f_uniq_terms.end(); it_f++) {
        for (it_e = e_uniq_terms.begin(); it_e != e_uniq_terms.end(); it_e++) {
            string f_term = *it_f;
            string e_term = *it_e;
            pair<string, string> one_pair = make_pair(f_term, e_term);
            cout << f_term << ":" << e_term << "==>" << term_prob[one_pair] << endl;
        }
    }
    cout << "-------------" << endl;
}

/**
 * @brief : m步骤
 **/
bool IBM_Model_One::
_m_step() {
    set<string>::iterator it_f;
    set<string>::iterator it_e;
    for (it_f = f_uniq_terms.begin(); it_f != f_uniq_terms.end(); it_f++) {
        for (it_e = e_uniq_terms.begin(); it_e != e_uniq_terms.end(); it_e++) {
            string f_term = *it_f;
            string e_term = *it_e;
            pair<string, string> one_pair = make_pair(f_term, e_term);
            if (e_count[e_term] != 0) {
                double prob = f_e_co_occur_count[one_pair] * 1.0 / e_count[e_term];
                term_prob[one_pair] = prob;
            }
        }
    }

    //排序
    vector<pair_map> score_vec;
    map<pair<string, string>, double>::iterator iter; 
    for (iter = term_prob.begin(); iter != term_prob.end(); ++iter) {
        score_vec.push_back(*iter);
    }
    sort(score_vec.begin(), score_vec.end(), cmp());
    
    for (int i = 0; i < score_vec.size(); ++i) {
        cout << "i:" << i << score_vec[i].first.first << "-" << score_vec[i].first.second << ":" << score_vec[i].second << endl;
    }

    return true;
}

/**
 * @brief : execute e_step
 **/
bool IBM_Model_One::
_e_step()  {
    int f_len = f.size();
    int e_len = e.size();

    assert(f_len == e_len);

    //for each sentence
    for (int i = 0; i < f_len; ++i) {
        vector<string> f_sen = f[i];
        vector<string> e_sen = e[i];
        _calc_sen_increment(f_sen, e_sen);
    }

    return true;
}

/**
 * @brief : 计算一个句对频次增量
 * @param f_sen : 法文分句
 * @param e_sen : 英文分句
 **/
double IBM_Model_One::
_calc_sen_increment(const vector<string>& f_sen,
                    const vector<string>& e_sen) {
    for (int i = 0; i < f_sen.size(); ++i) {
        const string f_term = f_sen[i];
        double f_sum = _calc_sen_sum_increment(f_term, e_sen);
        cout << "f_sum:" << f_sum << endl;
        for (int j = 0; j < e_sen.size(); ++j) {
            const string e_term = e_sen[j];
            double f_e_value = term_prob[make_pair(f_term, e_term)];
            double one_incre = f_e_value / f_sum;
            f_e_co_occur_count[make_pair(f_term, e_term)] += one_incre;
            e_count[e_term] += one_incre;
        }
    }
    return true;
}

/**
 * @brief : 计算一个f_term对一个英文句子总量
 * @param f_term : 法文一个term
 * @param e_sen : 英文分句
 **/
double IBM_Model_One::
_calc_sen_sum_increment(const string& f_term,
                        const vector<string>& e_sen) {
   double sum = 0.0;
   for (int i = 0; i < e_sen.size(); ++i) {
       sum += term_prob[make_pair(f_term, e_sen[i])];
   }
   return sum;
}
