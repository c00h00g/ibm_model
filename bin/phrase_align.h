#ifndef _PHRASE_ALIGN_H
#define _PHRASE_ALIGN_H

#include <map>
#include <utility> // pair
#include <assert.h>
#include <string>
#include <vector>
#include <fstream>
#include <memory.h>

#include "utils.h"
#include "term_idx_trans.h"

using std::pair;
using std::map;
using std::make_pair;
using std::string;
using std::vector;
using std::ifstream;

typedef map<pair<long long, long long>, double> AlignPro;
typedef map<pair<long long, long long>, double>::iterator AlignProIter;

static const int MAX_MAT_LEN = 50;

class Phrase_Align {
public:
    Phrase_Align();

    ~Phrase_Align();

    void load_align_dict(const char * file_name, AlignPro& align_dict);

    void load_all_data(const char * ori_file,
                       const char * forward_align_dict,
                       const char * reverse_align_dict,
                       const char * term_id_file);

    bool check_mat_valid(int f_start, 
                         int f_end, 
                         int e_start, 
                         int e_end,
                         int len_f,
                         int len_e);

    void get_ngram_align(const vector<string>& f_sen,
                         const vector<string>& e_sen);

    void debug_term_idx();

    void load_original_data(const char * file_name);

    void build_align_matrix(const vector<string>& f_sen,  
                            const vector<string>& e_sen);

    void gen_ngram_pair(const vector<string>& f_sen,
                        const vector<string>& e_sen,
                        int f_start, 
                        int f_end,
                        int e_start,
                        int e_end);

    void calc_row_col_sum();

    void deal_all_sen_pair();

    void deal_one_sen_pair(const vector<string>& f_sen,
                           const vector<string>& e_sen);

    void stat_ngram_prob();

private:
    //原始数据
    vector<vector<string> > f; //france segment
    vector<vector<string> > e; //english segment

    //映射
    AlignPro f_e_ngram_co_cnt;
    map<long long, double> e_ngram_cnt;
    

    //如果i和j && j和i都是对齐的，则mat_pro元素为1
    int mat_pro[MAX_MAT_LEN][MAX_MAT_LEN];

    int mat_pro_reverse[MAX_MAT_LEN][MAX_MAT_LEN];

    //每行每列1个数的总和
    int row_sum[MAX_MAT_LEN];
    int column_sum[MAX_MAT_LEN];

    TermIdxTrans * _term_idx;

    //正向概率
    AlignPro forward_align;

    //反向概率
    AlignPro reverse_align;

    //no copy allowed
    Phrase_Align(const Phrase_Align&);
    void operator=(const Phrase_Align&);
};

#endif
