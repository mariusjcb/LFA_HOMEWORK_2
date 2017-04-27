//
//  main.cpp
//  LFA_HOMEWORK_2
//
//  Created by Marius Ilie on 26/04/2017.
//  Copyright © 2017 Marius Ilie. All rights reserved.
//

//MARK: - Libraries
#include <stdlib.h>
#ifdef __APPLE__
    #include <unistd.h>
#else
    #include <io.h>
#endif

#include <iostream>
#include <algorithm>
#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>

using namespace std;

//MARK: - DEFINES
const int FileLineSize = 125; // OR: numeric_limits<streamsize>::max()

struct Edge {
    int start;
    char key;
    int end;
};

//MARK: - FILES USAGE
fstream* open_file(const char* file_name) {
    static fstream f(file_name);
    
    if(f.is_open())
        return &f;
    else cout << "File not found.\n";
    
    if (isatty(fileno(stdin)))
    {
        cout << "Change file name or...\n\nInsert file using '<your_file' in tty mode" << endl;
        exit(0);
    } else cout << "I am in tty mode...\n";
    
    return NULL;
}

ifstream* open_input_file(const char* file_name) {
    return (ifstream*)open_file(file_name);
}


ofstream* open_output_file(const char* file_name) {
    return (ofstream*)open_file(file_name);
}

ifstream& GotoLine(ifstream& file, unsigned int num){
    file.seekg(ios::beg);
    for(int i=0; i < num - 1; ++i)
        file.ignore(FileLineSize,'\n');

    return file;
}

//MARK: - READERS
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
        if(!(*file).eof())
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
        
        if(line.length())
        {
            char *cstr = new char[line.length() + 1];
            strcpy(cstr, line.c_str());
            
            p = strtok(cstr, " ");
        }
    }
    
    while (p != NULL)
    {
        int state = atoi(p);
        finalStates.push_back(state);
        
        p = strtok(NULL, " ");
    }
    
    return finalStates;
}

vector<Edge> readTransitionStates(int& max_node, ifstream *file = NULL) {
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
            
            if(max_node < currentEdge.start)
                max_node = currentEdge.start;
            
            if(max_node < currentEdge.end)
                max_node = currentEdge.end;
            
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
                
                if(max_node < currentEdge.start)
                    max_node = currentEdge.start;
                
                if(max_node < currentEdge.end)
                    max_node = currentEdge.end;
                
                transitionStates.push_back(currentEdge);
            }
        }
    }
    
    return transitionStates;
}

//MARK: - DATA USAGE
vector<int> find(int value, vector<int> into) {
    vector<int> index;
    
    for(int i = 0; i < into.size(); i++)
        if(into[i] == value)
            index.push_back(i);
    
    return index;
}

vector<int> find(int value, vector<Edge> into, int Edge::* field) {
    vector<int> index;
    
    for(int i = 0; i < into.size(); i++)
        if((&into[i])->*field == value)
            index.push_back(i);
    
    return index;
}

vector<int> find(int value, map< int, map<char, int> > into, const char key) {
    vector<int> index;
    
    for(int i = 0; i < into.size(); i++)
        if(into[i][key] == value)
            index.push_back(i);
    
    return index;
}

//MARK: - DFA Optimization
class DFA
{
    int start_state;
    vector<int> final_states;
    vector<Edge> transition_states;
    map< int, map<char, int> > partition_table;
    int max_node;
    int* reachable_nodes;
    int max_partnr;
    
public:
    void setStartState(int value) { start_state = value; }
    void setFinalStates(vector<int> value) { final_states = value; }
    void setTransitionStates(vector<Edge> value) { transition_states = value; }
    void setMaxNode(int value) { max_node = value; }
    
    DFA(ifstream *f = NULL) {
        setStartState(readStartState(f));
        setFinalStates(readFinalStates(f));
        
        int max_node = 0;
        setTransitionStates(readTransitionStates(max_node, f));
        setMaxNode(max_node);
        
        removeUnreachableNodes();
        buildPartitionTable();
    }
    
