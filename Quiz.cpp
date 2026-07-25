#include "Dictionary.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

// ============================================================================
// Vocabulary: bai kiem tra trac nghiem tu vung
//   - Hang doi cau hoi (qV[QUIZ_QUEUE_CAPACITY]) cai dat kieu MANG VONG
//     bang 2 chi so front/rear, tranh phai dich chuyen phan tu khi deQueue.
//   - Lich su tra loi (sV) cai dat bang danh sach lien ket don kieu STACK,
//     cau tra loi gan nhat luon nam tren dinh (top).
// ============================================================================

Vocabulary::Vocabulary() {
    top = nullptr;
    size = 0;
    front = -1;
    rear = -1;
    num = 0;
    dictRef = nullptr;
    loadIndex = 0;
    srand(static_cast<unsigned int>(time(nullptr)));
}

Vocabulary::~Vocabulary() {
    while (top != nullptr) {
        sV* p = top;
        top = top->next;
        delete p;
    }
    front = -1;
    rear = -1;
    num = 0;
}

sV* Vocabulary::createNode(char Voca[], char CA[], char YA[]) {
    sV* newNode = new sV();
    strncpy(newNode->Voc, Voca, MAX_STR_LEN - 1);
    newNode->Voc[MAX_STR_LEN - 1] = '\0';
    strncpy(newNode->correctAnwser, CA, MAX_STR_LEN - 1);
    newNode->correctAnwser[MAX_STR_LEN - 1] = '\0';
    strncpy(newNode->yourAnwser, YA, MAX_STR_LEN - 1);
    newNode->yourAnwser[MAX_STR_LEN - 1] = '\0';
    newNode->next = nullptr;
    return newNode;
}

void Vocabulary::push(char Voca[], char CA[], char YA[]) {
    sV* newNode = createNode(Voca, CA, YA);
    newNode->next = top; // top hien tai (co the la nullptr) tro thanh phan tu ke tiep
    top = newNode;
    ++size;
}

void Vocabulary::vAHistory() {
    clearScreen();
    sV* p = top;
    if (p == nullptr) {
        cout << "You haven't answered any questions yet.\n";
    } else {
        cout << "=== QUIZ HISTORY (newest -> oldest) ===\n\n";
        int index = 1;
        while (p != nullptr) {
            cout << index << ". Question: " << p->Voc << "\n";
            cout << "   Correct answer: " << p->correctAnwser << "\n";
            cout << "   You chose: " << p->yourAnwser << "\n\n";
            p = p->next;
            ++index;
        }
    }
    cout << "(Press any key to return to menu...)";
    readOneChar();
}

// enQueue(): them 1 cau hoi vao cuoi hang doi (mang vong, suc chua co dinh)
void Vocabulary::enQueue(char Voca[], char A[], char B[], char C[], char D[], char CA[]) {
    if (num == QUIZ_QUEUE_CAPACITY) {
        cout << "Question queue is full!\n";
        return;
    }

    int insertPos;
    if (front == -1) {
        front = rear = 0;
        insertPos = 0;
    } else {
        rear = (rear + 1) % QUIZ_QUEUE_CAPACITY;
        insertPos = rear;
    }

    strncpy(examEnglish[insertPos].Voc, Voca, MAX_STR_LEN - 1);
    examEnglish[insertPos].Voc[MAX_STR_LEN - 1] = '\0';
    strncpy(examEnglish[insertPos].A, A, MAX_STR_LEN - 1);
    examEnglish[insertPos].A[MAX_STR_LEN - 1] = '\0';
    strncpy(examEnglish[insertPos].B, B, MAX_STR_LEN - 1);
    examEnglish[insertPos].B[MAX_STR_LEN - 1] = '\0';
    strncpy(examEnglish[insertPos].C, C, MAX_STR_LEN - 1);
    examEnglish[insertPos].C[MAX_STR_LEN - 1] = '\0';
    strncpy(examEnglish[insertPos].D, D, MAX_STR_LEN - 1);
    examEnglish[insertPos].D[MAX_STR_LEN - 1] = '\0';
    strncpy(examEnglish[insertPos].correctAnswer, CA, MAX_STR_LEN - 1);
    examEnglish[insertPos].correctAnswer[MAX_STR_LEN - 1] = '\0';

    ++num;
}

