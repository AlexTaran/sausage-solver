all: build
build:
	g++ -std=c++2a -O2 -o solver solver.cpp 
run:
	./solver
clean:
	rm -f ./solver
