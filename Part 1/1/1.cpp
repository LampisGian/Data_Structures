#include <iostream>     // για τις τυπικές λειτουργίες εισόδου/εξόδου
#include <fstream>      // για τις λειτουργίες εισόδου/εξόδου αρχείων
#include <sstream>      // για τις λειτουργίες ροής συμβολοσειρών
#include <vector>       // για τη χρήση της δομής δεδομένων διανύσματος
#include <chrono>       // για τη μέτρηση του χρόνου
#include <algorithm>
using namespace std;

struct Data {
    string date;       // πεδίο για την αποθήκευση της ημερομηνίας
    int value;         // πεδίο για την αποθήκευση της τιμής
};


bool isInteger(const string &s) {
    return !s.empty() && find_if(s.begin(), s.end(), [](unsigned char c) { return !isdigit(c); }) == s.end();
}

vector<Data> read_csv(string filename) {
    vector<Data> dataList;
    ifstream file(filename);

    string line, word;
    string header;
    getline(file, header);  // παράλειψη της σειράς επικεφαλίδας

    while (getline(file, line)) {
        vector<string> row;
        stringstream s(line);
        while (getline(s, word, ',')) {
            row.push_back(word);  // προσθήκη κάθε κελιού δεδομένων στο διάνυσμα γραμμής
        }

        Data data;
        data.date = row[2];  // ανάθεση της ημερομηνίας από την τρίτη στήλη

        // Έλεγχος αν η συμβολοσειρά μπορεί να μετατραπεί σε ακέραιο
        if (isInteger(row[8])) {
            data.value = stoi(row[8]);  // μετατροπή συμβολοσειράς σε int και ανάθεση της τιμής από την ένατη στήλη
        } else {
            data.value = 0;  // ανάθεση μιας προεπιλεγμένης τιμής, ή χειρισμός του σφάλματος όπως εσείς θεωρείτε κατάλληλο
        }

        dataList.push_back(data);  // προσθήκη δεδομένων στη λίστα δεδομένων
    }

    file.close();
    return dataList;
}

// Συνάρτηση συγχώνευσης που χρησιμοποιείται στην mergesort
void merge(vector<Data>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    vector<Data> L(n1), R(n2);

    for(int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for(int j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    int i = 0;
    int j = 0;
    int k = l;

    // Συγχώνευση δύο υποπινάκων στον arr
    while (i < n1 && j < n2) {
        if (L[i].value <= R[j].value) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Αντιγραφή των υπόλοιπων στοιχείων του L[], εάν υπάρχουν
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Αντιγραφή των υπόλοιπων στοιχείων του R[], εάν υπάρχουν
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Συνάρτηση mergeSort
void mergeSort(vector<Data>& arr, int l, int r) {
    if (l < r) {
        int m = l+(r-l)/2;
        mergeSort(arr, l, m);     // ταξινόμηση πρώτου μισού
        mergeSort(arr, m+1, r);   // ταξινόμηση δεύτερου μισού

        merge(arr, l, m, r);      // συγχώνευση δύο μισών
    }
}

// Συνάρτηση counting sort
void countingSort(vector<Data>& arr) {
    int max = arr[0].value;
    for(int i = 1; i < arr.size(); i++) {
        if(arr[i].value > max) max = arr[i].value;
    }

    vector<int> count(max+1, 0);
    for(int i = 0; i < arr.size(); i++) {
        count[arr[i].value]++;   // μετρά την εμφάνιση κάθε τιμής
    }

    vector<Data> sortedArr(arr.size());
    for(int i = 1; i <= max; i++) {
        count[i] += count[i-1];  // υπολογίζει τον προθέματος πίνακα άθροισης
    }

    // κατασκευάζει τον ταξινομημένο πίνακα
    for(int i = arr.size()-1; i >= 0; i--) {
        sortedArr[count[arr[i].value]-1] = arr[i];
        count[arr[i].value]--;
    }

    arr = sortedArr;  // αντιγράφει τον ταξινομημένο πίνακα στον αρχικό πίνακα
}


int main()
{

    // read data απο το CSV αρχειο
    vector<Data> dataList = read_csv("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");

    //Copy του original data για later use
    vector<Data> dataListCopy = dataList;

    // μετρηση χρονου για merge sort
    auto start = chrono::high_resolution_clock::now();

    mergeSort(dataList, 0, dataList.size() - 1);

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout << "Time taken by merge sort: " << duration.count() << " microseconds" << endl;

    // Print του Sorted Data,ωστοσο εχει γινει "comment out" για να μην υπαρχει cluttering στο Console.

    /*
    cout << "Data after merge sort:" << endl;
    for (const Data& data : dataList) {
        cout << "Date: " << data.date << ", Value: " << data.value << endl;
    }
    */

    // Reset dataList στο αρχικο state πριν το Counting Sort
    dataList = dataListCopy;

    // μετρηση χρονου για counting sort
    start = chrono::high_resolution_clock::now();

    countingSort(dataList);

    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout << "Time taken by counting sort: " << duration.count() << " microseconds" << endl;

    // Print του Sorted Data,ωστοσο εχει γινει "comment out" για να μην υπαρχει cluttering στο Console.

    /*
    cout << "Data after counting sort:" << endl;
    for (const Data& data : dataList) {
        cout << "Date: " << data.date << ", Value: " << data.value << endl;
    }
    */


    return 0;
}
