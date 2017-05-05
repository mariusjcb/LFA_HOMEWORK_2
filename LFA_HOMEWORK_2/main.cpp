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
    else cout << "File from parameters not found.\n";
    
    #ifdef fileno
        if (isatty(fileno(stdin)))
        {
            cout << "Change file name or...\n\nInsert file using '<your_file' in tty mode" << endl;
            exit(0);
        } else cout << "I am in tty mode...\n";
    #endif
    
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

vector<Edge> readTransitionStates(int& max_node, int& min_node, ifstream *file = NULL) {
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
            
            if(currentEdge.start < min_node)
                min_node = currentEdge.start;
            
            if(currentEdge.end < min_node)
                min_node = currentEdge.end;
            
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

//MARK: Int to String for Windows
#if defined (WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <sstream>
string to_string(int a)
{
    stringstream ss;
    ss << a;
    return ss.str();
}
#endif

//MARK: - DATA USAGE
void print(vector<int> read) {
    for(vector<int>::iterator it = read.begin(); it != read.end(); it++)
        cout << *it << " ";
}

void print(vector<Edge> read) {
    for(vector<Edge>::iterator it = read.begin(); it != read.end(); it++)
        cout << it->start << " " << it->key << it->end << "\n";
}

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
    int min_node;
    int* reachable_nodes;
    int max_partnr = 0;
    
    //MARK: - Singleton
    DFA() {};
    DFA(DFA const&)             =  delete;
    void operator=(DFA const&)  =  delete;
    
public:
    static DFA& Entity()
    {
        static DFA instance;
        return instance;
    }
    
    //MARK: - Implementation
    void setStartState(int value) { start_state = value; }
    void setFinalStates(vector<int> value) { final_states = value; }
    void setTransitionStates(vector<Edge> value) { transition_states = value; }
    void setMaxNode(int value) { max_node = value; }
    void setMinNode(int value) { min_node = value; }
    
    //MARK: - Initialization
    void init(ifstream *f = NULL) {
        setStartState(readStartState(f));
        setFinalStates(readFinalStates(f));
        
        int max_node = 0;
        int min_node = 0;
        setTransitionStates(readTransitionStates(max_node, min_node, f));
        setMaxNode(max_node);
        setMinNode(min_node);
        
        removeUnreachableNodes();
        buildPartitionTable();
    }
    
    void removeUnreachableNodes() {
        reachable_nodes = new int[max_node+1]();
        
        for(int i = 0; i < transition_states.size(); i++)
            reachable_nodes[transition_states[i].end] = 1;
    }
    
    //MARK: - Partition Usage
    void buildPartitionTable() {
        for(int i = min_node; i <= max_node; i++) {
            if(reachable_nodes[i] != 1 && reachable_nodes[i] != start_state)
                continue;
            
            //MARK: find current node in edges list
            vector<int> edges = find(i, transition_states, &Edge::start);
            partition_table[i] = map<char, int>();
            
            //MARK: set initial partition
            int partition = 1;
            if(find(i, final_states).size())
                partition = 2;
            
            partition_table[i][EOF] = partition;
        }
        
        updatePartitionTable();
        max_partnr = 2;
    }
    
    void updatePartitionTable() {
        if(!partition_table.size()) return;
        
        for(int i = min_node; i <= max_node; i++) {
            if(reachable_nodes[i] != 1 && reachable_nodes[i] != start_state)
                continue;
            
            //MARK: find current node in edges list
            vector<int> edges = find(i, transition_states, &Edge::start);
            
            for(int j = 0; j < edges.size(); j++)
                partition_table[i][transition_states[edges[j]].key] = partition_table[transition_states[edges[j]].end][EOF];
        }
    }
    
    string getPartitionStates(int state, bool pushPartitionIntoIgnoreVector=false, vector<int>& ignored = *new vector<int>()) {
        string newNode = string();
        
        int partition = partition_table[state][EOF];
        if(pushPartitionIntoIgnoreVector)
            ignored.push_back(partition);
        
        for (auto& searchLine : partition_table)
            if (searchLine.second[EOF] == partition)
                newNode.append(to_string(searchLine.first));
        
        return newNode;
    }
    
    //MARK: - Print functions
    void printPartitionTable() {
        if(!partition_table.size()) return;
        
        cout << "\n";
        for(int i = min_node; i <= max_node; i++) {
            if(reachable_nodes[i] != 1 && reachable_nodes[i] != start_state)
                continue;
            
            cout << i << " ";
            for(map<char,int>::iterator it = partition_table[i].begin(); it != partition_table[i].end(); ++it)
                cout << partition_table[i][it->first] << " ";
            
            cout << "\n";
        }
    }
    
    void printStartState(bool usingPartitionTable=false) {
        if(!partition_table.size() || !usingPartitionTable)
        {
            cout << start_state;
            return;
        }
        
        int partition = partition_table[start_state][EOF];
        for (auto& searchLine : partition_table)
            if (searchLine.second[EOF] == partition)
                cout << searchLine.first;
        
        cout << "\n";
    }
    
    void printFinalStates(bool usingPartitionTable=false) {
        if(!partition_table.size() || !usingPartitionTable)
        {
            ::print(final_states);
            return;
        }
        
        vector<int> ignored_partitions;
        for (auto& final_state : final_states)
        {
            if(find(partition_table[final_state][EOF], ignored_partitions).size())
                continue;
            
            cout << getPartitionStates(final_state, true, ignored_partitions) << " ";
        }
        
        cout << "\n";
    }
    
    void printEdges(bool usingPartitionTable=false) {
        if(!partition_table.size() || !usingPartitionTable)
        {
            ::print(transition_states);
            return;
        }
        
        map<string, map<char, bool> > used_state;
        for (auto& transition_state : transition_states)
        {
            string partition = getPartitionStates(transition_state.start);
            if(transition_state.key == EOF || used_state[partition][transition_state.key] == true)
                continue;
            
            used_state[partition][transition_state.key] = true;
            cout << partition << " "<< transition_state.key << " " << getPartitionStates(transition_state.end) << "\n";
        }
        
        cout << "\n";
    }
    
    void print() {
        bool usingPartitionTable = max_partnr ? true : false;
        cout << "\n";
        
        printStartState(usingPartitionTable);
        printFinalStates(usingPartitionTable);
        printEdges(usingPartitionTable);
    }
    
    //MARK: - DFA Minimization Algorithm
    void minimize() {
        if(!partition_table.size()) return;
        
        vector<int> ignored_partitions;
        int lastCode = max_partnr;
        int newPartitionCode = max_partnr;
        
        for (auto& searchLine : partition_table) {
            if(find(searchLine.second[EOF], ignored_partitions).size() != 0)
                continue;
            
            for (map<int, map<char, int> >::iterator cline = partition_table.begin(); cline != partition_table.end(); cline++) {
                if(cline->first == searchLine.first) continue;
                if(cline->second[EOF] != searchLine.second[EOF]) continue;
                
                int ok = 1;
                for(auto& searchChar : searchLine.second) {
                    if(cline->second.find(searchChar.first) == cline->second.end()) { ok = 0; break; }
                    if(cline->second[searchChar.first] != searchChar.second) { ok = 0; break; }
                }
                
                if(ok == 0)
                {
                    if(newPartitionCode == lastCode) newPartitionCode++;
                    cline->second[EOF] = newPartitionCode;
                }
            }
            
            if(newPartitionCode != lastCode) {
                ignored_partitions.push_back(newPartitionCode);
                lastCode = newPartitionCode;
            }
        }
        
        if(lastCode != max_partnr) {
            max_partnr = lastCode;
            updatePartitionTable();
            
            minimize();
        } else updatePartitionTable();
    }
};

int main(int argc, const char * argv[]) {
    ifstream *f = open_input_file("inputsz.txt");
    
    DFA::Entity().init(f);
    DFA::Entity().minimize();
    
    cout << "\nOutput: ";
    DFA::Entity().print();
    
    return 0;
}
