CXX=g++
RM=rm -f

ibm_model.a : ibm_model.o
	   ar rcs $@ $<

ibm_model.o : ibm_model.cc ibm_model.h
	   g++ -c $<


clean:
	$(RM) ibm_model.o ibm_model.a
