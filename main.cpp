// main.cpp - Diem khoi dau DUY NHAT cua ban GUI.
// File nay chi lo: khoi tao cua so (GLFW), khoi tao Dear ImGui, va vong lap
// render. Toan bo logic tu dien nam trong Trie/History/Quiz.cpp (khong doi),
// toan bo giao dien nam trong Gui.cpp (RenderDictionaryApp).

#include "Dictionary.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <cstdio>

#include "Synonym.h"
static void GlfwErrorCallback(int error, const char* description) {
    fprintf(stderr, "[GLFW Error %d] %s\n", error, description);
}

int main() {
    // ------------------------------------------------------------------
    // Buoc 1: Khoi tao GLFW + tao cua so OpenGL
    // ------------------------------------------------------------------
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    const char* glslVersion = "#version 130"; // OpenGL 3.0 -> khop voi hint ben duoi
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(900, 700, "English - Vietnamese Dictionary (GUI)", nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Bat V-Sync

    // ------------------------------------------------------------------
    // Buoc 2: Khoi tao Dear ImGui
    // ------------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 22.0f, nullptr, io.Fonts->GetGlyphRangesVietnamese());
    (void)io;
    ImGui::StyleColorsDark();

    // ---> CHÃˆN 2 DÃ’NG NÃ€Y VÃ€O ÄÃ‚Y Äá»‚ PHÃ“NG TO GIAO DIá»†N <---
    io.FontGlobalScale = 1.2f;             
    ImGui::GetStyle().ScaleAllSizes(1.2f); 

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // ------------------------------------------------------------------
    // Buoc 3: Khoi tao du lieu tu dien (logic GIU NGUYEN, khong doi 1 dong)
    // ------------------------------------------------------------------
    Trie dictionary;
    dictionary.loadData("tudien.txt");

    SearchHistory history;
    Vocabulary quiz;
    quiz.setDictionary(dictionary);
    SynonymDictionary synonyms;
    synonyms.loadData("dongnghia.txt");
    // ------------------------------------------------------------------
    // Buoc 4: Vong lap chinh
    // ------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        RenderDictionaryApp(dictionary, history, quiz, synonyms); // Toan bo UI nam trong Gui.cpp

        ImGui::Render();
        int displayW, displayH;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.10f, 0.10f, 0.12f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // ------------------------------------------------------------------
    // Buoc 5: Don dep
    // ------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
