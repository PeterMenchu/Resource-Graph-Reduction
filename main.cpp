#include <iostream>
#include "GraphReduction.h"

using namespace std;
int main() {
    // bool to check if input file failed to open
    bool exit;
    cout << "*------------------------------------------*\n";
    cout << "Ensure input file is in project folder.\n";
    GraphReduction Project2;
    // get data
    exit = Project2.Read_and_Init();
    // if input file didn't open, exit = 1
    if ( exit == 0) {
        Project2.Reduce();// perform actual reduction
    }
    return 0;
}
