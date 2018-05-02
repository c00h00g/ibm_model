#include "ibm_model.h"

int main() {
    int max_iter_num = 10;
    IBM_Model_One * ibm_model = new IBM_Model_One(max_iter_num);
    ibm_model->load_data("../data/trans_data.500");

    cout << "load data success!" << endl;

    ibm_model->init();

    cout << "ibm model init  success!" << endl;

    ibm_model->train();

    cout << "ibm model train finish!" << endl;

    return 0;
}
