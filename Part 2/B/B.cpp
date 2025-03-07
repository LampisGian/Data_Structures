#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <stdexcept>

using namespace std;

// Δημιουργία struct κόμβου
struct Node {
    vector<string> dates;
    int value;
    Node *left, *right;
    int height;
};
// Υπολογισμός ύψους του κόμβου.
int height(Node *N) {
    if (N == NULL)
        return 0;
    return N->height;
}

// Δημιουργία νέου κόμβου.
Node* createNode(string date, int value) {
    Node* node = new Node();
    node->dates.push_back(date);
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return(node);
}
// Δεξιά περιστροφή γύρω από τον κόμβο y.
Node *rightRotate(Node *y) {
    Node *x = y->left;
    Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}
// Αριστερή περιστροφή γύρω από τον κόμβο x.
Node *leftRotate(Node *x) {
    Node *y = x->right;
    Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}
// Επιστρέφει την τιμή της ισορροπίας ενός κόμβου.
int getBalance(Node *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}
// Εισαγωγή κόμβου στην AVL Tree.
Node* insertNode(Node* node, string date, int value) {
    if (node == NULL)
        return(createNode(date, value));

    if (value < node->value)
        node->left = insertNode(node->left, date, value);
    else if (value > node->value)
        node->right = insertNode(node->right, date, value);
    else
        node->dates.push_back(date);

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    if (balance > 1 && value < node->left->value)
        return rightRotate(node);
    if (balance < -1 && value > node->right->value)
        return leftRotate(node);
    if (balance > 1 && value > node->left->value) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && value < node->right->value) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}
//ανεύρεση των ημερομηνιών με την ελάχιστη τιμή
void findMinValueDates(Node* node, int minValue) {
    if (node == NULL)
        return;

    if (node->value == minValue)
        for(const auto& date: node->dates)
            cout << "Date: " << date << ", Value: " << minValue << "\n";

    findMinValueDates(node->left, minValue);
    findMinValueDates(node->right, minValue);
}

//ανεύρεση των ημερομηνιών με την μέγιστη τιμή
void findMaxValueDates(Node* node, int maxValue) {
    if (node == NULL)
        return;

    if (node->value == maxValue)
        for(const auto& date: node->dates)
            cout << "Date: " << date << ", Value: " << maxValue << "\n";

    findMaxValueDates(node->left, maxValue);
    findMaxValueDates(node->right, maxValue);
}
//εύρεση της ελάχιστης τιμής στο δέντρο
int findMinValue(Node* node) {
    if (node == NULL)
        return numeric_limits<int>::max();

    int minValue = node->value;
    if (minValue == 0)
        minValue = numeric_limits<int>::max();

    int leftMin = findMinValue(node->left);
    int rightMin = findMinValue(node->right);

    return min(min(minValue, leftMin), rightMin);
}
//εύρεση της μέγιστης τιμής στο δέντρο
int findMaxValue(Node* node) {
    Node* current = node;
    while (current->right != NULL)
        current = current->right;
    return current->value;
}

// Σπάει ένα string σε ένα vector με βάση ένα delimiter.
vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}


// Εμφανίζει το AVL Tree με in-order traversal.
void printInorder(Node* node) {
    if (node == NULL)
        return;
    printInorder(node->left);
    for (const string& date : node->dates) {
        cout << "Date: " << date << ", Value: " << node->value << "\n";
    }
    printInorder(node->right);
}

int main() {
    Node *root = NULL;
    string date;
    int value;
    string line;
    bool firstLine = true;

    ifstream file("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");
    if (file.is_open()) {
        while (getline(file, line)) {
            if(firstLine) {
                firstLine = false;
                continue;
            }
            vector<string> data = split(line, ',');
            date = data[2];

            if(date.size() == 10) //αποφύγη σφάλματων εκτός ορίων
                date = date.substr(6,4) + "-" + date.substr(3,2) + "-" + date.substr(0,2);

            try { //Try,Catch για δεδομενα του Column "Value" που εμφανιζαν exception error invalid argument.
                value = stoi(data[8]);
            } catch(const std::invalid_argument& ia) {
                value = 0; // If there's an issue with the data, set it to 0
            }

            root = insertNode(root, date, value);
        }
        file.close();
    } else {
        cout << "Unable to open file\n";
        return 1;
    }

    int option = 0;
    do {
        cout << "\nPlease select an option:\n"
             << "1. Print BST in-order\n"
             << "2. Find days with the minimum value (Ignoring Values which are Zero)\n"
             << "3. Find days with the maximum value\n"
             << "4. Exit\n";
        cin >> option;

        switch(option) {
            case 1:
                printInorder(root);
                break;
            case 2:
                {
                    int minValue = findMinValue(root);
                    cout << "\nDates with the MINIMUM value (" << minValue << "):\n";
                    findMinValueDates(root, minValue);
                }
                break;
            case 3:
                {
                    int maxValue = findMaxValue(root);
                    cout << "\nDates with the MAXIMUM value (" << maxValue << "):\n";
                    findMaxValueDates(root, maxValue);
                }
                break;
            case 4:
                break;
            default:
                cout << "\nInvalid option, please try again.\n";
                break;
        }
    } while(option != 4);

    return 0;
}
