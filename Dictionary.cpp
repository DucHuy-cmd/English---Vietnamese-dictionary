#include "Dictionary.h"
#include <algorithm>
#include <cctype>

using namespace std;

// Khoi tao cay Trie
Trie::Trie() {
    root = unique_ptr<TrieNode>(new TrieNode());
}

// Chuan hoa chuoi (Tam thoi chi lam ban basic: chuyen chu thuong)
string Trie::normalize(const string& raw) {
    string result = raw;
    transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return tolower(c); });
    return result;
}

// Them tu vao tu dien
void Trie::insert(const string& word, const string& meaning) {
    string cleanWord = normalize(word);
    if (cleanWord.empty()) return;

    TrieNode* current = root.get();

    // Duyet qua tung ky tu de dua vao cay
    for (char ch : cleanWord) {
        if (ch < 'a' || ch > 'z') return;

        int index = ch - 'a';
        if (!current->children[index]) {
            current->children[index] = unique_ptr<TrieNode>(new TrieNode());
        }
        current = current->children[index].get();
    }

    // Neu tu chua ton tai, luu vao danh sach de dung sau
    if (!current->isEndOfWord) {
        allWords.emplace_back(cleanWord, meaning);
    }

    // Danh dau node ket thuc va gan nghia
    current->isEndOfWord = true;
    current->meaning = meaning;
}
