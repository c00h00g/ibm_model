#include "ibm_model.h"

int main() {
    int max_iter_num = 3;
    IBM_Model_One * ibm_model = new IBM_Model_One(max_iter_num);
    ibm_model->load_data("../data/trans_data");
    ibm_model->init();

    ibm_model->train();

    return 0;
}
