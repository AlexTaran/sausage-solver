all: build
build:
	g++ -O2 -o solver solver.cpp 
run:
	./solver
clean:
	rm -f ./solver
