#pragma once
#include <string>
#include <vector>
#include <utility>
#include <memory>

// ============================================================================
// CAC HANG SO DUNG CHUNG
// ============================================================================
constexpr int ALPHABET_SIZE = 26;        // 26 chu cai a-z
constexpr int QUIZ_QUEUE_CAPACITY = 10;  // So cau hoi toi da trong hang doi quiz
constexpr int MAX_STR_LEN = 100;         // Do dai toi da cho 1 tu / 1 dap an (char[])

// ============================================================================
// PHAN 1: TRIE ENGINE (khai bao - cai dat trong Trie.cpp)
// ============================================================================

// MOI: 1 ban ghi tu vung day du, dung cho allWords va tra ve ra ben ngoai class.
// Thay the cho pair<string,string> (word, meaning) truoc day de mang them
// 2 truong "Tu loai" (pos) va "Vi du" (example). Ca 2 truong co the de rong.
struct WordEntry {
    std::string word;
    std::string meaning;
    std::string pos;      // Tu loai, vd: "(n)", "(v)", "(adj)" - co the rong
    std::string example;  // Cau vi du - co the rong
};

struct TrieNode {
    std::unique_ptr<TrieNode> children[ALPHABET_SIZE];
    bool isEndOfWord;
    std::string meaning;
    std::string pos;       // MOI: tu loai cua tu nay (chi co y nghia khi isEndOfWord = true)
    std::string example;   // MOI: cau vi du cua tu nay

    TrieNode() : isEndOfWord(false) {}
};

class Trie {
private:
    std::unique_ptr<TrieNode> root;
    std::vector<WordEntry> allWords;

    // Trim 2 dau + chuyen thanh chu thuong -> dung cho TU KHOA (key) can dong nhat de tra cuu
    static std::string normalize(const std::string& raw);
    // MOI: chi trim 2 dau, KHONG lowercase -> dung cho nghia / tu loai / vi du (giu nguyen tieng Viet co dau)
    static std::string trim(const std::string& raw);

    void dfsCollect(TrieNode* node, std::string& currentWord,
                     std::vector<std::pair<std::string, std::string>>& results,
                     int limit) const;

    // Ho tro removeWord(): kiem tra 1 node co con nao khong
    bool isEmpty(TrieNode* node) const;
    // Ho tro removeWord(): de quy xoa tu va cat tia cac node thua
    bool removeHelper(TrieNode* node, const std::string& word, int depth);

public:
    Trie();
    ~Trie() = default;

    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    // Doc file dinh dang moi: "word|pos|meaning|example" (pos, example co the de trong)
    bool loadData(const std::string& filename);

    // MOI: them 2 tham so pos + example, co gia tri mac dinh rong.
    // Cac cho code cu goi insert(word, meaning) van bien dich binh thuong.
    void insert(const std::string& word, const std::string& meaning,
                const std::string& pos = "", const std::string& example = "");

    // Ban CU: giu nguyen cho nhung noi chi can nghia (vd: hien thi trong danh sach goi y)
    bool searchExact(const std::string& word, std::string& outMeaning) const;
    // MOI: ban day du 4 tham so, dung cho man hinh tra tu chinh de lay them pos + example
    bool searchExact(const std::string& word, std::string& outMeaning,
                      std::string& outPos, std::string& outExample) const;

    std::vector<std::pair<std::string, std::string>> getPrefixSuggestions(
        const std::string& prefix, int limit = 10) const;

    static int calculateLevenshtein(const std::string& s1, const std::string& s2);
    std::vector<std::pair<std::string, int>> getSpellingSuggestions(
        const std::string& wrongWord, int limit = 5) const;

    bool removeWord(const std::string& word);

    size_t wordCount() const { return allWords.size(); }
    const std::vector<WordEntry>& getAllWords() const { return allWords; }
};

// ============================================================================
// PHAN 2: DANH SACH LIEN KET DOI - LICH SU TRA CUU (khai bao - History.cpp)
// ============================================================================
struct HistoryNode {
    std::string word;
    std::string meaning;
    HistoryNode* prev;
    HistoryNode* next;

