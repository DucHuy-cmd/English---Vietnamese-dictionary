#pragma once
#include <string>
#include <vector>
#include <utility>
#include <memory>

using namespace std;

constexpr int ALPHABET_SIZE = 26;

struct TrieNode {
    unique_ptr<TrieNode> children[ALPHABET_SIZE];
    bool isEndOfWord;
    string meaning;

    TrieNode() : isEndOfWord(false) {}
};

class Trie {
private:
    unique_ptr<TrieNode> root;
    
    vector<pair<string, string>> allWords;

    static string normalize(const string& raw);

public:
    Trie();
    ~Trie() = default;
    void insert(const string& word, const string& meaning);
    bool searchExact(const string& word, string& outMeaning) const;

};
