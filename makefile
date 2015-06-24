all:
	clang++ -g -o jsontest -std=c++11 Json.cpp main.cpp
clean:
	rm -rf jsontest
