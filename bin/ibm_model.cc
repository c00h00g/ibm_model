#include "ibm_model.h"
#include<stdio.h>

typedef pair<pair<long long, long long>, double> pair_map;

struct cmp {
    bool operator() (const pair_map& p1, const pair_map& p2) {
        return p1.second > p2.second;
    }
};

IBM_Model_One::
IBM_Model_One(int max_num, int direction) {
    f.clear();
    e.clear();
    f_e_co_occur_count.clear();
    e_count.clear();
    _max_iter_num = max_num;
    _direction = direction;

    _term_index = new(std::nothrow) TermIdxTrans();

    //一定不能为空
    assert(_term_index != NULL);
}

IBM_Model_One::
~IBM_Model_One() {
    if (NULL != _term_index) {
        delete _term_index;
        _term_index = NULL;
    }
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
            long long term_idx = _term_index->get_term_index(e[i][j]);
            e_uniq_terms.insert(term_idx);
        }

        for (int j = 0; j < f[i].size(); ++j) {
            long long term_idx = _term_index->get_term_index(f[i][j]);
            f_uniq_terms.insert(term_idx);
        }
    }

    cout << "build f and e success!" << e_uniq_terms.size() << "--" << f_uniq_terms.size() << endl;

    //初始化转移矩阵
    init_term_freq();

    cout << "init_term_freq success!" << endl;

    return;
}

/**
 * @brief : 初始化t概率，表示p(f|e)的概率
 **/
void IBM_Model_One::
init_term_freq() {
    init_prob = 0.25;
    set<long long>::iterator it_f;
    set<long long>::iterator it_e;
    for (it_f = f_uniq_terms.begin(); it_f != f_uniq_terms.end(); it_f++) {
        for (it_e = e_uniq_terms.begin(); it_e != e_uniq_terms.end(); it_e++) {
            long long f_term = *it_f;
            long long e_term = *it_e;
            term_prob[make_pair(f_term, e_term)] = init_prob;
        }
    }
    //debug_info();
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
        long long e_idx = _term_index->get_term_index(e_sen[i]);
        e_count[e_idx] = 0;
        for (int j = 0; j < f_sen.size(); ++j) {
            long long f_idx = _term_index->get_term_index(f_sen[j]);
            f_e_co_occur_count[make_pair(f_idx, e_idx)] = 0.0;
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
    ifstream fin(f_name.c_str());  
    string input;
    while (getline(fin, input)) {
        if (_direction == 0) {
            deal_data_forword(input);
        }else if (_direction == 1) {
            deal_data_reverse(input);
        }
    }

    cout << "deal data success!" << endl;

#if _debug
    _term_index->print_term_to_index();
    _term_index->print_index_to_term();

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
#endif

    return true;
}

/**
 * @brief : 正向处理一行数据, 填充f和e数组
 * @param : 文件中一行
 **/
void IBM_Model_One::
deal_data_forword(const string& input) {
    vector<string> e_f_vec;
    string split_one_sep = "\t";
    split_string(input, e_f_vec, split_one_sep);

    string split_two_sep = " ";

    //split f
    vector<string> f_vec;
    split_string(e_f_vec[0], f_vec, split_two_sep);

    f.push_back(f_vec);
    for (int i = 0; i < f_vec.size(); ++i) {
        _term_index->insert(f_vec[i]);
    }

    //split e
    vector<string> e_vec;
    split_string(e_f_vec[1], e_vec, split_two_sep);

    e.push_back(e_vec);
    for (int i = 0; i < e_vec.size(); ++i) {
        _term_index->insert(e_vec[i]);
    }

    return;
}

/**
 * @brief : 反向处理一行数据, 填充f和e数组
 * @param : 文件中一行
 **/
void IBM_Model_One::
deal_data_reverse(const string& input) {
    vector<string> e_f_vec;
    string split_one_sep = "\t";
    split_string(input, e_f_vec, split_one_sep);

    string split_two_sep = " ";

    //split f
    vector<string> f_vec;
    split_string(e_f_vec[1], f_vec, split_two_sep);

    f.push_back(f_vec);
    for (int i = 0; i < f_vec.size(); ++i) {
        _term_index->insert(f_vec[i]);
    }

    //split e
    vector<string> e_vec;
    split_string(e_f_vec[0], e_vec, split_two_sep);

    e.push_back(e_vec);
    for (int i = 0; i < e_vec.size(); ++i) {
        _term_index->insert(e_vec[i]);
    }

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
        cout << "after e step!" << endl;

        //m step
        cout << "chg-before" << i << endl;
        _m_step();
        cout << "after m step!" << endl;
    }

    //dump term->idx 数据
    _term_index->dump_data("TERM_INDEX_MP");

    //dump 对齐概率
    string align_file = "ALIGN_PROB";
    if (_direction == 0) {
        align_file += "_FORWARD";
    }else if (_direction == 1) {
        align_file += "_REVERSE";
    }

    dump_prob(align_file.c_str());
    
    return;
}

