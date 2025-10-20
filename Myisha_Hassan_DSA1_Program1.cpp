// Myisha Hassan
// ECE264 DSA I Program #1: Foobar strength calculator
// Reads foobars from file, calculates strength based on type and position,
// and writes results to output file.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Base class for all foobars - stores name and position in line
class Foobar {
private:
    std::string name_;
    int position_; // 0 means not in line yet
protected:
    int getPosition() const { return position_; } // for derived classes
public:
    Foobar(const std::string& name, int pos = 0) : name_(name), position_(pos) {}
    virtual ~Foobar() = default;

    void setPosition(int pos) { position_ = pos; }
    const std::string& getName() const { return name_; }

    // Virtual function - strength calculation depends on foobar type
    virtual int getStrength() const { return getPosition(); }
};

// Foo creatures have triple strength
class Foo : public Foobar {
public:
    using Foobar::Foobar;
    int getStrength() const override { return getPosition() * 3; }
};

// Bar creatures get +15 to their strength
class Bar : public Foobar {
public:
    using Foobar::Foobar;
    int getStrength() const override { return getPosition() + 15; }
};

// Read input file - each line has type and name
// First line in file = back of line, last line = front
bool readInput(const std::string& inputFilename, std::vector<Foobar*>& vec) {
    std::ifstream in(inputFilename);
    if (!in) return false;

    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string type, name;
        ss >> type >> name;

        // Create appropriate object based on type
        if (type == "foobar") {
            vec.push_back(new Foobar(name));
        } else if (type == "foo") {
            vec.push_back(new Foo(name));
        } else if (type == "bar") {
            vec.push_back(new Bar(name));
        }
    }
    return true;
}

// Assign positions: last in vector (front of line) gets position 1
void updatePositions(std::vector<Foobar*>& vec) {
    int pos = 1;
    for (int i = vec.size() - 1; i >= 0; --i) {
        vec[i]->setPosition(pos);
        ++pos;
    }
}

// Write output: name and strength for each foobar
bool writeOutput(const std::string& outputFilename, const std::vector<Foobar*>& vec) {
    std::ofstream out(outputFilename);
    if (!out) return false;

    for (Foobar* p : vec) {
        out << p->getName() << ' ' << p->getStrength() << '\n';
    }
    return true;
}

int main() {
    std::vector<Foobar*> foobars;

    std::string inName, outName;
    std::cout << "Enter the name of the input file: ";
    std::cin >> inName;
    std::cout << "Enter the name of the output file: ";
    std::cin >> outName;

    if (!readInput(inName, foobars)) {
        return 1;
    }

    updatePositions(foobars);

    if (!writeOutput(outName, foobars)) {
        // Clean up before exit
        for (Foobar* p : foobars) delete p;
        return 1;
    }

    // Clean up allocated objects
    for (Foobar* p : foobars) delete p;
    return 0;
}