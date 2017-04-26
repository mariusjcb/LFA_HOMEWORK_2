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

int readStartState(ifstream *file = NULL) {
    if(file == NULL)
    {
        char buff[125];
    
        fgets(buff, sizeof(buff),stdin);
        char *p = strtok(buff, " ");
    
        return atoi(p);
    } else {
        int x = -1;
        *file >> x;
        return x;
    }
}

int main(int argc, const char * argv[])
{
    vector<Edge> edges;
    
    ifstream f("inputs.txt");
    int start_state = readStartState(&f);
    
    return 0;
}
