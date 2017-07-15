main: main.o verify.o
	g++ -Wall -std=c++11 -o main main.o verify.o
test: verify.o test.o
	g++ -Wall -std=c++11 -o test test.o verify.o
main.o: src/main.cpp
	g++ -Wall -std=c++11 -c src/main.cpp
verify.o: src/verify.cpp
	g++ -Wall -std=c++11 -c src/verify.cpp
test.o: src/test.cpp
	g++ -Wall -std=c++11 -c src/test.cpp