    HistoryNode(const std::string& w, const std::string& m)
        : word(w), meaning(m), prev(nullptr), next(nullptr) {}
};

class SearchHistory {
private:
    HistoryNode* head;
    HistoryNode* tail;
    int currentSize;
    static const int MAX_HISTORY = 10;

    void unlink(HistoryNode* node);
    void pushToFront(HistoryNode* node);

public:
    SearchHistory() : head(nullptr), tail(nullptr), currentSize(0) {}
    ~SearchHistory();

    SearchHistory(const SearchHistory&) = delete;
    SearchHistory& operator=(const SearchHistory&) = delete;

    void addWord(const std::string& word, const std::string& meaning);
    void printHistory() const; // Giu nguyen cho ban console (khong dung cho GUI)

    // MOI (cho GUI): tra ve du lieu thay vi in ra man hinh. Duyet linked-list
    // giong het printHistory(), chi khac o buoc cuoi la push_back thay vi cout.
    std::vector<std::pair<std::string, std::string>> getHistoryList() const;

    int size() const { return currentSize; }
};

// ============================================================================
// PHAN 3: CAC HAM PHU THUOC PLATFORM (khai bao - Platform.cpp)
// Gom het cac lenh phu thuoc he dieu hanh (Windows/Linux) vao 1 file duy nhat
// de cac module khac (Search.cpp, Quiz.cpp, main.cpp) khong can quan tam
// dang chay tren OS nao.
// ============================================================================
int readOneChar();      // Doc 1 ky tu tu ban phim, khong can Enter, khong hien ra man hinh
void clearScreen();     // Xoa man hinh console
void sleepMs(int milliseconds); // Tam dung chuong trinh (mili giay)

// ============================================================================
// PHAN 4: TINH NANG TRA CUU NANG CAO (khai bao - Search.cpp)
// ============================================================================
// In danh sach goi y (tu - nghia) ra man hinh, dung chung cho nhieu menu
void printSuggestions(const std::vector<std::pair<std::string, std::string>>& list);

// Live Search: nguoi dung go toi dau, goi y hien ra toi do (dua tren Trie)
// Khi Enter va tim thay nghia chinh xac -> tu dong luu vao SearchHistory
void liveSearchUI(Trie& dictionary, SearchHistory& history);

// ============================================================================
// PHAN 5: QUIZ TU VUNG - QUEUE (mang vong) + STACK (linked list) (Quiz.cpp)
// ============================================================================

// Hang doi cau hoi trac nghiem (Queue cai dat bang mang vong, suc chua co dinh)
struct queueVocabulary {
    char Voc[MAX_STR_LEN];
    char A[MAX_STR_LEN];
    char B[MAX_STR_LEN];
    char C[MAX_STR_LEN];
    char D[MAX_STR_LEN];
    char correctAnswer[MAX_STR_LEN];
};
using qV = queueVocabulary;

// Lich su cac cau da tra loi (Stack cai dat bang danh sach lien ket don)
struct stackVocabulary {
    char Voc[MAX_STR_LEN];
    char correctAnwser[MAX_STR_LEN];
    char yourAnwser[MAX_STR_LEN];
    stackVocabulary* next;
};
using sV = stackVocabulary;

// MOI (cho GUI): goi cau hoi/lich su duoi dang std::string thay vi char[],
// tien cho ImGui hien thi, khong lien quan gi den thuat toan Queue/Stack ben duoi.
struct QuizQuestion {
    std::string word, A, B, C, D;
};
struct QuizHistoryEntry {
    std::string word, correctAnswer, yourAnswer;
};

class Vocabulary {
private:
    qV examEnglish[QUIZ_QUEUE_CAPACITY];
    sV* top;    // Dinh stack (lich su tra loi)
    int size;   // So phan tu dang co trong stack
    int front;  // Chi so dau hang doi (cau hoi hien tai)
    int rear;   // Chi so cuoi hang doi
    int num;    // So cau hoi dang co trong hang doi

