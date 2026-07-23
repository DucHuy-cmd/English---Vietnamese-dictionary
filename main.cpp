#include "Dictionary.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== DICTIONARY PROJECT INITIALIZATION ===\n";
    cout << "[+] Successfully set up TrieNode structure.\n";
    cout << "[+] Implemented insert() function.\n\n";
    
    // Create an object to test the syntax and logic
    Trie dictionary;
    dictionary.insert("hello", "xin chao");
    dictionary.insert("developer", "lap trinh vien");

    cout << "> Test: Successfully inserted 2 words into the Trie!\n";
    cout << "> TODO: Develop exact lookup feature (searchExact) in the next commit...\n";

    return 0;
}