// deQueue(): xoa cau hoi o dau hang doi (da duoc tra loi xong)
void Vocabulary::deQueue() {
    if (num == 0) return;

    if (front == rear) {
        front = rear = -1;
    } else {
        front = (front + 1) % QUIZ_QUEUE_CAPACITY;
    }
    --num;
}

// moduleSaved(): chuyen cau vua tra loi tu Queue sang Stack (lich su),
// dong thoi nap ngay 1 cau moi tu Trie de hang doi luon co du cau hoi.
void Vocabulary::moduleSaved(char Voca[], char CA[], char YA[]) {
    push(Voca, CA, YA);
    deQueue();
    moduleLoadFromTrie();
}

// moduleLoadFromTrie(): lay 1 tu tiep theo trong tu dien, tao 3 dap an sai
// ngau nhien (lay tu nghia cua cac tu khac) + 1 dap an dung, tron vi tri,
// roi dua ca cau hoi vao hang doi.
void Vocabulary::moduleLoadFromTrie() {
    if (dictRef == nullptr) return; 
    if (num >= QUIZ_QUEUE_CAPACITY) return; 

    // 1. Sửa kiểu dữ liệu ở đây thành WordEntry
    const vector<WordEntry>& data = dictRef->getAllWords();
    int total = static_cast<int>(data.size());
    if (total == 0 || loadIndex >= total) return; 

    // 2. Thay .first thành .word và .second thành .meaning
    string word = data[loadIndex].word;
    string correctMeaning = data[loadIndex].meaning;
    ++loadIndex;

    vector<string> wrongChoices;
    int guard = 0;
    while (wrongChoices.size() < 3 && guard < 100) {
        int r = rand() % total;
        // 3. Thay .second thành .meaning ở vòng lặp check trùng
        bool trung = (data[r].meaning == correctMeaning);
        for (const auto& w : wrongChoices) {
            if (w == data[r].meaning) trung = true;
        }
        if (!trung) wrongChoices.push_back(data[r].meaning);
        ++guard;
    }
    while (wrongChoices.size() < 3) wrongChoices.push_back("N/A"); // Du phong khi tu dien qua it tu

    string choices[4] = {correctMeaning, wrongChoices[0], wrongChoices[1], wrongChoices[2]};
    for (int k = 3; k > 0; --k) { // Tron vi tri 4 dap an (Fisher-Yates)
        int j = rand() % (k + 1);
        swap(choices[k], choices[j]);
    }

    char Voc[MAX_STR_LEN], A[MAX_STR_LEN], B[MAX_STR_LEN], C[MAX_STR_LEN], D[MAX_STR_LEN], CA[MAX_STR_LEN];
    strncpy(Voc, word.c_str(), MAX_STR_LEN - 1); Voc[MAX_STR_LEN - 1] = '\0';
    strncpy(A, choices[0].c_str(), MAX_STR_LEN - 1); A[MAX_STR_LEN - 1] = '\0';
    strncpy(B, choices[1].c_str(), MAX_STR_LEN - 1); B[MAX_STR_LEN - 1] = '\0';
    strncpy(C, choices[2].c_str(), MAX_STR_LEN - 1); C[MAX_STR_LEN - 1] = '\0';
    strncpy(D, choices[3].c_str(), MAX_STR_LEN - 1); D[MAX_STR_LEN - 1] = '\0';
    strncpy(CA, correctMeaning.c_str(), MAX_STR_LEN - 1); CA[MAX_STR_LEN - 1] = '\0';

    enQueue(Voc, A, B, C, D, CA);
}

void Vocabulary::setDictionary(Trie& dict) {
    dictRef = &dict;
}

// shuffle(): tron thu tu cac cau hoi con lai trong hang doi + tron vi tri
// 4 dap an cua cau hoi sap hien thi (Fisher-Yates tren pham vi mang vong).
void Vocabulary::shuffle() {
    for (int i = num - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int idx1 = (front + i) % QUIZ_QUEUE_CAPACITY;
        int idx2 = (front + j) % QUIZ_QUEUE_CAPACITY;
        swap(examEnglish[idx1], examEnglish[idx2]);
    }

    char* ans[4] = {
        examEnglish[front].A, examEnglish[front].B,
        examEnglish[front].C, examEnglish[front].D
    };
    for (int i = 3; i > 0; --i) {
        int j = rand() % (i + 1);
        char temp[MAX_STR_LEN];
        strcpy(temp, ans[i]);
        strcpy(ans[i], ans[j]);
        strcpy(ans[j], temp);
    }
}

