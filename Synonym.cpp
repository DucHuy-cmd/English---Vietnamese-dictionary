#include "Synonym.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

using namespace std;

// ============================================================================
// Chuan hoa chuoi
// ============================================================================
string SynonymDictionary::trim(const string& raw) {
    size_t start = 0;
    while (start < raw.size() && isspace(static_cast<unsigned char>(raw[start]))) ++start;
    size_t end = raw.size();
    while (end > start && isspace(static_cast<unsigned char>(raw[end - 1]))) --end;
    return raw.substr(start, end - start);
}

string SynonymDictionary::normalizeKey(const string& raw) {
    string result = trim(raw);
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return result;
}

// ============================================================================
// loadData() - doc file dinh dang "word|meaning|syn1, syn2, syn3" moi dong
// ============================================================================
bool SynonymDictionary::loadData(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Loi] Khong the mo file: " << filename << endl;
        return false;
    }

    string line;
    int lineNumber = 0;
    int successCount = 0;

    while (getline(file, line)) {
        ++lineNumber;

        // Loai bo '\r' cuoi dong (file luu tren Windows nhung doc tren Linux)
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (line.empty()) continue;

        // Buoc 1: tach dong thanh 3 phan theo dau '|'
        // Dung stringstream + getline(ss, token, '|') de tach an toan, khong
        // gioi han so ky tu moi phan (khac voi find() thu cong tung dau '|').
        stringstream lineStream(line);
        string wordPart, meaningPart, synonymsPart;

        if (!getline(lineStream, wordPart, '|')) {
            cerr << "[Canh bao] Dong " << lineNumber << " thieu du 3 phan, bo qua: " << line << endl;
            continue;
        }
        if (!getline(lineStream, meaningPart, '|')) {
            cerr << "[Canh bao] Dong " << lineNumber << " thieu phan nghia, bo qua: " << line << endl;
            continue;
        }
        if (!getline(lineStream, synonymsPart)) { // Phan con lai (co the chua dau ',')
            cerr << "[Canh bao] Dong " << lineNumber << " thieu phan tu dong nghia, bo qua: " << line << endl;
            continue;
        }

        string word = normalizeKey(wordPart); // Tu khoa -> chuan hoa (lowercase) de tra cuu
        string meaning = trim(meaningPart);   // Nghia tieng Viet -> chi trim, giu nguyen dau

        if (word.empty() || meaning.empty()) {
            cerr << "[Canh bao] Dong " << lineNumber << " tu hoac nghia rong, bo qua: " << line << endl;
            continue;
        }

        // Buoc 2: tach phan tu dong nghia theo dau ',', trim tung tu
        SynonymData data;
        data.meaning = meaning;

        stringstream synStream(synonymsPart);
        string oneSynonym;
        while (getline(synStream, oneSynonym, ',')) {
            oneSynonym = trim(oneSynonym);
            if (!oneSynonym.empty()) {
                data.words.push_back(oneSynonym);
            }
        }

        if (data.words.empty()) {
            cerr << "[Canh bao] Dong " << lineNumber << " khong co tu dong nghia nao hop le, bo qua: " << line << endl;
            continue;
        }

        synonymMap[word] = data; // Ghi de neu tu da ton tai (file co dong trung)
        ++successCount;
    }

    file.close();
    cout << "[Thong bao] Da nap " << successCount << " muc tu dong nghia tu file '" << filename << "'." << endl;
    return true;
}

// ============================================================================
// findSynonyms() - tra cuu O(1) trung binh nho unordered_map
// ============================================================================
bool SynonymDictionary::findSynonyms(const string& word, SynonymData& outData) const {
    string key = normalizeKey(word);
    auto it = synonymMap.find(key);
    if (it == synonymMap.end()) {
        return false;
    }
    outData = it->second;
    return true;
}
