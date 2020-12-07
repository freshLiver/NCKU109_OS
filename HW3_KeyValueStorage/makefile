all:
	g++ -g -c -std=c++14 -pthread ./KeyValueStorage.cpp -o KeyValueStorage.o
	g++ -g -c -std=c++14 -pthread ./main.cpp -o main.o
	g++ -g -std=c++14 -pthread KeyValueStorage.o main.o -o main

rm:
	rm -f ./*.output
	rm -f ./*.o
	rm -rf ./db/
