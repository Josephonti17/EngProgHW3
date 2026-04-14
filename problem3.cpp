// Problem 3 - Dynamic Memory and Object Lifetime
//
// TextCollection wraps a vector<string> inside a std::shared_ptr.
// Copying a TextCollection gives you a second "view" that shares
// the exact same underlying vector -- so adding or removing a word
// through one view is visible through every other view that shares
// the same control block. The vector is destroyed automatically
// when the last owner goes out of scope (RAII, no raw new/delete).

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class TextCollection {
public:
    // default ctor: this collection owns a brand-new empty vector
    TextCollection()
        : data(std::make_shared<std::vector<std::string>>()) {}

    // file ctor: reads one whitespace-separated word at a time
    explicit TextCollection(const std::string& filename)
        : data(std::make_shared<std::vector<std::string>>()) {
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

    // append a word to the shared vector
    void addWord(const std::string& word) {
        data->push_back(word);
    }

    // remove the first occurrence of a word, if present
    void removeWord(const std::string& word) {
        auto it = std::find(data->begin(), data->end(), word);
        if (it != data->end()) {
            data->erase(it);
        }
    }

    // print every word in insertion order
    void printAll() const {
        for (const auto& w : *data) {
            std::cout << w << " ";
        }
        std::cout << "\n";
    }

    // how many TextCollection objects currently share this vector?
    long ownerCount() const { return data.use_count(); }

private:
    // Ownership: shared_ptr<vector<string>>.
    // The compiler-generated copy ctor copies the shared_ptr, which
    // increments the reference count -- both objects now point to
    // the same vector. No manual copy ctor needed.
    std::shared_ptr<std::vector<std::string>> data;
};

int main() {
    std::cout << "--- Build collection A from sample.txt ---\n";
    TextCollection a("word_index_test.txt");
    a.printAll();
    std::cout << "A owner count = " << a.ownerCount() << "\n\n";

    std::cout << "--- Make B a shared copy of A ---\n";
    TextCollection b = a;     // b and a now share the same vector
    std::cout << "A owner count = " << a.ownerCount() << "\n";
    std::cout << "B owner count = " << b.ownerCount() << "\n\n";

    std::cout << "--- Add \"hello\" through B ---\n";
    b.addWord("hello");
    std::cout << "A sees: ";
    a.printAll();
    std::cout << "B sees: ";
    b.printAll();
    std::cout << "(Both views updated because they share the vector)\n\n";

    std::cout << "--- Remove \"hello\" through A ---\n";
    a.removeWord("hello");
    std::cout << "B sees: ";
    b.printAll();
    std::cout << "\n";

    std::cout << "--- Independent collection C (separate vector) ---\n";
    TextCollection c;
    c.addWord("standalone");
    c.addWord("collection");
    c.printAll();
    std::cout << "C owner count = " << c.ownerCount() << "\n";
    std::cout << "A owner count = " << a.ownerCount()
              << " (unchanged, A and C are independent)\n";

    return 0;
}
