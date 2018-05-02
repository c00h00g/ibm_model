#include <map>
#include <string>

using std::map;
using std::string;

typedef map<string, long long>::iterator TermIdxIter;
typedef map<long long, string>::iterator IdxTermIter;

class TermIdxTrans {
public:
    TermIdxTrans();

    long long insert(const string& term);

    long long get_term_index(const string& term);

    string get_index_cor_term(long long term_idx);

private:
    map<string, long long> _term_to_index;
    map<long long, string> _index_to_term;
    int _cur_index;
};
