#include "Dictionary.h"
#include "Synonym.h"
#include "imgui.h"
#include <cstring>
#include <string>

using namespace std;

// ============================================================================
// Gui.cpp - Toan bo giao dien Dear ImGui.
// File nay CHI goi cac ham cong khai cua Trie / SearchHistory / Vocabulary
// (khong dong nao dung toi thuat toan ben trong cac class do).
// ============================================================================

enum class ViewTab { DICTIONARY, HISTORY, QUIZ, SYNONYMS };
static ViewTab currentTab = ViewTab::DICTIONARY;

// ---- State cho tab "Tra tu dien" ----
static char searchBuffer[256] = "";
static char newWordBuffer[256] = "";
static char newPosBuffer[64] = "";       // MOI: tu loai khi them tu
static char newMeaningBuffer[256] = "";
static char newExampleBuffer[256] = "";  // MOI: cau vi du khi them tu
static char deleteWordBuffer[256] = "";
static string lastAddDeleteMessage;

// ---- State cho tab "Quiz" ----
static bool quizStarted = false;
static QuizQuestion currentQuestion;
static string lastAnswerFeedback;
static string lastHintMessage; // MOI: thong bao ket qua khi bam nut Tro giup

// ---- State cho tab "Synonyms" ----
static char synonymSearchBuffer[256] = "";

static void RenderDictionaryTab(Trie& dictionary, SearchHistory& history) {
    ImGui::TextUnformatted("Enter word to search (suggestions appear below):");
    ImGui::SetNextItemWidth(-1);
    bool textChanged = ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer));
    (void)textChanged; // Khong can bat su kien rieng, cu moi frame la doc lai buffer

    string typed = searchBuffer;

    if (!typed.empty()) {
        // MOI: dung ban searchExact 4 tham so de lay them Tu loai + Vi du
        string exactMeaning, exactPos, exactExample;
        if (dictionary.searchExact(typed, exactMeaning, exactPos, exactExample)) {
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Meaning: %s", exactMeaning.c_str());

            // MOI: Tu loai hien thi mau xanh duong ngay canh nghia, chi khi khong rong
            if (!exactPos.empty()) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.45f, 0.65f, 1.0f, 1.0f), "[%s]", exactPos.c_str());
            }

            // MOI: Vi du hien thi mau mo (TextDisabled), chi khi khong rong
            if (!exactExample.empty()) {
                ImGui::TextDisabled("e.g. %s", exactExample.c_str());
            }

            history.addWord(typed, exactMeaning); // Da tra dung -> luu vao lich su
        } else {
            ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.1f, 1.0f), "Exact match not found.");

            // Live Search: goi y theo tien to, nho toc do cua Trie
            auto suggestions = dictionary.getPrefixSuggestions(typed, 10);
            if (!suggestions.empty()) {
                ImGui::Text("Suggestions (%zu words):", suggestions.size());
                ImGui::BeginChild("suggest_list", ImVec2(0, 150), true);
                for (auto& s : suggestions) {
                    // Bam vao 1 dong goi y -> dien tu do vao o tim kiem
                    string label = s.first + "  -  " + s.second;
                    if (ImGui::Selectable(label.c_str())) {
                        strncpy(searchBuffer, s.first.c_str(), sizeof(searchBuffer) - 1);
                        searchBuffer[sizeof(searchBuffer) - 1] = '\0';
                    }
                }
                ImGui::EndChild();
            } else {
                // Khong co tu nao khop tien to -> co the go sai chinh ta
                auto spelling = dictionary.getSpellingSuggestions(typed, 5);
                if (!spelling.empty()) {
                    ImGui::Text("Did you mean:");
                    for (auto& sp : spelling) {
                        string outMeaning;
                        dictionary.searchExact(sp.first, outMeaning); // ban 2 tham so - chi can nghia o day
                        ImGui::BulletText("%s (distance %d) - %s", sp.first.c_str(), sp.second, outMeaning.c_str());
                    }
                }
            }
        }
    }

    ImGui::Separator();
    ImGui::TextUnformatted("Add new word:");
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("English word", newWordBuffer, sizeof(newWordBuffer));
    ImGui::SetNextItemWidth(90);
    ImGui::InputText("Part of speech (optional)", newPosBuffer, sizeof(newPosBuffer)); // MOI
    ImGui::SetNextItemWidth(250);
    ImGui::InputText("Vietnamese meaning", newMeaningBuffer, sizeof(newMeaningBuffer));
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("Example sentence (optional)", newExampleBuffer, sizeof(newExampleBuffer)); // MOI
    ImGui::SameLine();
    if (ImGui::Button("Add word")) {
        if (strlen(newWordBuffer) > 0 && strlen(newMeaningBuffer) > 0) {
            // MOI: truyen them pos + example, dong bo voi chu ky insert() moi trong core
            dictionary.insert(newWordBuffer, newMeaningBuffer, newPosBuffer, newExampleBuffer);
            lastAddDeleteMessage = string("Added '") + newWordBuffer + "'.";
            newWordBuffer[0] = '\0';
            newPosBuffer[0] = '\0';
            newMeaningBuffer[0] = '\0';
            newExampleBuffer[0] = '\0';
        } else {
            lastAddDeleteMessage = "Word or meaning cannot be empty!";
        }
    }

    ImGui::TextUnformatted("Delete word:");
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("Word to delete", deleteWordBuffer, sizeof(deleteWordBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        if (dictionary.removeWord(deleteWordBuffer)) {
            lastAddDeleteMessage = string("Deleted '") + deleteWordBuffer + "'.";
        } else {
            lastAddDeleteMessage = "This word does not exist.";
        }
        deleteWordBuffer[0] = '\0';
    }

    if (!lastAddDeleteMessage.empty()) {
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", lastAddDeleteMessage.c_str());
    }
}

