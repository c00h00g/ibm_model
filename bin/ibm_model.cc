#include "ibm_model.h"
#include<stdio.h>

typedef pair<pair<uint64_t, uint64_t>, double> pair_map;

struct cmp {
    bool operator() (const pair_map& p1, const pair_map& p2) {
        return p1.second > p2.second;
    }
};

IBM_Model_One::
IBM_Model_One(uint32_t max_num, uint32_t direction, uint32_t thread_num) {
    f.clear();
    e.clear();
    f.reserve(TERM_SIZE);
    e.reserve(TERM_SIZE);
    f_e_co_occur_count.clear();
    e_count.clear();
    _max_iter_num = max_num;
    _direction = direction;
    _thread_num = thread_num;

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
    uint32_t f_size = f.size();
    uint32_t e_size = e.size();

    assert(f_size == e_size);

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
    //初始化概率
    init_prob = 0.25;

    uint32_t f_size = f.size();
    uint32_t e_size = e.size();

    assert(f_size == e_size);

    //all sentences
    for (uint32_t i = 0; i < f_size; ++i) {
        //left sentence <==> right sentence
        for (uint32_t j = 0; j < e[i].size(); ++j) {
            uint64_t term_l = _term_index->get_term_index(e[i][j]);

            for (uint32_t k = 0; k < f[i].size(); ++k) {
                uint64_t term_r = _term_index->get_term_index(f[i][k]);
                //english --> france prob
                term_prob[make_pair(term_l, term_r)] = init_prob;
            }
        }
    }

    return;
}

/**
 * @brief : 初始化e->f频次，以及e频次
 * @param e_sen : 英文分句
 * @param f_sen : 法文分句
 **/
