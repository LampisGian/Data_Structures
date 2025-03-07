#include <iostream> // Βιβλιοθήκη για είσοδο/έξοδο δεδομένων.
#include <fstream> // Βιβλιοθήκη για χειρισμό αρχείων.
#include <string> // Βιβλιοθήκη για χειρισμό συμβολοσειρών.
#include <sstream> // Βιβλιοθήκη για χειρισμό ροών συμβολοσειρών.
#include <vector> // Βιβλιοθήκη για χειρισμό δυναμικών πινάκων.
#include <stdexcept> // Βιβλιοθήκη για χειρισμό exceptions.

using namespace std;

// Δημιουργία struct κόμβου
struct Node {
    string date;
    int value;
    Node *left, *right;
    int height;
};

// Υπολογισμός ύψους του κόμβου.
int height(Node* N) {
    if (N == NULL)
        return 0;
    return N->height;
}

// Επιστρέφει το μεγαλύτερο από δύο ακέραια.
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Δημιουργία νέου κόμβου.
Node* createNode(string date, double value) {
    Node* node = new Node();
    node->date = date;
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
Node* insertNode(Node* node, string date, double value) {
    if (node == NULL)
        return(createNode(date, value));

    if (date < node->date)
        node->left = insertNode(node->left, date, value);
    else if (date > node->date)
        node->right = insertNode(node->right, date, value);
    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    if (balance > 1 && date < node->left->date)
        return rightRotate(node);
    if (balance < -1 && date > node->right->date)
        return leftRotate(node);
    if (balance > 1 && date > node->left->date) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && date < node->right->date) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Επιστρέφει τον κόμβο με την ελάχιστη τιμή.
Node* minValueNode(Node* node) {
    Node* current = node;
    while (current->left != NULL)
        current = current->left;

    return current;
}

// Διαγράφη κόμβου.
Node* deleteNode(Node* root, string date) {
    if (root == NULL)
        return root;

    if (date < root->date)
        root->left = deleteNode(root->left, date);
    else if(date > root->date)
        root->right = deleteNode(root->right, date);
    else {
        if((root->left == NULL) || (root->right == NULL)) {
            Node *temp = root->left ? root->left : root->right;
            if(temp == NULL) {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;
            delete temp;
        }
        else {
            Node* temp = minValueNode(root->right);
            root->date = temp->date;
            root->value = temp->value;
            root->right = deleteNode(root->right, temp->date);
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Αναζήτηση κόμβου με βάση την ημερομηνία.
Node* search(Node* root, string date) {
    if(root == NULL || root->date == date)
        return root;

    if(root->date < date)
        return search(root->right, date);

    return search(root->left, date);
}

// Εμφανίζει το AVL Tree με in-order traversal.
void inorder(Node *root) {
    if(root != NULL) {
        inorder(root->left);
        cout << root->date << "\t" << root->value << "\n";
        inorder(root->right);
    }
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

int main() {
    Node *root = NULL;
    string date;
    int value;
    string line;
    Node *res;
    Node *mod;

    ifstream file("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");
    if (file.is_open()) {
        while (getline(file, line)) {
            vector<string> data = split(line, ',');
            date = data[2];

            // Μετατροπή ημερομηνίας από DD/MM/YYYY σε YYYY-MM-DD για σύγκριση
            if(date.size() == 10) //αποφύγη σφάλματων εκτός ορίων
                date = date.substr(6,4) + "-" + date.substr(3,2) + "-" + date.substr(0,2);

            try { //Try,Catch για δεδομενα του Column "Value" που εμφανιζαν exception error invalid argument.
                value = stoi(data[8]);
            } catch(const std::invalid_argument& ia) {
                value = 0;
            }

            root = insertNode(root, date, value);
        }
        file.close();
    } else {
        cout << "Unable to open file\n";
        return 1;
    }

    int option; //Λειτουργειες.
    do {
        cout << "\n1. Display the BST in an in-order traversal.\n";
        cout << "2. Search for the value of a given date.\n";
        cout << "3. Modify the value of a specific date.\n";
        cout << "4. Delete a record corresponding to a specific date.\n";
        cout << "5. Exit the application.\n";
        cout << "\nPlease choose an option: ";
        cin >> option;
        switch(option) {
            case 1:
                inorder(root);
                break;
            case 2:
                cout << "\nEnter the date (format YYYY-MM-DD): ";
                cin >> date;
                res = search(root, date);
                if(res == NULL)
                    cout << "Record not found.\n";
                else
                    cout << "\nThe value of " << date << " is: " << res->value << ".\n";
                break;
            case 3:
                cout << "\nEnter the date (format YYYY-MM-DD): ";
                cin >> date;
                cout << "\nEnter the new value: ";
                cin >> value;
                mod = search(root, date);
                if(mod == NULL)
                    cout << "Record not found. No modifications were made.\n";
                else {
                    mod->value = value;
                    cout << "Record updated successfully!\n";
                }
                break;
            case 4:
                cout << "\nEnter the date (format YYYY-MM-DD): ";
                cin >> date;
                root = deleteNode(root, date);
                cout << "Record deleted successfully!\n";
                break;
            case 5:
                cout << "Exiting the application...\n";
                break;
            default:
                cout << "Invalid option! Please choose a valid one.\n";
        }
    } while(option != 5);

    return 0;
}
