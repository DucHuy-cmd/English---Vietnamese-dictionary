#include "Dictionary.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

using namespace std;
// ============================================================================
// MOI: Chuyen doi ky tu <-> chi so trong mang children[], mo rong tu 26 chu
// cai (a-z) sang ho tro them dau cach va dau gach ngang cho cum tu nhieu tieng.
// Dat static (chi dung noi bo file nay), khong can khai bao trong Dictionary.h.
// ============================================================================
static int charToIndex(char ch) {
    if (ch >= 'a' && ch <= 'z') return ch - 'a';
    if (ch == ' ') return 26;
    if (ch == '-') return 27;
    return -1; // Ky tu khong hop le
}

static char indexToChar(int index) {
    if (index >= 0 && index <= 25) return static_cast<char>('a' + index);
    if (index == 26) return ' ';
    return '-'; // index == 27
}

// ============================================================================
// Initialize / normalize string
// ============================================================================
Trie::Trie() {
    root = unique_ptr<TrieNode>(new TrieNode());
}

// Trim leading/trailing spaces + convert to lowercase -> ensure uniform data (dung cho TU KHOA)
string Trie::normalize(const string& raw) {
    string result = raw;

    size_t start = 0;
    while (start < result.size() && isspace(static_cast<unsigned char>(result[start]))) {
        ++start;
    }
    size_t end = result.size();
    while (end > start && isspace(static_cast<unsigned char>(result[end - 1]))) {
        --end;
    }
    result = result.substr(start, end - start);

    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });

    return result;
}

// MOI: chi trim 2 dau, KHONG lowercase -> dung cho nghia / tu loai / vi du
// (giu nguyen tieng Viet co dau, hoa/thuong nhu nguoi dung nhap)
string Trie::trim(const string& raw) {
    size_t start = raw.find_first_not_of(" \t\r\n");
    size_t end = raw.find_last_not_of(" \t\r\n");
    if (start == string::npos || end == string::npos) return "";
    return raw.substr(start, end - start + 1);
}

// ============================================================================
// insert() - O(L)
// MOI: them 2 tham so pos + example (mac dinh rong, khai bao o Dictionary.h)
// ============================================================================
void Trie::insert(const string& word, const string& meaning,
                   const string& pos, const string& example) {
    string cleanWord = normalize(word);
    if (cleanWord.empty()) return;

    TrieNode* current = root.get();
    for (char ch : cleanWord) {
        int index = charToIndex(ch);
        if (index == -1) return; // Ky tu khong hop le (khong phai a-z, space, '-') -> bo qua ca tu

        if (!current->children[index]) {
            current->children[index] = unique_ptr<TrieNode>(new TrieNode());
        }
        current = current->children[index].get();
    }

    if (current->isEndOfWord) {
        // Word already exists -> only update new meaning/pos/example in allWords (do not create duplicate record)
        for (auto& entry : allWords) {
            if (entry.word == cleanWord) {
                entry.meaning = meaning;
                entry.pos = pos;
                entry.example = example;
                break;
            }
        }
    } else {
        // Completely new word -> add record to flat list
        allWords.push_back(WordEntry{cleanWord, meaning, pos, example});
    }

    current->isEndOfWord = true;
    current->meaning = meaning;
    current->pos = pos;
    current->example = example;
}

// ============================================================================
// loadData() - read file dinh dang moi: "word|pos|meaning|example"
// (pos va example co the de trong, vd: "apple||qua tao|")
// ============================================================================
bool Trie::loadData(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Error] Cannot open file: " << filename << endl;
        return false;
    }

    allWords.reserve(1000);

    string line;
    int lineNumber = 0;
    int successCount = 0;

    while (getline(file, line)) {
        ++lineNumber;

        // Remove trailing '\r' (file saved on Windows but read on Linux, or vice versa)
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (line.empty()) continue;

        // Tach thu cong theo dau '|' (khong dung stringstream de tranh mat truong rong lien tiep, vd "hello||xin chao|")
        vector<string> parts;
        size_t start = 0;
        while (true) {
            size_t barPos = line.find('|', start);
            if (barPos == string::npos) {
                parts.push_back(line.substr(start));
                break;
            }
            parts.push_back(line.substr(start, barPos - start));
            start = barPos + 1;
        }

        // Toi thieu phai co word|pos|meaning ; example la tuy chon (co the thieu cot thu 4)
        if (parts.size() < 3) {
            cerr << "[Warning] Line " << lineNumber << " has invalid format, skipping: " << line << endl;
            continue;
        }

        string word = normalize(parts[0]);
        string pos_ = trim(parts[1]);
        string meaning = trim(parts[2]);
        string example = (parts.size() > 3) ? trim(parts[3]) : "";

        if (word.empty() || meaning.empty()) continue;

        insert(word, meaning, pos_, example);
        ++successCount;
    }

    file.close();
    cout << "[Notice] Loaded " << successCount << " vocabulary words from file '" << filename << "'." << endl;
    return true;
}

