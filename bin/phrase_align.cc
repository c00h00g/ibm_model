#include "phrase_align.h"

Phrase_Align::
Phrase_Align() {
    _term_idx = new(std::nothrow) TermIdxTrans();
    assert(NULL != _term_idx);
    f.clear();
    e.clear();
    f_e_ngram_co_cnt.clear();
    e_ngram_cnt.clear();
}

Phrase_Align::
~Phrase_Align() {
    if (NULL != _term_idx) {
        delete _term_idx;
        _term_idx = NULL;
    }
}

/**
 * @brief : 加载ibm生成的反向的词典
 * @param file_name : 输入文件名
 **/
void Phrase_Align::
load_align_dict(const char * file_name, AlignPro& align_dict) {
    ifstream fin(file_name);
    string line;
    while (getline(fin, line)) {
        vector<string> line_split;
        string spliter = "\t";
        split_string(line, line_split, spliter);
        if (line_split.size() != 5) {
            continue;
        }
        uint64_t from = atoi(line_split[0].c_str());
        string from_term = line_split[1];
        uint64_t to = atoi(line_split[2].c_str());
        string to_term = line_split[3];
        double prob = atof(line_split[4].c_str());
        pair<uint64_t, uint64_t> one_pair = make_pair(from, to);
        align_dict[one_pair] = prob;
    }
}

/**
 * @brief : 加载原始翻译序列
 **/
void Phrase_Align::
load_original_data(const char * file_name) {
    assert(NULL != file_name);
    ifstream fin(file_name);
    string input;
    while (getline(fin, input)) {
        vector<string> e_f_vec;
        string split_one_sep = "\t";
        split_string(input, e_f_vec, split_one_sep);

        string split_two_sep = " ";

        //split f
        vector<string> f_vec;
        split_string(e_f_vec[0], f_vec, split_two_sep);

        f.push_back(f_vec);

        //split e
        vector<string> e_vec;
        split_string(e_f_vec[1], e_vec, split_two_sep);

        e.push_back(e_vec);
    }

#if 0
    for (int i = 0; i < f.size(); ++i) {
        for (int j = 0; j < f[i].size(); ++j) {
            cout << f[i][j] << " ";
        }
        cout << endl;

        for (int j = 0; j < e[i].size(); ++j) {
            cout << e[i][j] << " ";
        }
        cout << endl;
    }
#endif

    return;
}

/**
 * @brief : 加载term id映射词典和对齐概率词典
 **/
void Phrase_Align::
load_all_data(const char * ori_file,
              const char * forward_align_dict,
              const char * reverse_align_dict,
              const char * term_id_file) {
    assert(NULL != ori_file);
    assert(NULL != forward_align_dict);
    assert(NULL != reverse_align_dict);
    assert(NULL != term_id_file);

    load_original_data(ori_file);

    load_align_dict(forward_align_dict, forward_align);
    load_align_dict(reverse_align_dict, reverse_align);

    _term_idx->load_data(term_id_file);
}

/**
 * @brief : 判断选中的长方形是否满足要求
 **/
bool Phrase_Align::
check_mat_valid(int f_start, 
                int f_end, 
                int e_start, 
                int e_end,
                int len_f,
                int len_e) {

    int select_len_f = f_end - f_start + 1;
    int select_len_e = e_end - e_start + 1;

    //if (select_len_f <= 1 && select_len_e <= 1) {
    //    return false;
    //}

    //判断边缘的sum之和是否等于行的sum之和

    //最上面一行
    int sum = 0;
    for (int i = e_start; i <= e_end; ++i) {
        sum += mat_pro[f_start][i];
    }
    if (sum != row_sum[f_start] || sum <= 0) {
        return false;
    }

    //最下面一行
    sum = 0;
    for (int i = e_start; i <= e_end; ++i) {
        sum += mat_pro[f_end][i];
    }
    if (sum != row_sum[f_end] || sum <= 0) {
        return false;
    }

    //最左边一列
    sum = 0;
    for (int i = f_start; i <= f_end; ++i) {
        sum += mat_pro[i][e_start];
    }
    if (sum != column_sum[e_start] || sum <= 0) {
        return false;
    }

    //最右边一列
    sum = 0;
    for (int i = f_start; i <= f_end; ++i) {
        sum += mat_pro[i][e_end];
    }
    if (sum != column_sum[e_end] || sum <= 0) {
        return false;
    }

    return true;
}

