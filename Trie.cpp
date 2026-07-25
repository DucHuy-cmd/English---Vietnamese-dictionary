#include "Dictionary.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

using namespace std;

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
        if (ch < 'a' || ch > 'z') return; // Invalid data -> ignore the whole word

        int index = ch - 'a';
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
        if (ch < 'a' || ch > 'z') return false;
        int index = ch - 'a';
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
            currentWord.push_back(static_cast<char>('a' + i));
            dfsCollect(node->children[i].get(), currentWord, results, limit);
            currentWord.pop_back();

            if (static_cast<int>(results.size()) >= limit) return;
        }
    }
}
