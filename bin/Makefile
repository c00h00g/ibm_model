CXX=g++
RM=rm -f

# generate lib
#ibm_model.a : ibm_model.o
#	   ar rcs $@ $<

ibm_model : main.o ibm_model.o utils.o
	$(CXX) -o $@ $^

main.o : main.cc ibm_model.h
	$(CXX) -c $<

utils.o : utils.cc utils.h
	$(CXX) -c $<

ibm_model.o : ibm_model.cc ibm_model.h utils.h
	$(CXX) -c $<

idx_term_tran.o : term_idx_trans.cc term_idx_trans.h
	$(CXX) -c $<

clean:
	$(RM) main.o ibm_model.o utils.o ibm_model
