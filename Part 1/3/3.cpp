#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

// Δημιουργούμε μια δομή για την αποθήκευση των δεδομένων Date και Value
struct Data {
    std::string date;
    double value;
};

// Συνάρτηση για τη μετατροπή μιας συμβολοσειράς ημερομηνίας από τη μορφή "DD/MM/YYYY" σε ακέραιο στη μορφή YYYYMMDD
int dateToInt(const std::string& date) {
    return std::stoi(date.substr(6,4) + date.substr(3,2) + date.substr(0,2));
}

// Συνάρτηση σύγκρισης για τη δομή Data με βάση την Date
bool compareByDate(const Data& a, const Data& b) {
    return dateToInt(a.date) < dateToInt(b.date);
}

// Συνάρτηση για τον έλεγχο εάν μια συμβολοσειρά μπορεί να μετατραπεί σε Double
bool isDouble(const std::string& str) {
    char* end = nullptr;
    double val = strtod(str.c_str(), &end);
    return end != str.c_str() && *end == '\0' && val != HUGE_VAL;
}

// Συνάρτηση για την ανάγνωση αρχείου CSV και την αποθήκευση των δεδομένων σε ένα vector τύπου Data
std::vector<Data> readCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Data> dataVec;
    std::string line, word;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream s(line);
        Data data;
        int columnCount = 0;
        while (std::getline(s, word, ',')) {
            columnCount++;
            if (columnCount == 3) {
                data.date = word;
            }
            else if (columnCount == 9) {
                if (isDouble(word)) {
                    data.value = std::stod(word);
                } else {
                    data.value = 0;
                }
            }
        }
        dataVec.push_back(data);
    }
    file.close();
    return dataVec;
}

// Συνάρτηση δυαδικής αναζήτησης για την εύρεση μιας ημερομηνίας
int binarySearch(std::vector<Data>& arr, const std::string& targetDate) {
    int left = 0, right = arr.size() - 1;
    int target = dateToInt(targetDate);

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int midDate = dateToInt(arr[mid].date);

        if (midDate == target) {
            return mid;
        }

        if (midDate < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

// Συνάρτηση αναζήτησης παρεμβολης για την εύρεση μιας ημερομηνίας
int interpolationSearch(std::vector<Data>& arr, const std::string& targetDate) {
    int left = 0, right = arr.size() - 1;
    int target = dateToInt(targetDate);

    while (left <= right && target >= dateToInt(arr[left].date) && target <= dateToInt(arr[right].date)) {
        if (right == left) {
            if (dateToInt(arr[left].date) == target) return left;
            return -1;
        }

        int pos = left + ((target - dateToInt(arr[left].date)) / (dateToInt(arr[right].date) - dateToInt(arr[left].date))) * (right - left);

        if (dateToInt(arr[pos].date) == target) return pos;
        if (dateToInt(arr[pos].date) < target) left = pos + 1;
        else right = pos - 1;
    }

    return -1;
}

// Κύρια συνάρτηση
int main() {
    // Διαβάζουμε τα δεδομένα από το αρχείο CSV
    std::vector<Data> data = readCSV("effects-of-covid-19-on-trade-at-15-december-2021-provisional.csv");

    // Ταξινομούμε τα δεδομένα με βάση την ημερομηνία
    std::sort(data.begin(), data.end(), compareByDate);

    std::string targetDate;
    std::cout << "Enter a date (DD/MM/YYYY): ";
    std::getline(std::cin, targetDate);

    // Χρονομετρούμε την διαδικασία της δυαδικής αναζήτησης
    auto start = std::chrono::high_resolution_clock::now();
    int dateIndexBinarySearch = binarySearch(data, targetDate);
    auto end = std::chrono::high_resolution_clock::now();
    auto binarySearchTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Binary search time: " << binarySearchTime << " microseconds." << std::endl;

    // Αν η δυαδική αναζήτηση βρήκε την ημερομηνία, επίσης εκτελούμε την αναζήτηση παρεμβολης και χρονομετρούμε
    if (dateIndexBinarySearch != -1) {
        double targetValue = data[dateIndexBinarySearch].value;
        std::cout << "Binary search found the date at index " << dateIndexBinarySearch <<std::endl;

        start = std::chrono::high_resolution_clock::now();
        int dateIndexInterpolationSearch = interpolationSearch(data, targetDate);
        end = std::chrono::high_resolution_clock::now();
        auto interpolationSearchTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Interpolation search time: " << interpolationSearchTime << " microseconds." << std::endl;

        // Αναφέρουμε τα αποτελέσματα της αναζήτησης παρεμβολης
        if (dateIndexInterpolationSearch != -1) {
            std::cout << "Interpolation search found the date at index " << dateIndexInterpolationSearch <<std::endl;
        } else {
            std::cout << "Interpolation search did not find the date.\n";
        }
    } else {
        std::cout << "Binary search did not find the date.\n";
    }

    return 0;
}
