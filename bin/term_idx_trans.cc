#include "term_idx_trans.h"

TermIdxTrans::TermIdxTrans() {
    _term_to_index.clear();
    _index_to_term.clear();
    _cur_index = 0;
}

uint64_t TermIdxTrans::insert(const string& term) {
    TermIdxIter t_i_iter = _term_to_index.find(term);
    //find it
    if (t_i_iter != _term_to_index.end()) {
        return _term_to_index[term];
    }

    _term_to_index[term] = _cur_index;
    _index_to_term[_cur_index] = term;
    _cur_index += 1;

    return _cur_index - 1;
}

uint64_t TermIdxTrans::get_term_index(const string& term) {
    TermIdxIter t_i_iter = _term_to_index.find(term);
    //find it
    if (t_i_iter != _term_to_index.end()) {
        return _term_to_index[term];
    }

    return -1;
}

string TermIdxTrans::
get_index_cor_term(uint64_t term_idx) {
    if (term_idx >= _cur_index) {
        return "";
    }

    return _index_to_term[term_idx];
}

void TermIdxTrans::
print_term_to_index() {
    TermIdxIter iter;
    cout << "term_idx size : " << _term_to_index.size() << endl;
    for (iter = _term_to_index.begin(); iter != _term_to_index.end(); ++iter) {
        string key = iter->first;
        uint64_t value = iter->second;
        cout << key << ":" << value << endl;
    }
}

void TermIdxTrans::
print_index_to_term() {
    IdxTermIter iter;
    cout << "idx_to_term size : " << _index_to_term.size() << endl;
    for (iter = _index_to_term.begin(); iter != _index_to_term.end(); ++iter) {
        uint64_t key = iter->first;
        string value = iter->second;
        cout << key << ":" << value << endl;
    }
}

/**
 * @brief : dump中间数据，用来生成phrase对齐
 **/
void TermIdxTrans::
dump_data(const char * file_name) {
    //dump term -> id映射关系
    ofstream td_mp_file;
    td_mp_file.open(file_name);

    TermIdxIter iter;
    for (iter = _term_to_index.begin(); iter != _term_to_index.end(); ++iter) {
        string key = iter->first;
        uint64_t value = iter->second;
        td_mp_file << key << "\t" << value << endl;
    }

    //关闭
    td_mp_file.close();

    return;
}

/**
 * @brief : 加载term ==> id映射数据
 **/
void TermIdxTrans::
load_data(const char * file_name) {
    ifstream fin(file_name);
    string input;
    while (getline(fin, input)) {
        vector<string> line_split;
        string split_sep = "\t";
        split_string(input, line_split, split_sep);
        if (line_split.size() != 2) {
            return;
        }
        string key = line_split[0];
        uint64_t value = atoi(line_split[1].c_str());
        _term_to_index[key] = value;
        _index_to_term[value] = key;
    }
}