static void RenderHistoryTab(SearchHistory& history) {
    ImGui::Text("Search history (last %d / 10 words):", history.size());
    ImGui::Separator();

    auto list = history.getHistoryList(); // Ham moi: tra ve du lieu thay vi cout
    if (list.empty()) {
        ImGui::TextDisabled("No words searched yet.");
        return;
    }

    if (ImGui::BeginTable("history_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Word");
        ImGui::TableSetupColumn("Meaning");
        ImGui::TableHeadersRow();
        for (auto& item : list) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(item.first.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(item.second.c_str());
        }
        ImGui::EndTable();
    }
}

static void RenderQuizTab(Vocabulary& quiz) {
    if (!quizStarted) {
        quiz.ensureQuestionsLoaded();   // Nap day hang doi cau hoi tu Trie
        // getCurrentQuestion() da tu goi resetHints() ben trong (xem Quiz.cpp)
        // -> vua dung cho lan bat dau Quiz dau tien, vua dung cho moi lan
        // sang cau hoi tiep theo, khong can goi resetHints() rieng o day.
        quizStarted = quiz.getCurrentQuestion(currentQuestion);
        lastHintMessage.clear();
    }

    if (!quiz.hasQuestions()) {
        ImGui::TextDisabled("Dictionary does not have enough words to create questions.");
        return;
    }

    ImGui::Text("Word to translate: %s", currentQuestion.word.c_str());

    // MOI: nut dung quyen tro giup - loai bot 1 dap an sai
    if (ImGui::Button("Use Hint (Eliminate 1 wrong answer)")) {
        char eliminated = quiz.useHint();
        if (eliminated == '\0') {
            lastHintMessage = "No more hints available for this question.";
        } else {
            lastHintMessage = string("Eliminated answer ") + eliminated + ".";
        }
    }
    if (!lastHintMessage.empty()) {
        ImGui::SameLine();
        ImGui::TextDisabled("%s", lastHintMessage.c_str());
    }

    ImGui::Separator();

    // idx: 0=A, 1=B, 2=C, 3=D - phai khop dung thu tu voi hintEliminated[] ben Quiz.cpp
    auto answerButton = [&](char key, const string& text, int idx) {
        if (quiz.isHintEliminated(idx)) return; // MOI: dap an da bi loai -> khong ve nut nay

        string btnLabel = string(1, key) + ". " + text;
        if (ImGui::Button(btnLabel.c_str(), ImVec2(-1, 0))) {
            string correctMeaning;
            bool isCorrect = quiz.submitAnswer(key, correctMeaning);
            lastAnswerFeedback = isCorrect
                ? "Correct!"
                : ("Incorrect! Correct answer: " + correctMeaning);

            // Cau da tra loi duoc moduleSaved()/spaced-repetition thay the bang
            // cau moi trong hang doi. Lay cau tiep theo de hien thi cho luot sau
            // -> resetHints() se tu dong chay ben trong getCurrentQuestion().
            quiz.getCurrentQuestion(currentQuestion);
            lastHintMessage.clear();
        }
    };

    answerButton('A', currentQuestion.A, 0);
    answerButton('B', currentQuestion.B, 1);
    answerButton('C', currentQuestion.C, 2);
    answerButton('D', currentQuestion.D, 3);

    if (!lastAnswerFeedback.empty()) {
        ImGui::Separator();
        ImGui::TextWrapped("%s", lastAnswerFeedback.c_str());
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Quiz history")) {
        auto qh = quiz.getQuizHistoryList(); // Ham moi: tra ve du lieu thay vi cout
        if (qh.empty()) {
            ImGui::TextDisabled("No questions answered yet.");
        } else {
            for (auto& e : qh) {
                ImGui::BulletText("%s | correct: %s | you chose: %s",
                                   e.word.c_str(), e.correctAnswer.c_str(), e.yourAnswer.c_str());
            }
        }
    }
}

