all:vocabmaster.exe vocab.exe vocfile.dll

vocabmaster.exe:src/vocabmaster.cpp src/vocfile.hpp
	g++ -c src/vocabmaster.cpp -include src/vocfile.hpp
vocab.exe:src/vocab.cpp src/vocfile.hpp
	g++ -c src/vocab.cpp -include src/vocfile.hpp
vocfile.dll:src/vocfile.cpp src/vocfile.hpp
	g++ -c src/vocfile.cpp -include src/vocfile.hpp