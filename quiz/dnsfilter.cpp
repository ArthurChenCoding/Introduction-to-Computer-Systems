#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <fstream>
#include <math.h>
using namespace std;

int main (int argc, char *argv[]){
    cout << "sidenote: my dig +short www.google.com command will only generate one IP address, therefore, it is possible that none of the result mathches your request." << endl;
    string host = argv[1];
    string target = argv[2];
    string command = "dig +short " + host + " >> temp.txt"; // compose command
    ifstream f("temp.txt");
    system(command.c_str()); // store IP into temp.txt
    string line;
    if (f.is_open()){
        while (getline (f,line)){
            if (line.find(target) != std::string::npos){ // cout if found target
                cout << line << '\n';
            }
        }
        f.close();
    }else {
        cout << "Unable to open file"; 
    }
    cout << "program ended, if there is no output, please try to target something else" << endl;
    return 0;
}