static void RenderSynonymsTab(SynonymDictionary& synonyms) {
    ImGui::TextUnformatted("Enter an English word to find its synonyms:");
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("##synonym_search", synonymSearchBuffer, sizeof(synonymSearchBuffer));
    ImGui::SameLine();
    // Nut van co de nguoi dung bam nhu yeu cau, nhung ket qua da tu cap nhat
    // ngay khi go (giong Live Search o tab Dictionary) nen bam hay khong deu ra ket qua dung.
    ImGui::Button("Find Synonyms");

    string typed = synonymSearchBuffer;

    if (!typed.empty()) {
        SynonymData data;
        ImGui::Separator();

        if (synonyms.findSynonyms(typed, data)) {
            // Dong 1: tu goc (in dam bang mau trang mac dinh, co font lon hon neu muon)
            ImGui::Text("%s", typed.c_str());

            // Dong 2: nghia tieng Viet - dung mau khac (vang nhat) de de phan biet
            // voi tu goc. ImGui khong co san font in nghieng, nen mo phong bang
            // mau + tien to "(nghia: ...)" thay vi italic that.
            ImGui::TextColored(ImVec4(0.85f, 0.75f, 0.25f, 1.0f), "(nghia: %s)", data.meaning.c_str());

            ImGui::Spacing();

            // Dong 3 tro di: liet ke tung tu dong nghia bang BulletText
            if (data.words.empty()) {
                ImGui::TextDisabled("(No synonyms listed for this word)");
            } else {
                for (auto& syn : data.words) {
                    ImGui::BulletText("%s", syn.c_str());
                }
            }
        } else {
            ImGui::TextDisabled("No synonyms found for '%s'.", typed.c_str());
        }
    }
}

// Ham duy nhat main.cpp can goi moi frame
void RenderDictionaryApp(Trie& dictionary, SearchHistory& history, Vocabulary& quiz,
                          SynonymDictionary& synonyms) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("English - Vietnamese Dictionary", nullptr,
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Dictionary")) currentTab = ViewTab::DICTIONARY;
    ImGui::SameLine();
    if (ImGui::Button("Search History")) currentTab = ViewTab::HISTORY;
    ImGui::SameLine();
    if (ImGui::Button("Quiz")) currentTab = ViewTab::QUIZ;
    ImGui::SameLine();
    if (ImGui::Button("Synonyms")) currentTab = ViewTab::SYNONYMS;

    ImGui::Separator();

    switch (currentTab) {
        case ViewTab::DICTIONARY: RenderDictionaryTab(dictionary, history); break;
        case ViewTab::HISTORY:    RenderHistoryTab(history); break;
        case ViewTab::QUIZ:       RenderQuizTab(quiz); break;
        case ViewTab::SYNONYMS:   RenderSynonymsTab(synonyms); break;
    }

    ImGui::End();
}