void IBM_Model_One::
debug_info() {
    set<long long>::iterator it_f;
    set<long long>::iterator it_e;
    for (it_f = f_uniq_terms.begin(); it_f != f_uniq_terms.end(); it_f++) {
        for (it_e = e_uniq_terms.begin(); it_e != e_uniq_terms.end(); it_e++) {
            long long f_term_idx = *it_f;
            long long e_term_idx = *it_e;
            string f_term = _term_index->get_index_cor_term(f_term_idx);
            string e_term = _term_index->get_index_cor_term(e_term_idx);

            pair<long long, long long> one_pair = make_pair(f_term_idx, e_term_idx);
            cout << f_term_idx << ":" << f_term << "||" << e_term_idx << ":" << e_term << "==>" << term_prob[one_pair] << endl;
        }
    }
    cout << "-------------" << endl;
}

/**
 * @brief : 将单term之间对齐的概率dump下来
 **/
void IBM_Model_One::
dump_prob(const char * file_name) {
    assert(NULL != file_name);

    ofstream term_align_prob;
    term_align_prob.open(file_name);

    //排序
    vector<pair_map> score_vec;
    map<pair<long long, long long>, double>::iterator iter; 
    for (iter = term_prob.begin(); iter != term_prob.end(); ++iter) {
        score_vec.push_back(*iter);
    }
    sort(score_vec.begin(), score_vec.end(), cmp());

    for (int i = 0; i < score_vec.size(); ++i) {
        string f_term = _term_index->get_index_cor_term(score_vec[i].first.first);
        string e_term = _term_index->get_index_cor_term(score_vec[i].first.second);
        term_align_prob << score_vec[i].first.first << "\t"
                        << f_term << "\t"
                        << score_vec[i].first.second << "\t"
                        << e_term << "\t"
                        << score_vec[i].second << endl;
    }

    term_align_prob.close();

    return;
}

/**
 * @brief : m步
 **/
bool IBM_Model_One::
_m_step() {
    set<long long>::iterator it_f;
    set<long long>::iterator it_e;
    for (it_f = f_uniq_terms.begin(); it_f != f_uniq_terms.end(); it_f++) {
        for (it_e = e_uniq_terms.begin(); it_e != e_uniq_terms.end(); it_e++) {
            long long f_term_idx = *it_f;
            long long e_term_idx = *it_e;
            pair<long long, long long> one_pair = make_pair(f_term_idx, e_term_idx);
            if (e_count[e_term_idx] != 0) {
                double prob = f_e_co_occur_count[one_pair] * 1.0 / e_count[e_term_idx];
                term_prob[one_pair] = prob;
            }
        }
    }

    //排序
    vector<pair_map> score_vec;
    map<pair<long long, long long>, double>::iterator iter; 
    for (iter = term_prob.begin(); iter != term_prob.end(); ++iter) {
        score_vec.push_back(*iter);
    }
    sort(score_vec.begin(), score_vec.end(), cmp());
    
    for (int i = 0; i < score_vec.size(); ++i) {
        string f_term = _term_index->get_index_cor_term(score_vec[i].first.first);
        string e_term = _term_index->get_index_cor_term(score_vec[i].first.second);

        cout << "chg-sort--" << i << ":" << score_vec[i].first.first << "-" << f_term << "||"
             << score_vec[i].first.second << "-" << e_term << "||" << score_vec[i].second << endl;
        if (i > 5000) {
            break;
        }
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
        long long f_term_idx = _term_index->get_term_index(f_term);

        double f_sum = _calc_sen_sum_increment(f_term, e_sen);

        for (int j = 0; j < e_sen.size(); ++j) {
            const string e_term = e_sen[j];
            long long e_term_idx = _term_index->get_term_index(e_term);

            double f_e_value = term_prob[make_pair(f_term_idx, e_term_idx)];
            double one_incre = f_e_value / f_sum;

            f_e_co_occur_count[make_pair(f_term_idx, e_term_idx)] += one_incre;
            e_count[e_term_idx] += one_incre;
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
   long long f_term_idx = _term_index->get_term_index(f_term);
   double sum = 0.0;
   for (int i = 0; i < e_sen.size(); ++i) {
       long long e_term_idx = _term_index->get_term_index(e_sen[i]);
       sum += term_prob[make_pair(f_term_idx, e_term_idx)];
   }
   return sum;
}
