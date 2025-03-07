// Συμπεριλαμβάνουμε τις απαραίτητες επικεφαλίδες
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <sstream>

using namespace std;

// Καθορίζουμε μια δομή δεδομένων stuct για την αποθήκευση του ζεύγους Date-Value
struct Node {
    string date;
    int value;
};

// Ορισμός της κλάσης HashTable
class HashTable {
private:
    // Ορίζουμε τον αριθμό των ομάδων κατακερματισμού/buckets στον πίνακα κατακερματισμού.
    static const int hashGroups = 11;
    // Δηλώνουμε έναν πίνακα κατακερματισμού ως πίνακα συνδεδεμένων λιστών (μέθοδος αλυσίδας για τη διαχείριση συγκρούσεων)
    list<Node*> table[hashGroups];

public:
    // Δηλώσεις συναρτήσεων μελών
    bool isEmpty() const;
    int hashFunction(string key);
    void insertNode(string date, int value);
    void deleteNode(string key);
    int getValue(string key);
    void printTable();
    void modifyValue(string key, int value);
};

// Έλεγχος αν ο πίνακας κατακερματισμού είναι άδειος
bool HashTable::isEmpty() const {
    // Επαναληπτική επανέλευση σε όλες τις ομάδες κατακερματισμού και έλεγχος αν υπάρχουν κάποιες καταχωρήσεις
    int sum = 0;
    for (int i = 0; i < hashGroups; i++) {
        sum += table[i].size();
    }
    return !sum;
}

// Συνάρτηση κατακερματισμού για τον υπολογισμό του index για ένα κλειδί
int HashTable::hashFunction(string key) {
    // Υπολογισμός του αθροίσματος των τιμών ASCII των χαρακτήρων στο κλειδί και επιστροφή του υπολοίπου όταν διαιρεθεί με τις ομάδες κατακερματισμού
    int sum = 0;
    for (int i = 0; i < key.length(); i++) {
        sum += (int)key[i];
    }
    return sum % hashGroups;
}

// Εισάγετε έναν κόμβο στον πίνακα κατακερματισμού
void HashTable::insertNode(string date, int value) {
    // Υπολογισμός του hash index
    int index = hashFunction(date);
    // Δημιουργία νέου κόμβου
    Node* newNode = new Node;
    newNode->date = date;
    newNode->value = value;
    // Εισαγωγή του νέου κόμβου στον υπολογισμένο index
    table[index].push_back(newNode);
}

// Διαγραφή ενός κόμβου από τον πίνακα κατακερματισμού
void HashTable::deleteNode(string key) {
    // Υπολογισμός του hash index
    int index = hashFunction(key);
    // Αναζήτηση για το κλειδί στην αντίστοιχη συνδεδεμένη λίστα
    list <Node*> :: iterator i;
    for (i = table[index].begin(); i != table[index].end(); i++) {
        if ((*i)->date == key)
            break;
    }
    // Εάν βρεθεί το κλειδί, διαγραφή του κόμβου και αφαίρεση του από τη συνδεδεμένη λίστα
    if (i != table[index].end()) {
        delete *i;
        table[index].erase(i);
    }
}

// Ανάκτηση μιας τιμής από τον πίνακα κατακερματισμού
int HashTable::getValue(string key) {
    // Υπολογισμός του hash index
    int index = hashFunction(key);
    // Αναζήτηση για το κλειδί στην αντίστοιχη συνδεδεμένη λίστα
    list <Node*> :: iterator i;
    for (i = table[index].begin(); i != table[index].end(); i++) {
        if ((*i)->date == key)
            break;
    }
    // Εάν βρεθεί το κλειδί, επιστροφή της αντίστοιχης τιμής
    if (i != table[index].end()) {
        return (*i)->value;
    }
    // Εάν δεν βρεθεί το κλειδί, επιστροφή -1
    return -1;
}

// Τροποποίηση μιας τιμής στον πίνακα κατακερματισμού
void HashTable::modifyValue(string key, int value) {
    // Υπολογισμός του hash index
    int index = hashFunction(key);
    // Αναζήτηση για το κλειδί στην αντίστοιχη συνδεδεμένη λίστα
    list <Node*> :: iterator i;
    for (i = table[index].begin(); i != table[index].end(); i++) {
        if ((*i)->date == key) {
            // Εάν βρεθεί το κλειδί, ενημέρωση της τιμής και επιστροφή
            (*i)->value = value;
            return;
        }
    }
    // Εάν δεν βρεθεί το κλειδί, εκτύπωση μηνύματος σφάλματος
    cout << "Το κλειδί δεν βρέθηκε.\n";
}

int main() {
    HashTable HT;

    //Load του CSV αρχειου στον πινακα κατακερματισμου.
    ifstream file("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> result;
        while(ss.good()) {
            string substr;
            getline(ss, substr, ',');
            result.push_back(substr);
        }
        string date = result[2];
        int value;
        try {
            value = stoi(result[8]);
        }
        catch (exception& e) {
            value = 0;
        }
        HT.insertNode(date, value);
    }
    file.close();

    // User interaction menu
    int option, value;
    string key;
    while (true) {
        cout << "\n1. Search Value by date.\n2. Modify Value by date.\n3. Delete entry by date.\n4. Exit.\n";
        cin >> option;
        switch (option) {
        case 1:
            cout << "Enter date: ";
            cin >> key;
            cout << "Value: " << HT.getValue(key) << "\n";
            break;
        case 2:
            cout << "Enter date: ";
            cin >> key;
            cout << "Enter new Value: ";
            cin >> value;
            HT.modifyValue(key, value);
            break;
        case 3:
            cout << "Enter date: ";
            cin >> key;
            HT.deleteNode(key);
            break;
        case 4:
            exit(0);
        default:
            cout << "Invalid option.\n";
            break;
        }
    }

    return 0;
}
