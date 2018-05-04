#include <map>
#include <string>
#include <iostream>

using std::map;
using std::string;
using std::cout;
using std::endl;

typedef map<string, long long>::iterator TermIdxIter;
typedef map<long long, string>::iterator IdxTermIter;

class TermIdxTrans {
public:
    TermIdxTrans();

    long long insert(const string& term);

    long long get_term_index(const string& term);

    string get_index_cor_term(long long term_idx);

    void print_term_to_index();
    void print_index_to_term();

private:
    map<string, long long> _term_to_index;
    map<long long, string> _index_to_term;
    //当前index
    long long _cur_index;
};
