main: main.o verify.o util.o ast.o type.o
	g++ -Wall -std=c++11 -o main main.o verify.o util.o ast.o type.o
test: verify.o test.o util.o ast.o type.o
	g++ -Wall -std=c++11 -o test test.o verify.o util.o ast.o type.o
main.o: src/main.cpp
	g++ -Wall -std=c++11 -c src/main.cpp
verify.o: src/verify.cpp
	g++ -Wall -std=c++11 -c src/verify.cpp
util.o: src/util.cpp
	g++ -Wall -std=c++11 -c src/util.cpp
ast.o: src/ast.cpp
	g++ -Wall -std=c++11 -c src/ast.cpp
type.o: src/type.cpp
	g++ -Wall -std=c++11 -c src/type.cpp
test.o: src/test.cpp
	g++ -Wall -std=c++11 -c src/test.cpp