// ============================================================================
// searchExact() - O(L)
// ============================================================================
bool Trie::searchExact(const string& word, string& outMeaning) const {
    string outPos, outExample;
    return searchExact(word, outMeaning, outPos, outExample);
}

// MOI: ban day du, tra ve them Tu loai + Vi du cho man hinh tra tu chinh
bool Trie::searchExact(const string& word, string& outMeaning,
                        string& outPos, string& outExample) const {
    string cleanWord = normalize(word);
    TrieNode* current = root.get();

   for (char ch : cleanWord) {
        int index = charToIndex(ch);
        if (index == -1) return false;
        if (!current->children[index]) return false;
        current = current->children[index].get();
    }

    if (current->isEndOfWord) {
        outMeaning = current->meaning;
        outPos = current->pos;
        outExample = current->example;
        return true;
    }
    return false;
}

// ============================================================================
// Autocomplete: getPrefixSuggestions() + dfsCollect() helper
// (Chi can word + meaning de hien thi danh sach goi y, khong doi)
// ============================================================================
void Trie::dfsCollect(TrieNode* node, string& currentWord,
                       vector<pair<string, string>>& results, int limit) const {
    if (static_cast<int>(results.size()) >= limit) return;

    if (node->isEndOfWord) {
        results.emplace_back(currentWord, node->meaning);
        if (static_cast<int>(results.size()) >= limit) return;
    }

    // Traverse a->z for natural alphabetical order
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (node->children[i]) {
            currentWord.push_back(indexToChar(i));
            dfsCollect(node->children[i].get(), currentWord, results, limit);
            currentWord.pop_back();

            if (static_cast<int>(results.size()) >= limit) return;
        }
    }
}
vector<pair<string, string>> Trie::getPrefixSuggestions(const string& prefix, int limit) const {
    vector<pair<string, string>> results;
    string cleanPrefix = normalize(prefix);

    TrieNode* current = root.get();
     for (char ch : cleanPrefix) {
        int index = charToIndex(ch);
        if (index == -1) return results;
        if (!current->children[index]) return results;
        current = current->children[index].get();
    }

    string wordBuilder = cleanPrefix;
    dfsCollect(current, wordBuilder, results, limit);
    return results;
}

// ============================================================================
// Levenshtein Distance - Dynamic programming, memory optimization O(min(n,m))
// ============================================================================
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

// getSpellingSuggestions() - pruning heuristic: ignore candidates with lengths
// differing too much from the wrong word, avoiding O(n*m) DP on
// the entire dictionary when the number of words is large.
vector<pair<string, int>> Trie::getSpellingSuggestions(const string& wrongWord, int limit) const {
    string cleanWrong = normalize(wrongWord);
    const int wrongLen = static_cast<int>(cleanWrong.size());
    const int maxAllowedDistance = max(2, wrongLen / 3 + 1);

    vector<pair<string, int>> candidates;
    candidates.reserve(allWords.size());

    for (const auto& entry : allWords) {
        const string& dictWord = entry.word;
        const int dictLen = static_cast<int>(dictWord.size());

        // Pruning heuristic based on length difference
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

// ============================================================================
// removeWord() - remove 1 word from Trie and prune unused nodes
// ============================================================================
bool Trie::isEmpty(TrieNode* node) const {
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (node->children[i]) return false;
    }
    return true;
}

bool Trie::removeHelper(TrieNode* node, const string& word, int depth) {
    if (!node) return false;

    if (depth == static_cast<int>(word.size())) {
        if (!node->isEndOfWord) return false;
        node->isEndOfWord = false;
        return isEmpty(node); // Notify parent node whether this node can be pruned
    }

    // This function is only called after searchExact() has confirmed the word exists,
    // so word[depth] is guaranteed to be a valid a-z character.
    int index = charToIndex(word[depth]);
    if (index == -1 || !node->children[index]) return false;

    bool shouldDeleteChild = removeHelper(node->children[index].get(), word, depth + 1);
    if (shouldDeleteChild) {
        node->children[index].reset();
    }

    return !node->isEndOfWord && isEmpty(node);
}

bool Trie::removeWord(const string& word) {
    string cleanWord = normalize(word);
    if (cleanWord.empty()) return false;

    string tempMeaning;
    if (!searchExact(cleanWord, tempMeaning)) {
        return false; // Word does not exist -> nothing to delete
    }

    removeHelper(root.get(), cleanWord, 0);

    auto it = remove_if(allWords.begin(), allWords.end(),
                         [&](const WordEntry& e) { return e.word == cleanWord; });
    if (it != allWords.end()) {
        allWords.erase(it, allWords.end());
    }

    return true;
}
