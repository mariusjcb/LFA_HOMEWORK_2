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

using namespace std;

struct Edge {
    int start;
    char key;
    int end;
};

char* readBufferFromConsoleInput() {
    static char buff[125];
    
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
        getline(*file, line);
        
        p = strtok(strdup(line.c_str()), " ");
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
    
    int start_state = readStartState(stdin == NULL ? &f : NULL);
    vector<int> final_states = readFinalStates(stdin == NULL ? &f : NULL);
    
    cout << start_state << "\n";
    for(int i = 0; i < final_states.size(); i++) {
        cout << final_states[i] << " ";
    }
    
    return 0;
}
