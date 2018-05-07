CXX=g++
RM=rm -f

# generate lib
#ibm_model.a : ibm_model.o
#	   ar rcs $@ $<

ibm_model : main.o ibm_model.o utils.o term_idx_trans.o phrase_align.o
	$(CXX) -o $@ $^

main.o : main.cc ibm_model.h
	$(CXX) -c $<

utils.o : utils.cc utils.h
	$(CXX) -c $<

term_idx_trans.o : term_idx_trans.cc term_idx_trans.h
	$(CXX) -c $<

ibm_model.o : ibm_model.cc ibm_model.h utils.h
	$(CXX) -c $<

phrase_align.o : phrase_align.cc phrase_align.h utils.h
	$(CXX) -c $<

clean:
	$(RM) main.o ibm_model.o utils.o ibm_model
