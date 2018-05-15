#ifndef _IBM_MODEL_H
#define _IBM_MODEL_H

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <utility> // pair
#include <fstream>
#include <iostream>
#include <assert.h>
#include <set>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

#include "utils.h"
#include "term_idx_trans.h"

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;
using std::set;

static const uint32_t TERM_SIZE = 100000;

//ibm model 1
class IBM_Model_One {
public:
    IBM_Model_One(uint32_t max_num, 
                  uint32_t direction,
                  uint32_t thread_num);

    ~IBM_Model_One();

    void init();

    void init_one_step();

    void init_term_freq();

    void init_term_count_freq(const vector<string>& e_sen, 
                              const vector<string>& f_sen);

    bool load_data(const string& f_name);

    void train();
    bool _e_step();
    bool _m_step();

    void _m_step_one_sen(const vector<string>& f_sen,
                         const vector<string>& e_sen);

    void _calc_sen_increment(const vector<string>& f_sen,
                              const vector<string>& e_sen);

    double _calc_sen_sum_increment(const string& f_term,
                                   const vector<string>& e_sen);

    void deal_data_forword(const string& input);

    void deal_data_reverse(const string& input);

    void debug_info();

    void dump_prob(const char * file_name);

    //多线程
    bool _start_e_step_thread();
    bool _train_thread(int thread_id);
    bool _e_step_thread(int start, int end);
    int _calc_sen_start(int thread_id);
    int _calc_sen_end(int thread_id);

private:
    vector< vector<string> > f; //france segment
    vector< vector<string> > e; //english segment

    map<pair<uint64_t, uint64_t>, double> f_e_co_occur_count; //f and e co occur times
    map<uint64_t, double> e_count; // e occur times

    map<pair<uint64_t, uint64_t>, double> term_prob; //term f and term e parameters, 最终输出
    double init_prob; // term map start probability

    int _max_iter_num;

    int _thread_num;
    std::mutex _mutex;

    //训练的方向
    //0: 正向
    //1: 反向
    int _direction;

    //term-->index索引
    TermIdxTrans * _term_index;

    //no copy allowed
    IBM_Model_One(const IBM_Model_One&);
    void operator=(const IBM_Model_One&);
};

#endif
