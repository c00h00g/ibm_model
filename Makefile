CXX=g++
RM=rm -f

all: ibm_model

ibm_model : ibm_model.o
	   g++ -o ibm_model ibm_model.o

ibm_model.o : ibm_model.h ibm_model.cpp
	   g++ -c ibm_model.cpp


clean:
	$(RM) ibm_model.o ibm_model
