#include <thread>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <ctime>

#define println(fmt, ...) printf(fmt"\n", __VA_ARGS__)

using namespace std;

int main(int argc, char const *argv[])
{
    // init string array
    string result[10000];

    // start output 
    clock_t start = clock();
    FILE *output = fopen ("output.txt","w");


    // time cout
    println("output cost time : %lu", clock() - start );
    return 0;
}
