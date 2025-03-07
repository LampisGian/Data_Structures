#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <algorithm>
using namespace std;

struct Data {
    string date;  // Ημερομηνία ως string
    long long cumulative;  // cumulative τιμή ως long long
};

// function για να ελέγξει εάν μια συγκεκριμένη συμβολοσειρά αντιπροσωπεύει έναν ακέραιο αριθμό
bool isInteger(const string &s) {
    return !s.empty() && find_if(s.begin(), s.end(), [](unsigned char c) { return !isdigit(c); }) == s.end();
}

// function για την ανάγνωση CSV αρχείου και την αποθήκευση τιμών σε ένα vector του Data
vector<Data> read_csv(string filename) {
    vector<Data> dataList;
    ifstream file(filename);

    string line, word;
    string header;
    getline(file, header);  // παραλείψη της σειράς επικεφαλίδας

    // Βρόγχος για κάθε γραμμή στο αρχείο
    while (getline(file, line)) {
        vector<string> row;
        stringstream s(line);

        // Βρόγχος για κάθε λέξη στη γραμμή
        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        Data data;
        data.date = row[2];  // Αποθήκευση ημερομηνίας

        // Ελέγχος εάν η συμβολοσειρά μπορεί να μετατραπεί σε long long
        if (isInteger(row[9])) {
            data.cumulative = stoll(row[9]);  // Αποθήκευση συσσωρευτικής
        } else {
            data.cumulative = 0; //αλλιως θετουμε τιμη 0.
        }

        dataList.push_back(data);
    }

    file.close();
    return dataList;
}

// function για τη δημιουργία μιας heapify δομής από τον πίνακα
void heapify(vector<Data>& arr, int n, int i) {
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    // Έλεγχος αν το αριστερό παιδί είναι μεγαλύτερο
    if (left < n && arr[left].cumulative > arr[largest].cumulative)
        largest = left;

    // Έλεγχος αν το δεξί παιδί είναι μεγαλύτερο
    if (right < n && arr[right].cumulative > arr[largest].cumulative)
        largest = right;

    // Εάν το μεγαλύτερο δεν είναι η ρίζα
    if (largest != i) {
        swap(arr[i], arr[largest]);

        // Αναδρομικά heapify το επηρεαζόμενο υποδέντρο
        heapify(arr, n, largest);
    }
}

// Λειτουργία για την εκτέλεση ταξινόμησης σωρού στον πίνακα
void heapSort(vector<Data>& arr) {
    int n = arr.size();

    // Δημιουργία σωρού
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Εξαγωγή στοιχείων από τον σωρό ένα ένα
    for (int i=n-1; i>=0; i--) {
        // Μετακίνηση της τρέχουσας ρίζας στο τέλος
        swap(arr[0], arr[i]);

        //  heapify στον μειωμένο σωρό
        heapify(arr, i, 0);
    }
}

// function για partition του πίνακα για quicksort
int partition(vector<Data>& arr, int low, int high) {
    int pivot = arr[high].cumulative;  // Άξονας
    int i = (low - 1);  // Δείκτης του μικρότερου στοιχείου


    for (int j = low; j <= high- 1; j++) {
        if (arr[j].cumulative < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

// function για την εκτέλεση quicksort στον πίνακα
void quickSortIterative(vector<Data>& arr, int low, int high) {
    // Δημιουργία ενός επιπλέον στοίβας
    vector<int> stack(high - low + 1);

    // Αρχικοποίηση κορυφής στοίβας
    int top = -1;

    // Προώθηση αρχικών τιμών χαμηλής και υψηλής στη στοίβα
    stack[++top] = low;
    stack[++top] = high;


    while (top >= 0) {
        // Pop high και low
        high = stack[top--];
        low = stack[top--];

        int p = partition(arr, low, high);

        // Εάν υπάρχουν στοιχεία στην αριστερή πλευρά του pivot, τότε προώθηση της αριστερής πλευράς στη στοίβα
        if (p - 1 > low) {
            stack[++top] = low;
            stack[++top] = p - 1;
        }

        // Εάν υπάρχουν στοιχεία στη δεξιά πλευρά του pivot, τότε προώθηση της δεξιάς πλευράς στη στοίβα
        if (p + 1 < high) {
            stack[++top] = p + 1;
            stack[++top] = high;
        }
    }
}

int main() {
    // Ανάγνωση δεδομένων από αρχείο CSV
    vector<Data> dataList = read_csv("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");

    //Copy του original data για later use
    vector<Data> dataListCopy = dataList;

    // Έναρξη χρονοδιακόπτη
    auto start = chrono::high_resolution_clock::now();

    // Εκτέλεση ταξινόμησης σωρού
    heapSort(dataList);

    // Διακοπή χρονοδιακόπτη και υπολογισμός διάρκειας
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Εκτύπωση χρόνου που απαιτήθηκε για την heapsort
    cout << "Time taken by heap sort: "
         << duration.count() << " microseconds" << endl;

    // Print του Sorted Data,ωστοσο εχει γινει "comment out" για να μην υπαρχει cluttering στο Console.
    /*
    cout << "Data after heap sort:" << endl;
    for (const Data& data : dataList) {
        cout << "Date: " << data.date << ", Cumulative: " << data.cumulative << endl;
    }
    */

    // Έναρξη χρονοδιακόπτη
    start = chrono::high_resolution_clock::now();

    // Εκτέλεση quicksort
    quickSortIterative(dataListCopy, 0, dataListCopy.size()-1);

    // Διακοπή χρονοδιακόπτη και υπολογισμός διάρκειας
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Εκτύπωση χρόνου που απαιτήθηκε για την quicksort
    cout << "Time taken by quick sort: "
         << duration.count() << " microseconds" << endl;

    // Print του Sorted Data,ωστοσο εχει γινει "comment out" για να μην υπαρχει cluttering στο Console.
    /*
    cout << "Data after quick sort:" << endl;
    for (const Data& data : dataListCopy) {
        cout << "Date: " << data.date << ", Cumulative: " << data.cumulative << endl;
    }
    */


    return 0;
}

