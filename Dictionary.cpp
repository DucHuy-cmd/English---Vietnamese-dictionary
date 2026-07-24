#include "Dictionary.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

using namespace std;

#ifdef _WIN32
    #include <conio.h>
    static int readOneChar() {
        return _getch();
    }
#else
    #include <termios.h>
    #include <unistd.h>
    static int readOneChar() {
        struct termios oldSettings, newSettings;

        if (tcgetattr(STDIN_FILENO, &oldSettings) != 0) {
            return getchar();
        }

        newSettings = oldSettings;
        newSettings.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

        int ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
        return ch;
    }
#endif

static void clearScreen() {
#ifdef _WIN32
    if (system("cls") != 0) {}
#else
    if (system("clear") != 0) {}
#endif
}

Trie::Trie() {
    root = unique_ptr<TrieNode>(new TrieNode());
}

string Trie::normalize(const string& raw) {
    string result = raw;

    size_t start = 0;
    while (start < result.size() &&
           isspace(static_cast<unsigned char>(result[start]))) {
        ++start;
    }

    size_t end = result.size();
    while (end > start &&
           isspace(static_cast<unsigned char>(result[end - 1]))) {
        --end;
    }

    result = result.substr(start, end - start);

    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return tolower(c); });

    return result;
}

void Trie::insert(const string& word, const string& meaning) {
    string cleanWord = normalize(word);
    if (cleanWord.empty()) return;

    TrieNode* current = root.get();

    for (char ch : cleanWord) {
        if (ch < 'a' || ch > 'z') return;

        int index = ch - 'a';
        if (!current->children[index]) {
            current->children[index] = unique_ptr<TrieNode>(new TrieNode());
        }
        current = current->children[index].get();
    }

    current->isEndOfWord = true;
    current->meaning = meaning;
    allWords.emplace_back(cleanWord, meaning);
}

bool Trie::loadData(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Loi] Khong the mo file: " << filename << endl;
        return false;
    }

    allWords.reserve(30000);

    string line;
    int lineNumber = 0;
    int successCount = 0;

    while (getline(file, line)) {
        ++lineNumber;

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) continue;

        size_t colonPos = line.find(':');
        if (colonPos == string::npos) {
            cerr << "[Canh bao] Dong " << lineNumber
                 << " sai dinh dang, bo qua: " << line << endl;
            continue;
        }

        string word = line.substr(0, colonPos);
        string meaning = line.substr(colonPos + 1);

        word = normalize(word);

        size_t mStart = meaning.find_first_not_of(" \t");
        size_t mEnd = meaning.find_last_not_of(" \t\r\n");
        meaning = (mStart != string::npos && mEnd != string::npos)
                    ? meaning.substr(mStart, mEnd - mStart + 1)
                    : "";

        if (word.empty() || meaning.empty()) continue;

        insert(word, meaning);
        ++successCount;
    }

    file.close();
    cout << "[Thong bao] Da nap " << successCount
              << " tu vung tu file '" << filename << "'." << endl;
    return true;
}

bool Trie::searchExact(const string& word, string& outMeaning) const {
    string cleanWord = normalize(word);
    TrieNode* current = root.get();

    for (char ch : cleanWord) {
        if (ch < 'a' || ch > 'z') return false;
        int index = ch - 'a';
        if (!current->children[index]) return false;
        current = current->children[index].get();
    }

    if (current->isEndOfWord) {
        outMeaning = current->meaning;
        return true;
    }
    return false;
}

void Trie::dfsCollect(TrieNode* node, string& currentWord,
                         vector<pair<string, string>>& results,
                         int limit) const {
    if (static_cast<int>(results.size()) >= limit) return;

    if (node->isEndOfWord) {
        results.emplace_back(currentWord, node->meaning);
        if (static_cast<int>(results.size()) >= limit) return;
    }

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (node->children[i]) {
            currentWord.push_back(static_cast<char>('a' + i));
            dfsCollect(node->children[i].get(), currentWord, results, limit);
            currentWord.pop_back();

            if (static_cast<int>(results.size()) >= limit) return;
        }
    }
}

vector<pair<string, string>> Trie::getPrefixSuggestions(
    const string& prefix, int limit) const {

    vector<pair<string, string>> results;
    string cleanPrefix = normalize(prefix);

    TrieNode* current = root.get();
    for (char ch : cleanPrefix) {
        if (ch < 'a' || ch > 'z') return results;
        int index = ch - 'a';
        if (!current->children[index]) return results;
        current = current->children[index].get();
    }

    string wordBuilder = cleanPrefix;
    dfsCollect(current, wordBuilder, results, limit);
    return results;
}

int Trie::calculateLevenshtein(const string& s1, const string& s2) {
    const size_t n = s1.size();
    const size_t m = s2.size();
    if (n == 0) return static_cast<int>(m);
    if (m == 0) return static_cast<int>(n);

    vector<int> previousRow(m + 1);
    vector<int> currentRow(m + 1);

    for (size_t j = 0; j <= m; ++j) previousRow[j] = static_cast<int>(j);

    for (size_t i = 1; i <= n; ++i) {
        currentRow[0] = static_cast<int>(i);
        for (size_t j = 1; j <= m; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                currentRow[j] = previousRow[j - 1];
            } else {
                int deleteCost = previousRow[j] + 1;
                int insertCost = currentRow[j - 1] + 1;
                int replaceCost = previousRow[j - 1] + 1;
                currentRow[j] = min({deleteCost, insertCost, replaceCost});
            }
        }
        swap(previousRow, currentRow);
    }
    return previousRow[m];
}

vector<pair<string, int>> Trie::getSpellingSuggestions(
    const string& wrongWord, int limit) const {

    string cleanWrong = normalize(wrongWord);
    const int wrongLen = static_cast<int>(cleanWrong.size());
    const int maxAllowedDistance = max(2, wrongLen / 3 + 1);

    vector<pair<string, int>> candidates;
    candidates.reserve(allWords.size());

    for (const auto& entry : allWords) {
        const string& dictWord = entry.first;
        const int dictLen = static_cast<int>(dictWord.size());

        if (abs(dictLen - wrongLen) > maxAllowedDistance) continue;

        int distance = calculateLevenshtein(cleanWrong, dictWord);
        if (distance <= maxAllowedDistance) {
            candidates.emplace_back(dictWord, distance);
        }
    }

    int actualLimit = min(limit, static_cast<int>(candidates.size()));
    partial_sort(
        candidates.begin(), candidates.begin() + actualLimit, candidates.end(),
        [](const pair<string, int>& a, const pair<string, int>& b) {
            if (a.second != b.second) return a.second < b.second;
            return a.first.size() < b.first.size();
        });
    candidates.resize(actualLimit);
    return candidates;
}
