#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

using std::fstream;
using std::ifstream;
using std::string;
using std::cout;
using std::endl;

int main ( int argc, char const *argv[] ) {
    ifstream fin ( argv[1], std::ios::in );

    string tmp;
    fin >> tmp;

    cout << tmp << "<<<" << endl;

    return 0;
}
