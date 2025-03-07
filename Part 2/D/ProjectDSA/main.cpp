#include "ProgramA.h"
#include "ProgramB.h"
#include "ProgramC.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    string choice;
    string subchoice;

    cout << "Please select data structure: \n1. BST \n2. Hashtable\n";
    getline(cin, choice);

    if (choice == "1") {
        cout << "You chose BST. Please choose loading method: \n1. By Date \n2. By Value\n";
        getline(cin, subchoice);

        if (subchoice == "1") {
            cout << "You chose 'By Date'. Loading data...\n";
            ProgramA::runProgramA();
        }
        else if (subchoice == "2") {
            cout << "You chose 'By Value'. Loading data...\n";
            ProgramB::runProgramB();
        }
        else {
            cout << "Invalid input. Exiting...\n";
        }
    }
    else if (choice == "2") {
        cout << "You chose Hashtable. Loading data...\n";
        ProgramC::runProgramC();
    }
    else {
        cout << "Invalid input. Exiting...\n";
    }

    return 0;
}
