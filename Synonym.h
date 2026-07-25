#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// ============================================================================
// Synonym.h - Tu dien dong nghia, doc du lieu tu file "dongnghia.txt"
// Dinh dang moi 3 phan, ngan cach boi '|':
//     Tu_tieng_Anh | Nghia_tieng_Viet | tu1, tu2, tu3
// Vi du:
//     happy|vui ve, hanh phuc|joyful, cheerful, glad
// ============================================================================

// 1 ban ghi day du cho 1 tu goc: nghia tieng Viet + danh sach tu dong nghia.
struct SynonymData {
    std::string meaning;              // Nghia tieng Viet cua tu goc
    std::vector<std::string> words;   // Danh sach tu dong nghia (tieng Anh)
};

class SynonymDictionary {
private:
    // Key = tu goc (da chuan hoa), Value = nghia + danh sach dong nghia.
    // unordered_map -> tra cuu trung binh O(1), phu hop vi day la tra cuu
    // theo tu khoa chinh xac (khong can autocomplete/prefix nhu Trie).
    std::unordered_map<std::string, SynonymData> synonymMap;

    // Chuan hoa TU KHOA (trim + lowercase) truoc khi tra/luu vao map,
    // dam bao "Happy", " happy ", "HAPPY" deu tra ra cung 1 ket qua.
    static std::string normalizeKey(const std::string& raw);

    // Chi trim 2 dau, KHONG lowercase -> dung cho nghia tieng Viet va
    // tung tu dong nghia (giu nguyen dau tieng Viet / chu hoa nguyen ban).
    static std::string trim(const std::string& raw);

public:
    SynonymDictionary() = default;

    // Doc file dinh dang "word|meaning|syn1, syn2, syn3" moi dong.
    // Tra ve false neu khong mo duoc file; cac dong sai dinh dang se bi
    // bo qua (co canh bao ra stderr) thay vi lam crash chuong trinh.
    bool loadData(const std::string& filename);

    // Tra cuu 1 tu: neu co trong tu dien, dien du lieu vao outData va tra
    // ve true; neu khong co, tra ve false (outData khong bi dong toi).
    bool findSynonyms(const std::string& word, SynonymData& outData) const;
};
