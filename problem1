// Problem 1 - Text Processing with Generic Algorithms
//
// Joseph
//
// Reads a text file, normalizes each word (lowercase + no punctuation),
// then uses STL algorithms to sort, dedupe, and count long words.
// No range-based for loops or operator[] are used for data processing.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

// helper: lowercase a word and strip any punctuation characters
static std::string normalize(std::string w) {
    // remove punctuation with remove_if + erase
    w.erase(std::remove_if(w.begin(), w.end(),
                           [](unsigned char c) { return std::ispunct(c); }),
            w.end());

    // convert remaining characters to lowercase
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
        std::cerr << "Error: could not open file \"" << argv[1] << "\"\n";
        return 1;
    }

    // read every whitespace-separated token into a vector
    std::vector<std::string> words;
    std::string token;
    while (in >> token) {
        words.push_back(token);
    }

    // normalize the whole vector in place using std::transform
    std::transform(words.begin(), words.end(), words.begin(), normalize);

    // if a token was pure punctuation it is now an empty string -- drop those
    words.erase(std::remove_if(words.begin(), words.end(),
                               [](const std::string& s) { return s.empty(); }),
                words.end());

    // remember the total before we dedupe
    const std::size_t totalWords = words.size();

    // sort alphabetically
    std::sort(words.begin(), words.end());

    // remove adjacent duplicates (std::unique needs a sorted range)
    auto newEnd = std::unique(words.begin(), words.end());
    words.erase(newEnd, words.end());

    const std::size_t uniqueWords = words.size();

    // count unique words with length >= 6
    const auto longWordCount =
        std::count_if(words.begin(), words.end(),
                      [](const std::string& s) { return s.size() >= 6; });

    std::cout << "Total number of words:           " << totalWords    << "\n";
    std::cout << "Number of unique words:          " << uniqueWords   << "\n";
    std::cout << "Number of words with length >=6: " << longWordCount << "\n";

    return 0;
}
