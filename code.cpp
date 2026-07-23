
void Trie::dfsCollect(TrieNode* node, string& currentWord,
                      vector<pair<string, string>>& results,
                      int limit) const {
    if (!node || results.size() >= limit) return;
    
    if (node->isEndOfWord) {
        results.push_back({currentWord, node->meaning});
        if (results.size() >= limit) return; 
    }

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (node->children[i]) {
            currentWord.push_back(static_cast<char>('a' + i));
            dfsCollect(node->children[i].get(), currentWord, results, limit);
            currentWord.pop_back(); 
        }
    }
}

vector<pair<string, string>> Trie::getPrefixSuggestions(
    const string& prefix, int limit) const {

    vector<pair<string, string>> results;
    string normalizedPrefix = normalize(prefix); 
    if (normalizedPrefix.empty()) return results;

    TrieNode* current = root.get();

    for (char c : normalizedPrefix) {
        int index = c - 'a';
        if (index < 0 || index >= ALPHABET_SIZE || !current->children[index]) {
            return results; 
        }
        current = current->children[index].get();
    }
    string currentWord = normalizedPrefix;
    dfsCollect(current, currentWord, results, limit);

    return results;
}
