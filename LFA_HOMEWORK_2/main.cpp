//
//  main.cpp
//  LFA_HOMEWORK_2
//
//  Created by Marius Ilie on 26/04/2017.
//  Copyright © 2017 Marius Ilie. All rights reserved.
//

//MARK: C libraries
#include <stdlib.h>

//MARK: C++ libraries
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>

using namespace std;

const int FileLineSize = 125;

struct Edge {
    int start;
    char key;
    int end;
};

ifstream& GotoLine(ifstream& file, unsigned int num){
    file.seekg(ios::beg);
    for(int i=0; i < num - 1; ++i)
        file.ignore(FileLineSize,'\n');

    return file;
}

char* readBufferFromConsoleInput() {
    static char buff[FileLineSize];
    
    fgets(buff, sizeof(buff), stdin);
    return strtok(buff, " ");
}

int readStartState(ifstream *file = NULL) {
    if(file == NULL)
    {
        return atoi(readBufferFromConsoleInput());
    } else {
        int x = -1;
        *file >> x;
        
        return x;
    }
}

vector<int> readFinalStates(ifstream *file = NULL) {
    vector<int> finalStates;
    char *p = NULL;
    
    if(file == NULL)
    {
        p = readBufferFromConsoleInput();
    } else {
        string line;
        GotoLine(*file, 2);
        getline(*file, line);
        
        char *cstr = new char[line.length() + 1];
        strcpy(cstr, line.c_str());
        
        p = strtok(cstr, " ");
    }
    
    while (p != NULL)
    {
        int state = atoi(p);
        finalStates.push_back(state);
        
        p = strtok(NULL, " ");
    }
    
    return finalStates;
}

int main(int argc, const char * argv[]) {
    vector<Edge> edges;
    
    ifstream f("inputs.txt");
    
    int start_state = readStartState(argc == 0 ? &f : NULL);
    vector<int> final_states = readFinalStates(argc == 0 ? &f : NULL);
    vector<int> transition_states = readFinalStates(argc == 0 ? &f : NULL);
    
    cout << start_state << "\n";
    for(int i = 0; i < final_states.size(); i++) {
        cout << final_states[i] << " ";
    }
    
    return 0;
}
