#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

class TextCollection {
public:
    // default ctor: make a new empty shared vector
    TextCollection()
        : data(std::make_shared<std::vector<std::string>>()) {}

    // file ctor: read whitespace-separated words from a file
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

    void addWord(const std::string& word) {
        data->push_back(word);
    }

    // remove the first matching word if it exists
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

    // how many TextCollection objects share this vector?
    long ownerCount() const { return data.use_count(); }

private:
    // shared_ptr means copying a TextCollection just bumps the refcount
    // instead of cloning the vector -- that's how shared ownership works.
    // When the last owner dies, the vector is freed automatically.
    std::shared_ptr<std::vector<std::string>> data;
};

int main() {
    std::cout << "--- Build collection A from word_index_test.txt ---\n";
    TextCollection a("word_index_test.txt");
    a.printAll();
    std::cout << "A owner count = " << a.ownerCount() << "\n\n";

    std::cout << "--- Make B a shared copy of A ---\n";
    TextCollection b = a;   // refcount goes 1 -> 2
    std::cout << "A owner count = " << a.ownerCount() << "\n";
    std::cout << "B owner count = " << b.ownerCount() << "\n\n";

    // mutate through B, show A sees it too
    std::cout << "--- Add \"hello\" through B ---\n";
    b.addWord("hello");
    std::cout << "A sees: ";
    a.printAll();
    std::cout << "B sees: ";
    b.printAll();
    std::cout << "(Both views updated because they share the vector)\n\n";

    // mutate through A, show B sees it too
    std::cout << "--- Remove \"hello\" through A ---\n";
    a.removeWord("hello");
    std::cout << "B sees: ";
    b.printAll();
    std::cout << "\n";

    // C is a separate collection, shouldn't affect A
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
