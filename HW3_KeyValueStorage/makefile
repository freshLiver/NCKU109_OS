all:
	g++ -g -c -std=c++14 ./DBWorker.cpp -o DBWorker.o
	g++ -g -c -std=c++14 ./KeyValueStorage.cpp -o KeyValueStorage.o
	g++ -g -c -std=c++14 ./main.cpp -o main.o
	g++ -g -std=c++14 -pthread DBWorker.o KeyValueStorage.o main.o -o main

rm:
	rm ./hw3example.output
	rm -r ./db/