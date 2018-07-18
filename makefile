all: marked

marked: main.cpp
	g++ -Wall -std=c++11 main.cpp parser.cpp lexer.cpp ast.cpp -o marked

clean:
	rm -rf marked