void IBM_Model_One::
init_term_count_freq(const vector<string>& f_sen, 
                     const vector<string>& e_sen) {
    for (uint32_t i = 0; i < f_sen.size(); ++i) {
        uint64_t f_idx = _term_index->get_term_index(f_sen[i]);
        for (uint32_t j = 0; j < e_sen.size(); ++j) {
            uint64_t e_idx = _term_index->get_term_index(e_sen[j]);
            e_count[e_idx] = 0;
            f_e_co_occur_count[make_pair(e_idx, f_idx)] = 0.0;
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
    for (uint32_t i = 0; i < f.size(); ++i) {
        init_term_count_freq(f[i], e[i]);
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

#if 1
    _term_index->print_term_to_index();
    _term_index->print_index_to_term();

    //for (int i = 0 ; i < f.size(); ++i) {
    //    for (int j = 0; j < f[i].size(); ++j) {
    //        cout << f[i][j] << endl;
    //    }
    //    cout << "=====" << endl;
    //}

    //for (int i = 0 ; i < e.size(); ++i) {
    //    for (int j = 0; j < e[i].size(); ++j) {
    //        cout << e[i][j] << endl;
    //    }
    //    cout << "=====" << endl;
    //}
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
    for (uint32_t i = 0; i < f_vec.size(); ++i) {
        _term_index->insert(f_vec[i]);
    }

    //split e
    vector<string> e_vec;
    split_string(e_f_vec[1], e_vec, split_two_sep);

    e.push_back(e_vec);
    for (uint32_t i = 0; i < e_vec.size(); ++i) {
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

    //split e
    vector<string> e_vec;
    split_string(e_f_vec[0], e_vec, split_two_sep);

    e.push_back(e_vec);

    //插入
    for (uint32_t i = 0; i < e_vec.size(); ++i) {
        _term_index->insert(e_vec[i]);
    }

    for (uint32_t i = 0; i < f_vec.size(); ++i) {
        _term_index->insert(f_vec[i]);
    }

    return;
}

/**
 * @brief : 模型训练，输出是中间参数t(f|e)
 **/
void IBM_Model_One::
train() {
    for (uint32_t i = 0; i < _max_iter_num; ++i) {
        cout << "start iter:" << i << "-------" << endl;
        //初始化
        init_one_step();

        //e step
        cout << "f_size:" << f.size() << "thread_num : " << _thread_num << endl;
        if (f.size() < _thread_num) {
            cout << "start single thread e_step" << endl;
            _e_step();
        }else {
            cout << "start multi thread e_step" << endl;
            _start_e_step_thread();
        }
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
    return;
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
    map<pair<uint64_t, uint64_t>, double>::iterator iter; 
    for (iter = term_prob.begin(); iter != term_prob.end(); ++iter) {
        score_vec.push_back(*iter);
    }
    sort(score_vec.begin(), score_vec.end(), cmp());

    for (uint32_t i = 0; i < score_vec.size(); ++i) {
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

void IBM_Model_One::
_m_step_one_sen(const vector<string>& f_sen,
                const vector<string>& e_sen) {
    for (uint32_t i = 0; i < f_sen.size(); ++i) {
        for (uint32_t j = 0; j < e_sen.size(); ++j) {
            uint64_t f_term_idx = _term_index->get_term_index(f_sen[i]);
            uint64_t e_term_idx = _term_index->get_term_index(e_sen[j]);
            pair<uint64_t, uint64_t> one_pair = make_pair(e_term_idx, f_term_idx);
            if (e_count[e_term_idx] != 0) {
                double prob = f_e_co_occur_count[one_pair] * 1.0 / e_count[e_term_idx];
                term_prob[one_pair] = prob;
            }
        }
    }
}

/**
 * @brief : m步
 **/
bool IBM_Model_One::
_m_step() {
    uint32_t f_size = f.size();
    uint32_t e_size = e.size();

    assert(f_size == e_size);

    for (uint32_t i = 0; i < f_size; ++i) {
        _m_step_one_sen(f[i], e[i]);
    }

    //排序
    vector<pair_map> score_vec;
    map<pair<uint64_t, uint64_t>, double>::iterator iter; 
    for (iter = term_prob.begin(); iter != term_prob.end(); ++iter) {
        score_vec.push_back(*iter);
    }
    sort(score_vec.begin(), score_vec.end(), cmp());
    
    for (uint32_t i = 0; i < score_vec.size(); ++i) {
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

int32_t IBM_Model_One::
_calc_sen_end(int32_t thread_id) {
    int32_t sen_num_avg = f.size() / _thread_num;
    int32_t end = -1;
    if (thread_id == _thread_num) {
        end = f.size() - 1;
    }else {
        end = sen_num_avg * (thread_id + 1) - 1;
    }
    return end;
}

int32_t IBM_Model_One::
_calc_sen_start(int32_t thread_id) {
    int32_t sen_num_avg = f.size() / _thread_num;
    return sen_num_avg * (thread_id);
}

bool IBM_Model_One::
_train_thread(int32_t thread_id) {
    int32_t start = _calc_sen_start(thread_id);
    int32_t end = _calc_sen_end(thread_id);
    printf("thread_id : %d, start : %d, end : %d\n", thread_id, start, end);
    _e_step_thread(start, end);
    return true;
}

bool IBM_Model_One::
_start_e_step_thread() {
    std::vector<std::thread> threads;
    for (int32_t i = 0; i < _thread_num; ++i) {
        threads.push_back(std::thread([=]() { _train_thread(i); }));
    }

    //保证所有线程能够创建完成
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    printf("create thread done \n");

    for (int32_t i = 0; i < _thread_num; ++i) {
        threads[i].join();
    }

    printf("thread run done \n");

    return true;
}

bool IBM_Model_One::
_e_step_thread(int32_t start, int32_t end) {
    for (int32_t i = start; i <= end; ++i) {
        vector<string> f_sen = f[i];
        vector<string> e_sen = e[i];
        _calc_sen_increment(f_sen, e_sen);
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
void IBM_Model_One::
_calc_sen_increment(const vector<string>& f_sen,
                    const vector<string>& e_sen) {
    for (int i = 0; i < f_sen.size(); ++i) {
        const string f_term = f_sen[i];
        uint64_t f_term_idx = _term_index->get_term_index(f_term);

        double f_sum = _calc_sen_sum_increment(f_term, e_sen);

        for (int j = 0; j < e_sen.size(); ++j) {
            const string e_term = e_sen[j];
            uint64_t e_term_idx = _term_index->get_term_index(e_term);

            double f_e_value = term_prob[make_pair(e_term_idx, f_term_idx)];
            double one_incre = f_e_value / f_sum;

            _mutex.lock();
            f_e_co_occur_count[make_pair(e_term_idx, f_term_idx)] += one_incre;
            e_count[e_term_idx] += one_incre;
            _mutex.unlock();
        }
    }
}

/**
 * @brief : 计算一个f_term对一个英文句子总量
 * @param f_term : 法文一个term
 * @param e_sen : 英文分句
 **/
double IBM_Model_One::
_calc_sen_sum_increment(const string& f_term,
                        const vector<string>& e_sen) {
   uint64_t f_term_idx = _term_index->get_term_index(f_term);
   double sum = 0.0;
   for (int i = 0; i < e_sen.size(); ++i) {
       uint64_t e_term_idx = _term_index->get_term_index(e_sen[i]);
       pair<uint64_t, uint64_t> term_pair = make_pair(e_term_idx, f_term_idx);
       //词典中有值
       if (term_prob.find(term_pair) != term_prob.end()) {
           sum += term_prob[term_pair];
       }
   }
   return sum;
}
