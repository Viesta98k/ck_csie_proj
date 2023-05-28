vocab_master: main.o crawler.o csv.o core.o vocab.o
	gcc -o vocab_master main.o crawler.o csv.o core.o vocab.o
main.o: 
	gcc -c main.cpp
csv.o: csv.hpp
	gcc -c csv.cpp
core.o: core.hpp
	gcc -c core.cpp
vocab.o:
	gcc -c vocab.cpp