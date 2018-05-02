#include "term_idx_trans.h"

TermIdxTrans::TermIdxTrans() {
    _term_to_index.clear();
    _index_to_term.clear();
    _cur_index = 0;
}

long long TermIdxTrans::insert(const string& term) {
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

long long TermIdxTrans::get_term_index(const string& term) {
    TermIdxIter t_i_iter = _term_to_index.find(term);
    //find it
    if (t_i_iter != _term_to_index.end()) {
        return _term_to_index[term];
    }

    return -1;
}

string TermIdxTrans::
get_index_cor_term(long long term_idx) {
    if (term_idx >= _cur_index) {
        return "";
    }

    return _index_to_term[term_idx];
}