void Phrase_Align::
calc_row_col_sum() {
    memset(row_sum, sizeof(row_sum), 0);
    memset(column_sum, sizeof(column_sum), 0);

    //对于每一行
    for (int i = 0; i < MAX_MAT_LEN; ++i) {
        int sum = 0;
        for (int j = 0; j < MAX_MAT_LEN; ++j) {
            sum += mat_pro[i][j];
        }
        row_sum[i] = sum;
    }

    //对于每一列
    for (int i = 0; i < MAX_MAT_LEN; ++i) {
        int sum = 0;
        for (int j = 0; j < MAX_MAT_LEN; ++j) {
            sum += mat_pro[j][i];
        }
        column_sum[i] = sum;
    }

    return;
}

/**
 * @brief : 生成ngram数据
 **/
void Phrase_Align::
gen_ngram_pair(const vector<string>& f_sen,
               const vector<string>& e_sen,
               int f_start, 
               int f_end,
               int e_start,
               int e_end) {
    //产生f ngram
    string f_ngram = "";
    for (int i = f_start; i <= f_end; ++i) {
        if (i != f_end) {
            f_ngram += f_sen[i] + " ";
        } else {
            f_ngram += f_sen[i];
        }
    }

    //产生e ngram
    string e_ngram = "";
    for (int i = e_start; i <= e_end; ++i) {
        if (i != e_end) {
            e_ngram += e_sen[i] + " ";
        } else {
            e_ngram += e_sen[i];
        }
    }

    cout << "f_ngram :" << f_ngram << "e_ngram :" << e_ngram << endl;

    //插入term-id词典
    _term_idx->insert(f_ngram);
    _term_idx->insert(e_ngram);

    int f_ngram_idx = _term_idx->get_term_index(f_ngram);
    int e_ngram_idx = _term_idx->get_term_index(e_ngram);
    
    pair<uint64_t, uint64_t> f_e_pair = make_pair(f_ngram_idx, e_ngram_idx);
    if (f_e_ngram_co_cnt.find(f_e_pair) != f_e_ngram_co_cnt.end()) {
        f_e_ngram_co_cnt[make_pair(f_ngram_idx, e_ngram_idx)] += 1;
    }else {
        f_e_ngram_co_cnt[make_pair(f_ngram_idx, e_ngram_idx)] = 1;
    }

    if (e_ngram_cnt.find(e_ngram_idx) != e_ngram_cnt.end()) {
        e_ngram_cnt[e_ngram_idx] += 1;
    }else {
        e_ngram_cnt[e_ngram_idx] = 1;
    }

    return;
}

/**
 * @brief : 获取ngram对齐
 **/
void Phrase_Align::
get_ngram_align(const vector<string>& f_sen,
                const vector<string>& e_sen) {

    int len_f = f_sen.size() > MAX_MAT_LEN ? MAX_MAT_LEN : f_sen.size();
    int len_e = e_sen.size() > MAX_MAT_LEN ? MAX_MAT_LEN : e_sen.size();

    //枚举所有的长方形
    for (int s_i = 0; s_i < len_f; ++s_i) {
        for (int e_i = s_i; e_i < len_f; ++e_i) {
            for (int s_j = 0; s_j < len_e; ++s_j) {
                for (int e_j = s_j; e_j < len_e; ++e_j) {
                    //cout << "f_start:" << s_i << "||"
                    //     << "f_end:" << e_i << "||"
                    //     << "e_start:" << s_j << "||"
                    //     << "e_end:" << e_j << "||" << endl;

                    bool valid = check_mat_valid(s_i, e_i, s_j, e_j, len_f, len_e);
                    if (false == valid) {
                        continue;
                    }
                    //有效
                    gen_ngram_pair(f_sen, e_sen, s_i, e_i, s_j, e_j);
                }
            }
        }
    }

    return;
}

/**
 * @根据数据构造对齐数组
 **/ 
