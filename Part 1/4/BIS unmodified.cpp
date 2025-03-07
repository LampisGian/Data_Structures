#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <algorithm>  // Για sort function.
using namespace std;

struct Data {
    long long date;  // Ημερομηνία ως Unix time
    long long value;  // Τιμή ως long long
};

// Μετατροπή ενός string ημερομηνίας σε Unix time
long long convertDateToUnix(string date) {
    std::tm tm = {};  // Αρχικοποίηση σε μηδέν
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%d/%m/%Y");  // Προσαρμογή σε μορφή dd/mm/yyyy
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0; // Υποθέτοντας την έναρξη της ημέρας
    time_t time = mktime(&tm);
    return (long long)time;
}

// Ανάγνωση CSV αρχείου και αποθήκευση τιμών σε ένα vector τύπου Data
vector<Data> read_csv(string filename) {
    vector<Data> dataList;
    ifstream file(filename);

    string line, word;
    getline(file, line);  // Παράβλεψη επικεφαλίδας

    // Επανάληψη για κάθε γραμμή του αρχείου
    while (getline(file, line)) {
        vector<string> row;
        stringstream s(line);

        // Επανάληψη για κάθε λέξη της γραμμής
        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        Data data;
        data.date = convertDateToUnix(row[2]);  // Μετατροπή ημερομηνίας σε Unix time

        // Έλεγχος εάν η συμβολοσειρά μπορεί να μετατραπεί σε long long
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

// Δυαδική αναζήτηση παρεμβολης
int BinaryInterpolationSearch(vector<Data>& arr, int start, int end, long long target) {
    int i = 0;
    int size = (end - start) + 1;
    int middlepos = start + ((float)(target - arr[start].date) / (arr[end].date - arr[start].date)) * (end - start);

    if (target > arr[end].date) {
        return -1;
    }

    while (target != arr[middlepos].date && sqrt(size) <= end && start <= sqrt(size)) {
        size = (end - start) + 1;

        if (target >= arr[middlepos].date) {
            int test = middlepos + i * sqrt(size) - 1;

            while (target > arr[test].date) {
                i++;
                test = middlepos + i * sqrt(size) - 1;
            }

            end = middlepos + i * sqrt(size);
            start = middlepos + (i - 1) * sqrt(size);
        } else if (target < arr[middlepos].date) {
            int test = middlepos - i * sqrt(size) + 1;

            while (target < arr[test].date) {
                i++;
                test = middlepos - i * sqrt(size) + 1;
            }

            end = middlepos - (i - 1) * sqrt(size);
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
    // Διαβάσμα των δεδομένων από το CSV αρχείο
    vector<Data> dataList = read_csv("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");

    // Ταξινομήση του dataList κατά ημερομηνία
    sort(dataList.begin(), dataList.end(), [](const Data& a, const Data& b) {
        return a.date < b.date;
    });

    // Λήψη ημερομηνίας από τον χρήστη
    string date;
    cout << "Enter a date (dd/mm/yyyy): ";  // Προσαρμογή σε μορφή dd/mm/yyyy
    getline(cin, date);

    // Μετατροπή ημερομηνίας σε Unix time
    long long unixDate = convertDateToUnix(date);

    // Έναρξη χρονομέτρου
    auto start = chrono::high_resolution_clock::now();

    // Εκτέλεση BIS
    int index = BinaryInterpolationSearch(dataList, 0, dataList.size() - 1, unixDate);

    // Διακοπή χρονομέτρου και υπολογισμός διάρκειας
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Έλεγχος αν βρέθηκε η ημερομηνία
    if (index != -1) {
        // Εκτύπωση τιμής για την ημερομηνία
        cout << "Value for " << date << ": " << dataList[index].value << endl;
    } else {
        cout << "Date not found" << endl;
    }

    // Εκτύπωση διάρκειας αναζήτησης
    cout << "Time taken by search: " << duration.count() << " microseconds" << endl;

    return 0;
}
