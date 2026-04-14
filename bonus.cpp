// Bonus Problem - Custom Deleter and Resource Logging
// EE 5102 / EE 4953 - Homework 3
// Joseph
//
// Same idea as Problem 3, but the shared_ptr is constructed with
// a custom deleter lambda that prints a message when the vector
// is actually destroyed. This only happens when the LAST owner
// goes out of scope, which we demonstrate with a nested block.
//
// Note: when attaching a custom deleter you can't use make_shared,
// so we give shared_ptr a raw `new` pointer plus the lambda. The
// shared_ptr still fully owns it -- the only `delete` is inside
// the deleter itself.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class TextCollection {
public:
    TextCollection()
        : data(new std::vector<std::string>(),
               [](std::vector<std::string>* p) {
                   std::cout << "[deleter] shared vector destroyed (had "
                             << p->size() << " word(s))\n";
                   delete p;
               }) {}

    explicit TextCollection(const std::string& filename)
        : TextCollection()  // delegate so we reuse the custom deleter
    {
        std::ifstream in(filename);
        if (!in) {
            std::cerr << "Warning: could not open " << filename << "\n";
            return;
        }
        std::string w;
        while (in >> w) {
            data->push_back(w);
        }
    }

    void addWord(const std::string& word) {
        data->push_back(word);
    }

    void removeWord(const std::string& word) {
        auto it = std::find(data->begin(), data->end(), word);
        if (it != data->end()) {
            data->erase(it);
        }
    }

    void printAll() const {
        for (const auto& w : *data) {
            std::cout << w << " ";
        }
        std::cout << "\n";
    }

    long ownerCount() const { return data.use_count(); }

private:
    std::shared_ptr<std::vector<std::string>> data;
};

int main() {
    std::cout << "Creating outer collection\n";
    TextCollection outer;
    outer.addWord("alpha");
    outer.addWord("beta");
    std::cout << "outer owner count = " << outer.ownerCount() << "\n";

    {
        std::cout << "\n-- entering inner scope, copying outer into inner --\n";
        TextCollection inner = outer;
        inner.addWord("gamma");
        std::cout << "inner sees: ";
        inner.printAll();
        std::cout << "outer owner count = " << outer.ownerCount()
                  << " (2 because inner shares the vector)\n";
        std::cout << "-- leaving inner scope: deleter should NOT fire yet --\n";
    }

    std::cout << "\nBack in main. outer owner count = "
              << outer.ownerCount() << " (back to 1)\n";
    std::cout << "outer still sees: ";
    outer.printAll();

    std::cout << "\n-- about to return from main: deleter SHOULD fire now --\n";
    return 0;
}
