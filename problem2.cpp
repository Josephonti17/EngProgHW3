// Problem 2 - Associative Containers: Word Index
//
// Builds a map<string, set<int>> mapping each word to the
// set of line numbers on which it appears. Then lets the user
// query interactively until EOF.

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <cctype>

// same normalize helper as Problem 1 (lowercase + strip punctuation)
static std::string normalize(std::string w) {
    w.erase(std::remove_if(w.begin(), w.end(),
                           [](unsigned char c) { return std::ispunct(c); }),
            w.end());
    std::transform(w.begin(), w.end(), w.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return w;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <textfile>\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "Error: could not open \"" << argv[1] << "\"\n";
        return 1;
    }

    std::map<std::string, std::set<int>> index;

    // read the file line by line and track line numbers
    std::string line;
    int lineNum = 0;
    while (std::getline(in, line)) {
        ++lineNum;
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            word = normalize(word);
            if (!word.empty()) {
                index[word].insert(lineNum);
            }
        }
    }

    std::cout << "Indexed " << index.size() << " unique words across "
              << lineNum << " lines.\n";
    std::cout << "(Press Ctrl+D / Ctrl+Z to quit)\n\n";

    // interactive query loop -- stops on EOF
    std::string query;
    while (true) {
        std::cout << "Enter word to search: ";
        if (!(std::cin >> query)) {
            std::cout << "\nGoodbye.\n";
            break;
        }
        query = normalize(query);

        auto it = index.find(query);
        if (it == index.end()) {
            std::cout << "\"" << query << "\" was not found in the file.\n";
        } else {
            std::cout << query << " occurs on lines:";
            // set<int> is already sorted, so this prints in order
            for (int ln : it->second) {
                std::cout << " " << ln;
            }
            std::cout << "\n";
        }
    }

    return 0;
}
