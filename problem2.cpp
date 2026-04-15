// Builds a map<string, set<int>> mapping each word to the
// set of line numbers on which it appears. Then lets the user
// query interactively until EOF

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <cctype>

// same normalize helper (lowercase + strip punctuation)
static std::string normalize(std::string w) {
    // Strip punctuation characters using the erase-remove idiom.
    w.erase(std::remove_if(w.begin(), w.end(),
                           [](unsigned char c) { return std::ispunct(c); }),
            w.end());
    // Lowercase everything that's left
    std::transform(w.begin(), w.end(), w.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return w;
}

int main(int argc, char* argv[]) {
    // need exactly one argument: the filename to index
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <textfile>\n";
        return 1;
    }
    // open the file for reading, bail out if it doesn't exist
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
        ++lineNum; // lines are 1-indexed
        
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            word = normalize(word);
            if (!word.empty()) {
                // index[word] auto-creates an empty set<int> if this is
                // the first time we've seen this word. Then .insert() adds
                // the current line number (or ignores it if already there)
                index[word].insert(lineNum);
            }
        }
    }

    // Give the user a quick summary of what we indexed, then start accepting queries.
    std::cout << "Indexed " << index.size() << " unique words across "
              << lineNum << " lines.\n";
    std::cout << "(Press Ctrl+D / Ctrl+Z to quit)\n\n";

    // interactive query loop, stops on EOF
    std::string query;
    while (true) {
        std::cout << "Enter word to search: ";
        if (!(std::cin >> query)) {
            std::cout << "\nGoodbye.\n";
            break;
        }

        // Normalize the query the same way we normalized the file
        query = normalize(query);
        // map::find returns an iterator to the match, or end() if missing.
        // We use find instead of operator[] because operator[] would
        // auto-create an empty entry for a word that isn't in the map,
        // which would be a side effect
        auto it = index.find(query);
        if (it == index.end()) {
            std::cout << "\"" << query << "\" was not found in the file.\n";
        } else {
             // it->first is the key (the word), it->second is the set<int>.
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