// moduleExam(): vong lap chinh cua bai kiem tra
void Vocabulary::moduleExam() {
    // Sua loi thieu du lieu ban dau: neu hang doi con trong, tu dong nap
    // cau hoi tu Trie truoc khi bat dau (toi da du QUIZ_QUEUE_CAPACITY cau).
    while (num < QUIZ_QUEUE_CAPACITY) {
        int before = num;
        moduleLoadFromTrie();
        if (num == before) break; // Khong nap them duoc nua (het tu hoac chua co Trie)
    }

    while (true) {
        if (num == 0) {
            clearScreen();
            cout << "No more questions (dictionary is empty or out of words).\n";
            cout << "(Press any key to return to menu...)";
            readOneChar();
            return;
        }

        shuffle();
        clearScreen();
        cout << "Word to translate: " << examEnglish[front].Voc << "\n\n";
        cout << "A. " << examEnglish[front].A << "\n";
        cout << "B. " << examEnglish[front].B << "\n";
        cout << "C. " << examEnglish[front].C << "\n";
        cout << "D. " << examEnglish[front].D << "\n\n";
        cout << "Enter answer (A/B/C/D), Esc to exit: ";

        int answer = readOneChar();
        if (answer == 27) return; // Phim Esc -> thoat ve menu

        while ((answer < 'a' || answer > 'd') && (answer < 'A' || answer > 'D')) {
            cout << "\nInvalid answer, please try again: ";
            answer = readOneChar();
            if (answer == 27) return;
        }

        char* chosenText = nullptr;
        switch (answer) {
            case 'A': case 'a': chosenText = examEnglish[front].A; break;
            case 'B': case 'b': chosenText = examEnglish[front].B; break;
            case 'C': case 'c': chosenText = examEnglish[front].C; break;
            case 'D': case 'd': chosenText = examEnglish[front].D; break;
        }

        cout << "\n";
        if (strcmp(chosenText, examEnglish[front].correctAnswer) == 0) {
            cout << "Correct!\n";
            // Trả lời đúng -> nạp câu mới
            moduleSaved(examEnglish[front].Voc, examEnglish[front].correctAnswer, chosenText);
        } else {
            cout << "Incorrect! The correct answer is: " << examEnglish[front].correctAnswer << "\n";
            
            // Trả lời sai -> Spaced Repetition (nhét lại vào cuối hàng đợi)
            push(examEnglish[front].Voc, examEnglish[front].correctAnswer, chosenText);
            
            char V[MAX_STR_LEN], A[MAX_STR_LEN], B[MAX_STR_LEN], C[MAX_STR_LEN], D[MAX_STR_LEN], CA[MAX_STR_LEN];
            strcpy(V, examEnglish[front].Voc);
            strcpy(A, examEnglish[front].A);
            strcpy(B, examEnglish[front].B);
            strcpy(C, examEnglish[front].C);
            strcpy(D, examEnglish[front].D);
            strcpy(CA, examEnglish[front].correctAnswer);
            
            deQueue();
            enQueue(V, A, B, C, D, CA);
        }

        sleepMs(1200);
    }
}

// ============================================================================
// CAC HAM MOI CHO GUI - chi goi lai dung cac ham/thanh phan da co o tren
// (push, enQueue, deQueue, shuffle, moduleSaved, moduleLoadFromTrie...),
// KHONG viet lai bat ky thuat toan Queue/Stack/Fisher-Yates nao.
// ============================================================================

// Y het doan while dau tien cua moduleExam(), tach rieng de GUI chu dong
// goi 1 lan khi nguoi dung chuyen sang tab Quiz.
void Vocabulary::ensureQuestionsLoaded() {
    while (num < QUIZ_QUEUE_CAPACITY) {
        int before = num;
        moduleLoadFromTrie();
        if (num == before) break; // Khong nap them duoc nua
    }
}

