
CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
BIN=test

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^
	rm -f *.o

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o $(BIN)