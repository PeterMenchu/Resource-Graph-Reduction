//
// Created by Peter Menchu on 4/26/20.
//

#ifndef OSPROJ2_GRAPHREDUCTION_H
#define OSPROJ2_GRAPHREDUCTION_H

#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
using namespace std;

class GraphReduction {
private:
    // holds request edges
    int **procGraph;
    // holds allocation edges
    int **resGraph;
    // the following replicates the variables in input
    int num_processes;
    int num_resources;
    // stores max units per resource
    int *maxAllocs;
    // stores units in use per resource
    int *currAlloc;
    // flag to indicate nothing went wrong before allocation.
    // indicates that the dynamic data needs to be deleted if equal to 1
    int allocated;
public:
    // constructor
    GraphReduction();
    // read in input and generate the matrices my algorithm uses
    bool Read_and_Init();
    // function to perform the actual reduction
    void Reduce();
    // destructor, deletes allocations
    ~GraphReduction();
};


#endif //OSPROJ2_GRAPHREDUCTION_H