bool Vocabulary::getCurrentQuestion(QuizQuestion& outQuestion) {
    if (num == 0) return false;

    shuffle(); // Ham shuffle() co san: tron thu tu hang doi + tron 4 dap an cua cau hien tai
    resetHints(); // MOI: moi lan lay 1 cau hoi "hien tai" (cau moi HOAC bat dau lai Quiz) -> reset het trang thai an

    outQuestion.word = examEnglish[front].Voc;
    outQuestion.A = examEnglish[front].A;
    outQuestion.B = examEnglish[front].B;
    outQuestion.C = examEnglish[front].C;
    outQuestion.D = examEnglish[front].D;
    return true;
}

bool Vocabulary::submitAnswer(char answer, string& outCorrectMeaning) {
    if (num == 0) return false;

    char normalized = static_cast<char>(toupper(static_cast<unsigned char>(answer)));
    char* chosenText = nullptr;
    switch (normalized) {
        case 'A': chosenText = examEnglish[front].A; break;
        case 'B': chosenText = examEnglish[front].B; break;
        case 'C': chosenText = examEnglish[front].C; break;
        case 'D': chosenText = examEnglish[front].D; break;
        default: return false; // Dap an khong hop le
    }

    outCorrectMeaning = examEnglish[front].correctAnswer;
    bool isCorrect = (strcmp(chosenText, examEnglish[front].correctAnswer) == 0);

    if (isCorrect) {
        // TRẢ LỜI ĐÚNG: Lưu lịch sử, xóa khỏi Queue và nạp câu MỚI từ Trie
        moduleSaved(examEnglish[front].Voc, examEnglish[front].correctAnswer, chosenText);
    } else {
        // TRẢ LỜI SAI (Spaced Repetition): 
        // 1. Vẫn đẩy vào Stack lịch sử để ghi nhận đã chọn sai
        push(examEnglish[front].Voc, examEnglish[front].correctAnswer, chosenText);
        
        // 2. Lưu tạm dữ liệu câu hỏi hiện tại trước khi xóa
        char V[MAX_STR_LEN], A[MAX_STR_LEN], B[MAX_STR_LEN], C[MAX_STR_LEN], D[MAX_STR_LEN], CA[MAX_STR_LEN];
        strcpy(V, examEnglish[front].Voc);
        strcpy(A, examEnglish[front].A);
        strcpy(B, examEnglish[front].B);
        strcpy(C, examEnglish[front].C);
        strcpy(D, examEnglish[front].D);
        strcpy(CA, examEnglish[front].correctAnswer);
        
        // 3. Xóa khỏi đầu hàng đợi và nhét ngược lại vào CUỐI hàng đợi
        deQueue();
        enQueue(V, A, B, C, D, CA);
    }

    return isCorrect;
}

// Duyet Stack Y HET vAHistory() o tren, chi khac la push_back vao vector
// thay vi cout ra console.
vector<QuizHistoryEntry> Vocabulary::getQuizHistoryList() const {
    vector<QuizHistoryEntry> result;
    for (sV* p = top; p != nullptr; p = p->next) {
        QuizHistoryEntry entry;
        entry.word = p->Voc;
        entry.correctAnswer = p->correctAnwser;
        entry.yourAnswer = p->yourAnwser;
        result.push_back(entry);
    }
    return result;
}

// ============================================================================
// TINH NANG "TRO GIUP" (Hint) - loai bot dap an sai trong trac nghiem
// ============================================================================
void Vocabulary::resetHints() {
    for (int i = 0; i < 4; ++i) hintEliminated[i] = false;
}

bool Vocabulary::isHintEliminated(int idx) const {
    if (idx < 0 || idx > 3) return false;
    return hintEliminated[idx];
}

char Vocabulary::useHint() {
    if (num == 0) return '\0';

    const char letters[4] = {'A', 'B', 'C', 'D'};
    char* texts[4] = {
        examEnglish[front].A, examEnglish[front].B,
        examEnglish[front].C, examEnglish[front].D
    };

    // Gom lai cac dap an vua SAI vua CHUA bi an truoc do
    vector<int> candidates;
    for (int i = 0; i < 4; ++i) {
        bool isCorrectChoice = (strcmp(texts[i], examEnglish[front].correctAnswer) == 0);
        if (!isCorrectChoice && !hintEliminated[i]) {
            candidates.push_back(i);
        }
    }

    // Luon giu lai >= 1 dap an sai (tranh an het, lo luon dap an dung)
    if (candidates.size() <= 1) return '\0';

    int pick = candidates[rand() % candidates.size()];
    hintEliminated[pick] = true;
    return letters[pick];


