#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class TextCollection {
public:
    // The lambda here is the custom deleter -- it runs exactly once,
    // when the refcount drops to zero, and is the ONLY place we delete.
    // Note: can't use make_shared when attaching a custom deleter.
    TextCollection()
        : data(new std::vector<std::string>(),
               [](std::vector<std::string>* p) {
                   std::cout << "[deleter] shared vector destroyed (had "
                             << p->size() << " word(s))\n";
                   delete p;
               }) {}

    // delegating ctor so we reuse the custom-deleter setup above
    explicit TextCollection(const std::string& filename)
        : TextCollection()
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
        // inner lives only inside this block -- refcount goes 1 -> 2
        std::cout << "\n-- entering inner scope, copying outer into inner --\n";
        TextCollection inner = outer;
        inner.addWord("gamma");
        std::cout << "inner sees: ";
        inner.printAll();
        std::cout << "outer owner count = " << outer.ownerCount()
                  << " (2 because inner shares the vector)\n";
        std::cout << "-- leaving inner scope: deleter should NOT fire yet --\n";
    }
    // inner is destroyed here, refcount drops back to 1, but the
    // vector survives because outer is still alive.

    std::cout << "\nBack in main. outer owner count = "
              << outer.ownerCount() << " (back to 1)\n";
    std::cout << "outer still sees: ";
    outer.printAll();

    // when main returns, outer dies -> refcount 0 -> deleter fires
    std::cout << "\n-- about to return from main: deleter SHOULD fire now --\n";
    return 0;
}
