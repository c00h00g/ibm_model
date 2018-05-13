#include "ibm_model.h"
#include "phrase_align.h"


const char * DATA_PATH = "../data/trans_data";

static void run_ibm_model(int direction) {
    int max_iter_num = 10;
    int thread_num = 10;
    IBM_Model_One * ibm_model = new IBM_Model_One(max_iter_num, direction, thread_num);

    ibm_model->load_data(DATA_PATH);

    cout << "load data success!" << endl;

    ibm_model->init();

    cout << "ibm model init  success!" << endl;

    ibm_model->train();
    cout << "ibm model train finish!" << endl;

    delete ibm_model;
    ibm_model = NULL;
}

static void run_phrase_align() {
    Phrase_Align * phrase_align = new Phrase_Align();
    cout << "start load data" << endl;

    phrase_align->load_all_data(DATA_PATH,
                                "./ALIGN_PROB_FORWARD",
                                "./ALIGN_PROB_REVERSE",
                                "./TERM_INDEX_MP");

    cout << "after load data" << endl;

    phrase_align->debug_term_idx();

    phrase_align->deal_all_sen_pair();

    delete phrase_align;
}

int main() {
    //正向
    run_ibm_model(0);

    //反向
    run_ibm_model(1);

    //phrase 对齐
    run_phrase_align();

    return 0;
}
