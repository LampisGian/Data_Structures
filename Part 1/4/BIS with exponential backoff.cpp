#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <algorithm>  // For the sort function
using namespace std;

struct Data {
    long long date;  // Ημερομηνία ως Unix time
    long long value;   // Τιμή ως long long
};

// Μετατροπή μιας συμβολοσειράς ημερομηνίας σε Unix time
long long convertDateToUnix(string date) {
    std::tm tm = {};  // Αρχικοποίηση σε μηδέν
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%d/%m/%Y");  // Προσαρμογή σε μορφή dd/mm/yyyy
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0; // Υποθέτοντας την αρχή της ημέρας
    time_t time = mktime(&tm);
    return (long long)time;
}

// Διαβάσμα του αρχείου CSV και αποθηκεύση τις τιμές του σε ένα vector του Data
vector<Data> read_csv(string filename) {
    vector<Data> dataList;
    ifstream file(filename);

    string line, word;
    getline(file, line);  // Παράκαμψη της γραμμής επικεφαλίδας

    // Βρόγχος για κάθε γραμμή στο αρχείο
    while (getline(file, line)) {
        vector<string> row;
        stringstream s(line);

        // Βρόγχος για κάθε λέξη στη γραμμή
        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        Data data;
        data.date = convertDateToUnix(row[2]);  // Μετατροπή ημερομηνίας σε Unix time

       // Έλεγχος αν η συμβολοσειρά μπορεί να μετατραπεί σε long long
        if (!row[8].empty() && row[8].find_first_not_of("-0123456789") == string::npos) {
            try {
                data.value = stoll(row[8]);  // Αποθήκευση τιμής
            }
            catch (std::out_of_range&) {
                data.value = 0;
            }
        } else {
            data.value = 0;
        }

        dataList.push_back(data);
    }

    file.close();
    return dataList;
}

// Binary Interpolation Search with exponential backoff
int BinaryInterpolationSearch(vector<Data>& arr, int start, int end, long long target) {
    int i = 1;  // // Ρυθμίση του i σε 1 αντί για 0 για exponential backoff
    int size = (end - start) + 1;
    int middlepos = start + ((float)(target - arr[start].date) / (arr[end].date - arr[start].date)) * (end - start);

    if (target > arr[end].date) {
        return -1;
    }

    while (target != arr[middlepos].date && i <= end && start <= i) {
        size = (end - start) + 1;

        if (target >= arr[middlepos].date) {
            int test = middlepos + i * sqrt(size) - 1;

            while (target > arr[test].date && test < end) {
                i = i * 2;  // Exponential backoff
                test = middlepos + i * sqrt(size) - 1;
            }

            end = middlepos + i * sqrt(size);
            start = middlepos + (i / 2) * sqrt(size);  // Προσαρμογή σε i/2 επειδή το i έχει διπλασιαστεί
        } else if (target < arr[middlepos].date) {
            int test = middlepos - i * sqrt(size) + 1;

            while (target < arr[test].date && test > start) {
                i = i * 2;  // Exponential backoff
                test = middlepos - i * sqrt(size) + 1;
            }

            end = middlepos - (i / 2) * sqrt(size);  // Προσαρμογή σε i/2 επειδή το i έχει διπλασιαστεί
            start = middlepos - i * sqrt(size);
        }

        middlepos = start + ((float)(target - arr[start].date) / (arr[end].date - arr[start].date)) * (end - start);
    }

    if (target == arr[middlepos].date) {
        return middlepos;
    } else {
        return -1;
    }
}

int main() {
    // Διαβασμα δεδομενων απο το CSV file
    vector<Data> dataList = read_csv("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");

    // Ταξινόμηση dataList κατά ημερομηνία
    sort(dataList.begin(), dataList.end(), [](const Data& a, const Data& b) {
        return a.date < b.date;
    });

    // Λήψη ημερομηνίας από τον χρήστη
    string date;
    cout << "Enter a date (dd/mm/yyyy): ";  // Adjusted to dd/mm/yyyy format
    getline(cin, date);

    //  Μετατροπή ημερομηνίας σε Unix time
    long long unixDate = convertDateToUnix(date);

    // Έναρξη χρονομέτρου
    auto start = chrono::high_resolution_clock::now();

    //Εκτέλεση BIS
    int index = BinaryInterpolationSearch(dataList, 0, dataList.size() - 1, unixDate);

    //  Διακοπή χρονομέτρου και υπολογισμός διάρκειας
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Έλεγχος αν βρέθηκε η ημερομηνία
    if (index != -1) {
        // Εκτύπωση τιμής για την ημερομηνί
        cout << "Value for " << date << ": " << dataList[index].value << endl;
    } else {
        cout << "Date not found." << endl;
    }

    // Εκτύπωση διάρκειας αναζήτησης
    cout << "Time taken by search: " << duration.count() << " microseconds" << endl;

    return 0;
}