    void removeUnreachableNodes() {
        reachable_nodes = new int[max_node+1]();
        
        for(int i = 0; i < transition_states.size(); i++)
            reachable_nodes[transition_states[i].end] = 1;
    }
    
    void buildPartitionTable() {
        for(int i = 0; i <= max_node; i++) {
            if(reachable_nodes[i] == 1 || reachable_nodes[i] == start_state)
            {
                // find current node in edges list
                vector<int> edges = find(i, transition_states, &Edge::start);
                partition_table[i] = map<char, int>();
                
                // set initial partition
                int partition = 1;
                if(find(i, final_states).size())
                    partition = 2;
                
                partition_table[i][EOF] = partition;
                
                // set partition for each end node
                // using edges list
                for(int j = 0; j < edges.size(); j++)
                {
                    partition = 1;
                    if(find(transition_states[edges[j]].end, final_states).size())
                        partition = 2;
                    
                    partition_table[i][transition_states[edges[j]].key] = partition;
                }
            }
        }
        
        max_partnr = 2;
    }
    
    void printPartitionTable() {
        cout << "\n";
        for(int i = 0; i <= max_node; i++) {
            if(reachable_nodes[i] == 1 || reachable_nodes[i] == start_state)
            {
                cout << i << " ";
                for(map<char,int>::iterator it = partition_table[i].begin(); it != partition_table[i].end(); ++it)
                    cout << partition_table[i][it->first] << " ";
                
                cout << "\n";
            }
        }
    }
    
    void minimize() {
        map< int, map<char, int> > _partition_table = partition_table;
        for(int i = 1; i <= max_partnr; i++) {
            // order partition by keys
            int max_end = 1;
            map< char, map < int, vector<int> > > end_partitions;
            for(int k = 0; k<=max_node; k++)
            {
                for(map<char,int>::iterator it = _partition_table[k].begin(); it != _partition_table[k].end(); ++it)
                {
                    int startOfLoop = 1;
                    int endOfLoop = max_partnr;
                    if(it->first != EOF)
                        startOfLoop = endOfLoop = i;
                    
                    for(int j = startOfLoop; j <= endOfLoop; j++)
                    {
                        if(max_end < j)
                            max_end = j;
                        
                        end_partitions[it->first][j] = find(j, _partition_table, it->first);
                        
                        cout << "\nend " << it->first << ": ";
                        
                        for(int ijk = 0; ijk < end_partitions[it->first][j].size(); ijk++)
                            cout << end_partitions[it->first][j][ijk] << " ";
                    }
                }
            }
            
            // find matches
            for(int j = 1; j <= max_end; j++)
            {
                set<int> matches(end_partitions[EOF][j].begin(), end_partitions[EOF][j].end());
                
                for(map< char, map < int, vector<int> > >::iterator it = end_partitions.begin(); it != end_partitions.end(); ++it)
                {
                    if(end_partitions[it->first].size() && it->first != EOF)
                    {
                        set<int> s1(end_partitions[it->first][j].begin(), end_partitions[it->first][j].end());
                        set<int> s2 = matches;
                        set<int> s3;
                        
                        set_intersection(s1.begin(), s1.end(), s2.begin(), matches.end(), inserter(s3, s3.begin()));
                        matches = s3;
                        
                        cout << "part[" << it->first << "][" << j << "]: ";
                        for (set<int>::iterator match_node = s1.begin(); match_node != s1.end(); match_node++)
                            cout << *match_node;
                    }
                }
                
                if(matches.size())
                {
                    max_partnr++;
                    for (set<int>::iterator match_node = matches.begin(); match_node != matches.end(); match_node++)
                        _partition_table[*match_node][EOF] = max_partnr;
                }
            }
        }
        
        if(_partition_table != partition_table)
        {
            partition_table = _partition_table;
            
            printPartitionTable();
            minimize();
        }
    }
};

int main(int argc, const char * argv[]) {
    ifstream *f = open_input_file("inputsz.txt");
    
    DFA dfa = *new DFA(f);
    dfa.printPartitionTable();
    dfa.minimize();
    dfa.printPartitionTable();
    
    return 0;
}
