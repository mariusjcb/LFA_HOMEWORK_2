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

const int FileLineSize = 125; // OR: numeric_limits<streamsize>::max()

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

vector<Edge> readTransitionStates(ifstream *file = NULL) {
    vector<Edge> transitionStates;
    
    if(file == NULL) {
        int start;
        char key;
        int end;
        
        while(fscanf(stdin, "%d %c %d", &start, &key, &end) != EOF) {
            Edge currentEdge = *new Edge;
            
            currentEdge.start = start;
            currentEdge.key = key;
            currentEdge.end = end;
            
            transitionStates.push_back(currentEdge);
        }
    } else {
        GotoLine(*file, 3);
    
        while(!(*file).eof())
        {
            char *p = NULL;
            
            string line;
            getline(*file, line);
            
            if(line.length())
            {
                char *cstr = new char[line.length() + 1];
                strcpy(cstr, line.c_str());
                
                Edge currentEdge = *new Edge;
                
                p = strtok(cstr, " ");
                currentEdge.start = atoi(p);
                
                p = strtok(NULL, " ");
                currentEdge.key = *p;
                
                p = strtok(NULL, " ");
                currentEdge.end = atoi(p);
                
                transitionStates.push_back(currentEdge);
            }
        }
    }
    
    return transitionStates;
}

int main(int argc, const char * argv[]) {
    ifstream f("inputs.txt");
    vector<Edge> edges;
    
    int start_state = readStartState(f.is_open() ? &f : NULL);
    vector<int> final_states = readFinalStates(f.is_open() ? &f : NULL);
    vector<Edge> transition_states = readTransitionStates(f.is_open() ? &f : NULL);
    
    cout << start_state << "\n";
    for(int i = 0; i < final_states.size(); i++) {
        cout << final_states[i] << " ";
    }
    
    cout << "\n";
    for(int i = 0; i < transition_states.size(); i++) {
        cout << transition_states[i].start << " " << transition_states[i].key << " " << transition_states[i].end << "\n";
    }
    
    return 0;
}
