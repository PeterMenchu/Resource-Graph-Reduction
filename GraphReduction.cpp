//
// Created by Peter Menchu on 4/26/20.
//

#include "GraphReduction.h"

GraphReduction::GraphReduction(){
    num_processes = 0;
    // -1 used as a check to end the first while loop, since theoretically 0 is valid.
    // See below in Read_and_Init why I do this.
    num_resources = -1;
    // allocated is a flag to indicate if program exited before data is allocated for
    // the graphs, since they are deleted from the destructor.
    allocated = 0;
}
// Function that handles the actual reduction
void GraphReduction::Reduce() {
    int remainingProcs = num_processes;// decrement as procs are removed
    int blocked = 0;// counts waiting/blocked procs
    int j = 0;// iterator that keeps track of what proc is being evaluated
    bool full = 0;// flag that indicates if a resource has/has no free units

    // Start
    // If procs or resources are not present, no deadlocks.
    if (num_processes == 0 || num_resources == 0) {
        cout << "Either num_processes or num_resources is set to 0, no deadlocks.\n";
    } else {
        // loop to iterate through processes
        do {
            if (procGraph[j][0] > -1) { // -1 here indicates removed
                // check for requests
                for (int i = 0; i < num_resources; i++) {
                    // if requests, check if there are free units from that resource
                    if (procGraph[j][i] >= 1) {
                        // if no free units, mark as full to check next process
                        if (currAlloc[i] == maxAllocs[i]) {
                            full = 1;
                        }
                    }
                }

                // check if process is removable, if so remove and deallocate.
                // Note, this is only the case if requested resources have free units
                // or if there are no requests.
                if (full == 0) {
                    // indicate a proc has been removed from total
                    remainingProcs--;
                    // reset blocked since removal of allocations may mean other processes are unblocked
                    blocked = 0;
                    // indicate specific removed proc as a flag for the loop
                    procGraph[j][0] = -1;
                    // loop through resources
                    for (int i = 0; i < num_resources; i++) {
                        // check if any resources are allocated to this process
                        if (resGraph[i][j] >= 1) {
                            // remove all allocations to removed process
                            while(resGraph[i][j] != 0) {
                                // looping like this in case multiple units are allocated to this proc
                                resGraph[i][j]--;
                                // reduce total allocation for the found resource
                                currAlloc[i]--;
                            }
                        }
                    }
                } else { // if not removable
                    // each time a waiting proc is found, increment. Note this is reset when a proc is removed
                    // since processes need to be rechecked.
                    blocked++;
                }
            }
            // to make sure the loop doesn't go out of bounds, check for wraparound to first process
            if (j == num_processes - 1) {
                j = 0;
            } else { // else just increment
                j++;
            }
            // reset full for next check
            full = 0;
            // loop will exit if blocked is the same as # of remaining procs, or if there are no procs left
        } while (blocked != remainingProcs && remainingProcs != 0);
    }
    // if all procs removed, no deadlock
    if (remainingProcs == 0){
        cout << "\nThis graph does not contain a deadlock!\n\n";
    } else { // if not, there is a deadlock
        cout << "\nThis graph contains a deadlock.\n\n";
    }
    return;
}

