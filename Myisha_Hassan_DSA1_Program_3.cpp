// THIS IS THE PROVIDED CODE FOR PROGRAM #3, DSA 1, FALL 2025

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cctype>
#include <cstdlib>

using namespace std;

// A simple class; each object holds three public fields
class Data {
public:
  string lastName;
  string firstName;
  string ssn;
};

// Load the data from a specified input file
void loadDataList(list<Data *> &l, const string &filename) {

  ifstream input(filename);
  if (!input) {
    cerr << "Error: could not open " << filename << "\n";
    exit(1);
  }

  // The first line indicates the size
  string line;
  getline(input, line);
  stringstream ss(line);
  int size;
  ss >> size;

  // Load the data
  for (int i = 0; i < size; i++) {
    getline(input, line);
    stringstream ss2(line);
    Data *pData = new Data();
    ss2 >> pData->lastName >> pData->firstName >> pData->ssn;
    l.push_back(pData);
  }

  input.close();
}

// Output the data to a specified output file
void writeDataList(const list<Data *> &l, const string &filename) {

  ofstream output(filename);
  if (!output) {
    cerr << "Error: could not open " << filename << "\n";
    exit(1);
  }

  // Write the size first
  int size = l.size();
  output << size << "\n";

  // Write the data
  for (auto pData:l) {
    output << pData->lastName << " " 
	   << pData->firstName << " " 
	   << pData->ssn << "\n";
  }

  output.close();
}

// Sort the data according to a specified field
// (Implementation of this function will be later in this file)
void sortDataList(list<Data *> &);

// The main function calls routines to get the data, sort the data,
// and output the data. The sort is timed according to CPU time.
int main() {
  string filename;
  cout << "Enter name of input file: ";
  cin >> filename;
  list<Data *> theList;
  loadDataList(theList, filename);

  cout << "Data loaded.\n";

  cout << "Executing sort...\n";
  clock_t t1 = clock();
  sortDataList(theList);
  clock_t t2 = clock();
  double timeDiff = ((double) (t2 - t1)) / CLOCKS_PER_SEC;

  cout << "Sort finished. CPU time was " << timeDiff << " seconds.\n";

  cout << "Enter name of output file: ";
  cin >> filename;
  writeDataList(theList, filename);

  return 0;
}

// -------------------------------------------------
// YOU MAY NOT CHANGE OR ADD ANY CODE ABOVE HERE !!!
// -------------------------------------------------

// You may add global variables, functions, and/or
// class definitions here if you wish.

/*
    @author Myisha Hassan

    This program sorts an input file based on last names, 
    first names, and SSNs and outputs the sorted data. 
    It handles three scenarios:
      - T1/T2: general case (full sort)
      - T3: already sorted by names (sort same-name blocks by SSN)
      - T4: all names identical (fast SSN-only radix sort)
*/

// Maximum records capacity
const int MAX_DATA_SIZE = 1100000;

// Global arrays for sorting
static Data* g_dataArray[MAX_DATA_SIZE];
static Data* g_tmpArray[MAX_DATA_SIZE];  // used for radix / counting sort

// SSNs are in the format "DDD-DD-DDDD".
// These are the indices of the 9 digits in the SSN string.
const int SSN_DIGIT_INDICES[] = {0, 1, 2, 4, 5, 7, 8, 9, 10};
const int SSN_NUM_DIGITS      = 9;

// Comparator: last name, first name, then SSN
inline bool myLessThan(const Data* a, const Data* b) {
    int cmp = a->lastName.compare(b->lastName);
    if (cmp != 0) return cmp < 0;

    cmp = a->firstName.compare(b->firstName);
    if (cmp != 0) return cmp < 0;

    return a->ssn < b->ssn;
}

// Comparator for SSN-only sorting (used for T3 blocks)
inline bool myLessThanSSN(const Data* a, const Data* b) {
    return a->ssn < b->ssn;
}

// Get a single SSN digit (0–9) at the given position.
// position = 0 is least significant digit (rightmost),
// position = 8 is most significant digit (leftmost).
inline int getSsnDigit(const Data* pData, int position) {
    int ssnCharIndex = SSN_DIGIT_INDICES[SSN_NUM_DIGITS - 1 - position];
    return pData->ssn[ssnCharIndex] - '0';
}

// Stable counting sort on one SSN digit, used by radixSortT4
void countingSortByDigit(int n, int position) {
    const int RADIX = 10;
    int count[RADIX] = {0};

    // Count digit occurrences
    for (int i = 0; i < n; ++i) {
        int d = getSsnDigit(g_dataArray[i], position);
        count[d]++;
    }

    // Convert to cumulative counts
    for (int i = 1; i < RADIX; ++i) {
        count[i] += count[i - 1];
    }

    // Build output array (stable: process from right to left)
    for (int i = n - 1; i >= 0; --i) {
        int d = getSsnDigit(g_dataArray[i], position);
        g_tmpArray[count[d] - 1] = g_dataArray[i];
        count[d]--;
    }

    // Copy back into main array
    for (int i = 0; i < n; ++i) {
        g_dataArray[i] = g_tmpArray[i];
    }
}

// Radix sort for T4: sort by SSN digits only (all names identical)
void radixSortT4(int n) {
    // LSD radix: sort from least significant to most significant digit
    for (int position = 0; position < SSN_NUM_DIGITS; ++position) {
        countingSortByDigit(n, position);
    }
}

void sortDataList(list<Data *> &l) {
    int n = static_cast<int>(l.size());
    if (n <= 1) return;

    // Copy list pointers to array
    int index = 0;
    for (Data* p : l) {
        g_dataArray[index++] = p;
    }

    // Check if all names are identical and SSNs have correct format (T4 case)
    bool allSameName = true;
    const string& baseLast  = g_dataArray[0]->lastName;
    const string& baseFirst = g_dataArray[0]->firstName;

    for (int i = 0; i < n; ++i) {
        if (g_dataArray[i]->lastName  != baseLast ||
            g_dataArray[i]->firstName != baseFirst) {
            allSameName = false;
            break;
        }
    }

    if (allSameName) {
        // T4: all names identical -> sort by SSN only (radix sort)
        radixSortT4(n);

    } else {
        // Check if names are already sorted by (lastName, firstName) (T3 case)
        bool namesNonDecreasing = true;
        for (int i = 0; i < n - 1; ++i) {
            if (g_dataArray[i]->lastName > g_dataArray[i + 1]->lastName ||
               (g_dataArray[i]->lastName == g_dataArray[i + 1]->lastName &&
                g_dataArray[i]->firstName > g_dataArray[i + 1]->firstName)) {
                namesNonDecreasing = false;
                break;
            }
        }

        if (namesNonDecreasing) {
            // T3: already sorted by (lastName, firstName)
            // Sort only blocks with identical names by SSN
            int i = 0;
            while (i < n) {
                int j = i + 1;
                while (j < n &&
                       g_dataArray[j]->lastName  == g_dataArray[i]->lastName &&
                       g_dataArray[j]->firstName == g_dataArray[i]->firstName) {
                    ++j;
                }
                if (j - i > 1) {
                    std::sort(g_dataArray + i, g_dataArray + j, myLessThanSSN);
                }
                i = j;
            }

        } else {
            // T1/T2: general case -> full sort by lastName, firstName, then SSN
            std::sort(g_dataArray, g_dataArray + n, myLessThan);
        }
    }

    // Copy sorted pointers back to list
    auto it = l.begin();
    for (int i = 0; i < n; ++i, ++it) {
        *it = g_dataArray[i];
    }
}