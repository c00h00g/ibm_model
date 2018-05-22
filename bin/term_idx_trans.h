#ifndef _TERM_IDX_TRANS_
#define _TERM_IDX_TRANS_

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include "utils.h"

using std::map;
using std::string;
using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::vector;

typedef map<string, uint64_t>::iterator TermIdxIter;
typedef map<uint64_t, string>::iterator IdxTermIter;

class TermIdxTrans {
public:
    TermIdxTrans();

    uint64_t insert(const string& term);

    uint64_t get_term_index(const string& term);

    string get_index_cor_term(uint64_t term_idx);

    void print_term_to_index();
    void print_index_to_term();

    void dump_data(const char * file_name);

    void load_data(const char * file_name);

private:
    map<string, uint64_t> _term_to_index;
    map<uint64_t, string> _index_to_term;
    //当前index
    uint64_t _cur_index;
};

#endif
