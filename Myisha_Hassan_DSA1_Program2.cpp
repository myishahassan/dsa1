/*
Myisha Hassan
ECE264 DSA I Program #2

Description:
Implements stacks and queues using a singly linked list. The program reads
commands from an input file to create lists, push values onto them, and pop
values from them, and writes results to an output file.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>

// Used to determine which list type to use
enum DataType {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING
};

// Abstract base class
template <typename T>
class SimpleList {
private:
    // Node structure for singly linked list
    class Node {
    public:
        T data;
        Node* next;
        Node(const T& value, Node* nextNode = nullptr)
            : data(value), next(nextNode) {}
    };

    Node* head;
    Node* tail;
    std::string name;

protected:
    void insertStart(const T& value);
    void insertEnd(const T& value);
    T removeStart();

public:
    SimpleList(const std::string& listName);
    virtual ~SimpleList();

    std::string getName() const;
    bool isEmpty() const;

    // Pure virtual functions - implemented by Stack and Queue
    virtual void push(const T& value) = 0;
    virtual T pop() = 0;
};

// Stack implementation - LIFO (push and pop from front)
template <typename T>
class Stack : public SimpleList<T> {
public:
    Stack(const std::string& name);
    void push(const T& value) override;
    T pop() override;
};

// Queue implementation - FIFO (push at end, pop from front)
template <typename T>
class Queue : public SimpleList<T> {
public:
    Queue(const std::string& name);
    void push(const T& value) override;
    T pop() override;
};

// Find a list by name in the list of SimpleList pointers
template <typename T>
SimpleList<T>* findList(std::list< SimpleList<T>* >& lst, const std::string& name) {
    for (auto p : lst) {
        if (p->getName() == name) return p;
    }
    return nullptr;
}

// Determine data type from first character of list name
DataType getDataType(const std::string& name) {
    if (name[0] == 'i') return TYPE_INT;
    if (name[0] == 'd') return TYPE_DOUBLE;
    return TYPE_STRING;
}

// SimpleList member function implementations
template <typename T>
SimpleList<T>::SimpleList(const std::string& listName)
    : head(nullptr), tail(nullptr), name(listName) {
}

template <typename T>
SimpleList<T>::~SimpleList() {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
}

template <typename T>
void SimpleList<T>::insertStart(const T& value) {
    Node* newNode = new Node(value, head);
    head = newNode;
    if (tail == nullptr) tail = newNode;
}

template <typename T>
void SimpleList<T>::insertEnd(const T& value) {
    Node* newNode = new Node(value);
    if (tail == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
}

template <typename T>
T SimpleList<T>::removeStart() {
    Node* oldHead = head;
    T value = oldHead->data;
    head = head->next;
    if (head == nullptr) tail = nullptr;
    delete oldHead;
    return value;
}

template <typename T>
std::string SimpleList<T>::getName() const {
    return name;
}

template <typename T>
bool SimpleList<T>::isEmpty() const {
    return head == nullptr;
}

// Stack member function implementations
template <typename T>
Stack<T>::Stack(const std::string& name)
    : SimpleList<T>(name) {
}

template <typename T>
void Stack<T>::push(const T& value) {
    this->insertStart(value);
}

template <typename T>
T Stack<T>::pop() {
    return this->removeStart();
}

// Queue member function implementations
template <typename T>
Queue<T>::Queue(const std::string& name)
    : SimpleList<T>(name) {
}

template <typename T>
void Queue<T>::push(const T& value) {
    this->insertEnd(value);
}

template <typename T>
T Queue<T>::pop() {
    return this->removeStart();
}

// Get input and output filenames from user
void get_files(std::string& inputFileName, std::string& outputFileName) {
    std::cout << "Enter name of input file: ";
    std::cin >> inputFileName;
    std::cout << "Enter name of output file: ";
    std::cin >> outputFileName;
}

// Process all commands from input file
void process_commands(const std::string& inputFileName, const std::string& outputFileName) {
    std::ifstream inFile(inputFileName);
    std::ofstream outFile(outputFileName);

    if (!inFile.is_open() || !outFile.is_open()) return;

    // Lists for each data type
    std::list< SimpleList<int>* > listSLi;
    std::list< SimpleList<double>* > listSLd;
    std::list< SimpleList<std::string>* > listSLs;

    std::string line;

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        outFile << "PROCESSING COMMAND: " << line << '\n';

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        // CREATE command
        if (command == "create") {
            std::string listName, which;
            iss >> listName >> which;

            DataType type = getDataType(listName);

            if (type == TYPE_INT) {
                SimpleList<int>* p = findList(listSLi, listName);
                if (p != nullptr) {
                    outFile << "ERROR: This name already exists!" << '\n';
                } else {
                    if (which == "stack")
                        listSLi.push_front(new Stack<int>(listName));
                    else
                        listSLi.push_front(new Queue<int>(listName));
                }
            }
            else if (type == TYPE_DOUBLE) {
                SimpleList<double>* p = findList(listSLd, listName);
                if (p != nullptr) {
                    outFile << "ERROR: This name already exists!" << '\n';
                } else {
                    if (which == "stack")
                        listSLd.push_front(new Stack<double>(listName));
                    else
                        listSLd.push_front(new Queue<double>(listName));
                }
            }
            else {
                SimpleList<std::string>* p = findList(listSLs, listName);
                if (p != nullptr) {
                    outFile << "ERROR: This name already exists!" << '\n';
                } else {
                    if (which == "stack")
                        listSLs.push_front(new Stack<std::string>(listName));
                    else
                        listSLs.push_front(new Queue<std::string>(listName));
                }
            }
        }

        // PUSH command
        else if (command == "push") {
            std::string listName, valueStr;
            iss >> listName >> valueStr;

            DataType type = getDataType(listName);

            if (type == TYPE_INT) {
                SimpleList<int>* p = findList(listSLi, listName);
                if (p == nullptr) outFile << "ERROR: This name does not exist!" << '\n';
                else {
                    int value;
                    std::istringstream conv(valueStr);
                    conv >> value;
                    p->push(value);
                }
            }
            else if (type == TYPE_DOUBLE) {
                SimpleList<double>* p = findList(listSLd, listName);
                if (p == nullptr) outFile << "ERROR: This name does not exist!" << '\n';
                else {
                    double value;
                    std::istringstream conv(valueStr);
                    conv >> value;
                    p->push(value);
                }
            }
            else {
                SimpleList<std::string>* p = findList(listSLs, listName);
                if (p == nullptr) outFile << "ERROR: This name does not exist!" << '\n';
                else p->push(valueStr);
            }
        }

        // POP command
        else if (command == "pop") {
            std::string listName;
            iss >> listName;

            DataType type = getDataType(listName);

            if (type == TYPE_INT) {
                SimpleList<int>* p = findList(listSLi, listName);
                if (p == nullptr) outFile << "ERROR: This name does not exist!" << '\n';
                else if (p->isEmpty()) outFile << "ERROR: This list is empty!" << '\n';
                else outFile << "Value popped: " << p->pop() << '\n';
            }
            else if (type == TYPE_DOUBLE) {
                SimpleList<double>* p = findList(listSLd, listName);
                if (p == nullptr) outFile << "ERROR: This name does not exist!" << '\n';
                else if (p->isEmpty()) outFile << "ERROR: This list is empty!" << '\n';
                else outFile << "Value popped: " << p->pop() << '\n';
            }
            else {
                SimpleList<std::string>* p = findList(listSLs, listName);
                if (p == nullptr) outFile << "ERROR: This name does not exist!" << '\n';
                else if (p->isEmpty()) outFile << "ERROR: This list is empty!" << '\n';
                else outFile << "Value popped: " << p->pop() << '\n';
            }
        }
    }

    // Clean up allocated lists
    for (auto p : listSLi) delete p;
    for (auto p : listSLd) delete p;
    for (auto p : listSLs) delete p;
}

int main() {
    std::string inputFileName, outputFileName;
    get_files(inputFileName, outputFileName);
    process_commands(inputFileName, outputFileName);
    return 0;
}