    Trie* dictRef;  // Con tro toi tu dien (gan 1 lan qua setDictionary)
    int loadIndex;  // Vi tri tu tiep theo se nap tu Trie vao hang doi

    // MOI (tinh nang Tro giup): danh dau dap an nao (0=A,1=B,2=C,3=D) da bi
    // loai boi useHint() cho CAU HOI HIEN TAI. Khoi tao san = false nho
    // in-class initializer, khong can dong nao trong constructor .cpp.
    bool hintEliminated[4] = { false, false, false, false };

    sV* createNode(char Voca[], char CA[], char YA[]);
    void push(char Voca[], char CA[], char YA[]);
    void enQueue(char Voca[], char A[], char B[], char C[], char D[], char CA[]);
    void deQueue();
    void shuffle();

public:
    Vocabulary();
    ~Vocabulary();

    Vocabulary(const Vocabulary&) = delete;
    Vocabulary& operator=(const Vocabulary&) = delete;

    void setDictionary(Trie& dict);
    void moduleLoadFromTrie();      // Nap 1 cau hoi moi tu Trie vao hang doi
    void moduleSaved(char Voca[], char CA[], char YA[]); // Luu cau vua tra loi vao stack
    void moduleExam();              // Vong lap lam bai kiem tra (ban console, giu nguyen)
    void vAHistory();               // Xem lich su tra loi (ban console, giu nguyen)

    bool hasQuestions() const { return num > 0; }

    // ==== MOI (cho GUI) - chi goi lai cac ham/thanh phan da co, khong doi thuat toan ====

    // Nap day hang doi cau hoi (toi da QUIZ_QUEUE_CAPACITY), goi 1 lan khi vao tab Quiz.
    // Y het vong lap dau cua moduleExam(), tach rieng de GUI chu dong goi truoc.
    void ensureQuestionsLoaded();

    // Tron cau hoi/dap an (goi shuffle() co san) va tra ve cau hoi dang hien tai.
    // Tra false neu hang doi dang rong.
    bool getCurrentQuestion(QuizQuestion& outQuestion);

    // Kiem tra dap an nguoi dung chon ('A'..'D', khong phan biet hoa/thuong).
    // Ben trong goi dung moduleSaved() nhu ban console (luu stack + nap cau moi).
    // Tra ve true neu dung; outCorrectMeaning luon duoc dien du lieu.
    bool submitAnswer(char answer, std::string& outCorrectMeaning);

    // Duyet stack lich su giong het vAHistory(), tra ve vector thay vi cout.
    std::vector<QuizHistoryEntry> getQuizHistoryList() const;

    // ==== MOI: tinh nang "Tro giup" - loai bot dap an sai trong trac nghiem ====
    void resetHints();                      // Xoa trang thai an cua tat ca 4 dap an (goi khi sang cau moi)
    bool isHintEliminated(int idx) const;   // idx: 0=A, 1=B, 2=C, 3=D
    char useHint();                         // Loai ngau nhien 1 dap an sai chua bi an; tra ve ky tu vua an ('\0' neu khong the an them)
};

// ============================================================================
// PHAN 6: TU DIEN DONG NGHIA
// Dinh nghia day du (struct SynonymData + class SynonymDictionary) da chuyen
// sang file Synonym.h rieng. O day chi forward-declare de RenderDictionaryApp
// ben duoi khai bao duoc tham so kieu SynonymDictionary& ma KHONG bat buoc
// moi noi include Dictionary.h phai keo theo Synonym.h.
// ============================================================================
class SynonymDictionary;

// ============================================================================
// PHAN 7: GIAO DIEN DO HOA (khai bao - cai dat trong Gui.cpp bang Dear ImGui)
// Khong dua kieu du lieu ImGui vao day de cac file khac include Dictionary.h
// ma khong bat buoc phai co ImGui.
// ============================================================================
void RenderDictionaryApp(Trie& dictionary, SearchHistory& history, Vocabulary& quiz,
                          SynonymDictionary& synonyms);