bool GraphReduction::Read_and_Init() {
    string filename;
    ifstream fin;// file stream
    string buffer;// stores temporary input
    string numBuffer;// holds data that gets converted to an integer
    int graphCount = 0;// counts if all procs have been read in, move to resources if so
    bool foundAllocs = 0;// checks if # units have been found
    int j = 0;// iterator
    int k = 0;// another iterator
    cout << "Enter name of input file: ";
    cin >> filename;
    fin.open(filename);
    if (fin.fail())// checks if the file failed to open
    {
        cout << "Error: file could not be opened\n";
        return 1; // return 1 to exit main
    }
    // read data
    while(!fin.eof() && num_resources == -1) {
        getline(fin, buffer);
        // check for comments or blank line
        if (buffer[0] != '%' && buffer[0] != '\n') {
            // get # processes
            if (buffer[4] == 'p' && buffer.length() > 3) {
                j = 0; // track characters in case multi digit input is given
                // NOTE: I do this for all integers, just in case...
                for (int i = 14; i < buffer.length(); i++) {
                    if (isdigit(buffer[i])) {
                        numBuffer[j] = buffer[i];// concatenate all digits
                        j++;
                    }
                }
                // convert into one whole integer
                num_processes = stoi(numBuffer, nullptr);
                cout << "Number of processes: " << num_processes << endl;
                numBuffer.clear();
            } else if (buffer[4] == 'r' && buffer.length() > 3) {
                // same approach as before
                j = 0;
                for (int i = 14; i < buffer.length(); i++) {
                    numBuffer[j] = buffer[i];
                    j++;
                }
                num_resources = stoi(numBuffer, nullptr);
                cout << "Number of resources: " << num_resources << endl;
                numBuffer.clear();
            }

            buffer.clear();
        }
    }
    // now that # of procs and resources are found, allocate
    allocated = 1; // mark that allocations are occurring and nothing went wrong before this
    // stores max allocations
    maxAllocs = new int [num_resources];
    // graph to hold request edges
    procGraph = new int*[num_processes];
    for(int i = 0; i < num_processes; i++){
        procGraph[i] = new int[num_resources];
    }
    // graph to hold allocation edges
    resGraph = new int*[num_resources];
    for(int i = 0; i < num_resources; i++){
        resGraph[i] = new int[num_processes];
    }
    // holds units in use
    currAlloc = new int [num_resources];
    for(int i = 0; i < num_resources; i++){
        currAlloc[i] = 0;
    }
    // continue reading and setting data
    while(!fin.eof()) {
        // similar to before
        getline(fin, buffer);
        if (buffer[0] != '%' && buffer[0] != '\n') {
            // find max units
            if (isdigit(buffer[0]) && foundAllocs == 0) {
                j = 0;
                cout << "Max units: ";
                // handled exactly as before, check if multi digit units are used
                for (int i = 0; i < buffer.length(); i++) {
                    if (isdigit(buffer[i])) {
                        numBuffer.push_back(buffer[i]);

                    } else { // if comma is found
                        maxAllocs[j] = stoi(numBuffer, nullptr);

                        j++;
                        numBuffer.clear();
                        cout << maxAllocs[j - 1] << " ";
                    }
                }
                // if last value didn't get added. depends on text editor used to create input it seems.
                if (j < num_resources) {
                    maxAllocs[j] = stoi(numBuffer, nullptr);

                    j++;
                    numBuffer.clear();
                    cout << maxAllocs[j - 1] << " ";
                }
                cout << endl;
                foundAllocs = 1;

            } else if (isdigit(buffer[0])) {
                // read request data for procGraph
                if (graphCount < num_processes) { // getting info about resource requests
                    j = 0;
                    k = 0;
                    // again, checking in case there are multi-digit requests
                    for (int i = 0; i < buffer.length(); i++) {
                        if (isdigit(buffer[i])) {
                            numBuffer.push_back(buffer[i]);

                        } else {
                            if (k < num_processes) {
                                k++; // iterate to get past proc to proc area of graph
                                numBuffer.clear();
                            } else {
                                procGraph[graphCount][j] = stoi(numBuffer, nullptr);

                                j++;
                                numBuffer.clear();
                                //cout << procGraph[graphCount][j - 1] << " ";
                            }
                        }
                    }
                    // making sure the last value got added, depends on text editor/IDE used.
                    if (j < num_processes) {
                        procGraph[graphCount][j] = stoi(numBuffer, nullptr);

                        j++;
                        numBuffer.clear();
                        //cout << procGraph[graphCount][j - 1] << " ";
                    }
                    //cout << endl;
                    graphCount++;
                    j = 0;
                } else {
                    k = 0;
                    // check for allocation edges similar as before
                    for (int i = 0; i < buffer.length(); i++) {
                        if (isdigit(buffer[i]) && k < num_processes) {
                            numBuffer.push_back(buffer[i]);

                        } else if (k < num_processes) {

                            resGraph[j][k] = stoi(numBuffer, nullptr);
                            if (resGraph[j][k] > 0) {
                                currAlloc[j] += resGraph[j][k];
                            }
                            k++;
                            numBuffer.clear();
                            //cout << resGraph[j][k - 1] << " ";
                        }
                    }
                    //cout << endl;
                    j++;
                }
            }
            buffer.clear();
        }
    }
    fin.close(); // close file
    
    return 0;// return 0 if input file opened properly and such
}

GraphReduction::~GraphReduction() {
    // if allocations occurred, delete allocations
    if (allocated > 0) {
        delete[] maxAllocs;
        for (int i = 0; i < num_processes; i++) {
            delete[] procGraph[i];
        }
        delete[] procGraph;

        for (int i = 0; i < num_resources; i++) {
            delete[] resGraph[i];
        }
        delete[] resGraph;
        delete [] currAlloc;
    }
}


