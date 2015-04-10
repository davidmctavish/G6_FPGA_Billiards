	CXX=clang++
	CXXFLAGS=-g -std=c++11 -Wall
	BIN=prog

SRC=$(wildcard *.cpp)

OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^

#OBJECTS=physics.o test.o angles.o
#prog: $(OBJECTS)
#	$(CXX) $(OBJECTS) -o prog
#physics.o: physics.cpp
#	$(CXX) $(CXXFLAGS) -c physics.cpp -o physics.o
#angles.o: angles.cpp
#	$(CXX) $(CXXFLAGS) -c angles.cpp -o angles.o
#test.o: test.cpp
#	$(CXX) $(CXXFLAGS) -c test.cpp -o test.o

clean:
	    rm -f *.o
		    rm $(BIN)