void Phrase_Align::
build_align_matrix(const vector<string>& f_sen,  
                   const vector<string>& e_sen) {
    //初始化
    memset(mat_pro, 0, sizeof(mat_pro));
    memset(mat_pro_reverse, 0, sizeof(mat_pro_reverse));

    double MIN_PROB = 0.2;
    int len_f = f_sen.size() > MAX_MAT_LEN ? MAX_MAT_LEN : f_sen.size();
    int len_e = e_sen.size() > MAX_MAT_LEN ? MAX_MAT_LEN : e_sen.size();

    for (int i = 0; i < len_f; ++i) {
        for(int j = 0; j < len_e; ++j) {
            string term_f = f_sen[i];
            string term_e = e_sen[j];
            uint64_t f_index = _term_idx->get_term_index(term_f);
            uint64_t e_index = _term_idx->get_term_index(term_e);

            double f_2_e = reverse_align[make_pair(f_index, e_index)];
            double e_2_f = forward_align[make_pair(e_index, f_index)];

            printf("france:%s, english:%s, f_index:%d, e_index:%d, f_2_e:%0.4f, e_2_f:%0.4f\n",
                      term_f.c_str(),
                      term_e.c_str(),
                      f_index,
                      e_index,
                      f_2_e,
                      e_2_f);

            if ((f_2_e > MIN_PROB || e_2_f > MIN_PROB) && f_index != e_index) {
                cout << term_f << ":" << f_index << ":" << f_2_e << "||"
                     << term_e << ":" << e_index << ":" << e_2_f << endl;
                mat_pro[i][j] = 1;
                mat_pro_reverse[i][j] = 1;
            }
        }
    }

#if 1
    //output english headers
    cout << "            ";
    for (int i = 0; i < len_e; ++i) {
        string term_e = e_sen[i];
        cout << term_e << "                ";
    }
    cout << endl;

    //output probility
    for (int i = 0; i < len_f; ++i) {
        string term_f = f_sen[i];
        cout << term_f << "  ";
        for(int j = 0; j < len_e; ++j) {
            //cout << mat_pro[i][j];
            //printf("%0.4f||%0.4f", mat_pro[i][j], mat_pro_reverse[i][j]);
            printf("%d||%d", mat_pro[i][j], mat_pro_reverse[i][j]);
            for (int k = 0; k < e_sen[j].size() + 4; ++k) {
                cout << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
#endif

    return;
}

/**
 * @brief : 处理所有的句对
 **/
void Phrase_Align::
deal_all_sen_pair() {
    assert(f.size() == e.size());

    for (int i = 0; i < f.size(); ++i) {
        deal_one_sen_pair(f[i], e[i]);
    }

    stat_ngram_prob();
}

/**
 * @brief : 处理一个句对
 **/
void Phrase_Align::
deal_one_sen_pair(const vector<string>& f_sen,
                  const vector<string>& e_sen) {
    //构造对齐矩阵
    build_align_matrix(f_sen, e_sen);

    //计算每行每列sum之和
    calc_row_col_sum();

    //生成ngram对齐
    get_ngram_align(f_sen, e_sen);
}

void Phrase_Align::
stat_ngram_prob() {
    for (AlignProIter iter = f_e_ngram_co_cnt.begin(); iter != f_e_ngram_co_cnt.end(); ++iter) {
        pair<uint64_t, uint64_t> one_pair = iter->first;
        double f_e_cnt = iter->second;

        uint64_t f_term_idx = one_pair.first;
        uint64_t e_term_idx = one_pair.second;

        double e_cnt = 0.0;
        if (e_ngram_cnt.find(e_term_idx) == e_ngram_cnt.end()) {
            continue;
        }
        e_cnt = e_ngram_cnt[e_term_idx];

        double rate = f_e_cnt * 1.0 / e_cnt;
        
        string f_ngram = _term_idx->get_index_cor_term(f_term_idx);
        string e_ngram = _term_idx->get_index_cor_term(e_term_idx);

        cout << f_ngram << "||" << e_ngram << "||" << e_cnt << "||" << f_e_cnt << "||" << rate << endl;
    }
}

void Phrase_Align::
debug_term_idx() {
    _term_idx->print_term_to_index();
    return;
}

