#include "Dictionary.h"
#include <iostream>

using namespace std;

// ============================================================================
// SearchHistory: danh sach lien ket doi, hoat dong kieu LRU Cache
//   - Tu tra cuu MOI NHAT luon nam o HEAD
//   - Tu LAU NHAT nam o TAIL, bi xoa khi vuot qua MAX_HISTORY (10 tu)
//   - Tra lai 1 tu da co san -> "nhac" node do len HEAD thay vi tao ban ghi moi
// ============================================================================

// Thao 1 node ra khoi danh sach (noi lai prev/next cua 2 node lan can)
// nhung KHONG delete - dung khi can chuyen vi tri node ma khong cap phat lai.
void SearchHistory::unlink(HistoryNode* node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        tail = node->prev;
    }

    node->prev = nullptr;
    node->next = nullptr;
}

// Chen 1 node da co san (chua nam trong danh sach) vao vi tri HEAD
void SearchHistory::pushToFront(HistoryNode* node) {
    node->prev = nullptr;
    node->next = head;

    if (head) head->prev = node;
    head = node;

    if (!tail) tail = node;
}

void SearchHistory::addWord(const string& word, const string& meaning) {
    // Tim xem tu nay da co trong lich su chua
    HistoryNode* existing = nullptr;
    for (HistoryNode* cur = head; cur != nullptr; cur = cur->next) {
        if (cur->word == word) {
            existing = cur;
            break;
        }
    }

    if (existing) {
        if (existing == head) return; // Da o dau roi, khong can lam gi them
        unlink(existing);
        existing->meaning = meaning; // Cap nhat nghia moi nhat (phong khi tu dien thay doi)
        pushToFront(existing);
        return;
    }

    // Tu hoan toan moi -> tao node va chen vao HEAD
    HistoryNode* newNode = new HistoryNode(word, meaning);
    pushToFront(newNode);
    ++currentSize;

    // Vuot qua gioi han -> xoa node cu nhat o TAIL
    if (currentSize > MAX_HISTORY) {
        HistoryNode* oldTail = tail;
        unlink(oldTail);
        delete oldTail;
        --currentSize;
    }
}

void SearchHistory::printHistory() const {
    if (head == nullptr) {
        cout << "Search history is empty." << endl;
        return;
    }

    cout << "\n=== SEARCH HISTORY (newest -> oldest) ===" << endl;
    int index = 1;
    for (HistoryNode* cur = head; cur != nullptr; cur = cur->next, ++index) {
        cout << "  " << index << ". " << cur->word << " - " << cur->meaning << endl;
    }
}

// MOI (cho GUI): duyet linked-list Y HET printHistory() o tren, chi khac
// buoc cuoi la push_back vao vector thay vi cout ra console.
vector<pair<string, string>> SearchHistory::getHistoryList() const {
    vector<pair<string, string>> result;
    for (HistoryNode* cur = head; cur != nullptr; cur = cur->next) {
        result.emplace_back(cur->word, cur->meaning);
    }
    return result;
}

// Giai phong toan bo node con lai -> tranh Memory Leak
SearchHistory::~SearchHistory() {
    HistoryNode* cur = head;
    while (cur != nullptr) {
        HistoryNode* next = cur->next;
        delete cur;
        cur = next;
    }
    head = nullptr;
    tail = nullptr;
    currentSize = 0;
}
