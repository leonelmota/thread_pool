CXX=g++
CXX_FLAGS=-pthread -std=c++17

all: build

build: main.cpp
	${CXX} ${CXX_FLAGS} $^ -o prog
	
clean:
	rm -f *.o prog
