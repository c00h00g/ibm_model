#include<string>
#include<iostream>
#include<vector>
#include<map>
#include<utility> // pair

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;

//ibm model 1
class IBM_Model_One {
public:
	IBM_Model_One(int max_num);

	bool load_data(const string& f_name);
	bool train();
	bool _e_step();
	bool _m_step();

    bool _calc_increment(
		   const vector<string>& one_f_sen,
		   const vector<string>& one_e_sen);

    double _calc_one_increment(
		   const string& one_f_word,
		   const string& one_e_word,
		   const vector<string>& one_e_sen);
private:
	vector<vector<string> >  f; //france segment
	vector<vector<string> > e; //english segment
	map<pair<string, string>, double> f_e_co_occur_count; //f and e co occur times
	map<string, double> f_e_count;
	int _max_iter_num;
};
