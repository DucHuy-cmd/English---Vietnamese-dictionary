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

    void dfsCollect(TrieNode* node, string& currentWord,
                     vector<pair<string, string>>& results,
                     int limit) const;
                     
    // Hai ham ho tro xoa tu
    bool removeHelper(TrieNode* node, const string& word, int depth);
    bool isEmpty(TrieNode* node) const;

public:
    Trie();
    ~Trie() = default;

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    bool loadData(const string& filename);
    void insert(const string& word, const string& meaning);
    bool searchExact(const string& word, string& outMeaning) const;
    
    // Ham xoa tu
    bool removeWord(const string& word);
    
    vector<pair<string, string>> getPrefixSuggestions(
        const string& prefix, int limit) const;

    int calculateLevenshtein(const string& s1, const string& s2) const;

    vector<pair<string, int>> getSpellingSuggestions(
        const string& wrongWord, int limit) const;

    size_t wordCount() const { return allWords.size(); }
};
