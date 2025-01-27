#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "file_system.hpp"

using namespace std;


bool isNum(const string& str) {
    for (char c : str)
        if (!isdigit(c))
            return false;
    return true;
}


int main() {    
    
    while (true) {
        string command;
        getline(cin, command); 
        
        vector<string> args;
        istringstream ss{command};

        for (string word; getline(ss, word, ' '); ) {
            args.push_back(word);
        }

        if (args.size() == 1 && args[0] == "in") {
            cout << initialize() << endl; 
        } else if (args.size() == 3 && args[0] == "wm" && isNum(args[1])) {
            int bytes = write_memory(stoi(args[1]), args[2]);
            cout << bytes << " bytes written to memory" << endl;
        } else if (args.size() == 3 && args[0] == "rm" && isNum(args[1])) {
            cout << read_memory(stoi(args[1]), stoi(args[2])) << endl;
        } else if (args.size() == 3 && args[0] == "sk" && isNum(args[1]) && isNum(args[2])) {
            int pos = seek(stoi(args[1]), stoi(args[2]));
            if (pos == -1) cout << "error" << endl;
            else cout << "position is " << pos << endl;
        } else if (args.size() == 2 && args[0] == "op") {
            int index = open(args[1]);
            if (index == -1) cout << "error" << endl;
            else cout << args[1] << " opened " << index << endl;
        } else if (args.size() == 2 && args[0] == "cl" && isNum(args[1])) {
            int index = close(stoi(args[1])); 
            if (index == -1) cout << "error" << endl;
            else cout << index << " closed" << endl;
        }


        else {
            cout << "error" << endl;
        }
    }

    return 0;
}
