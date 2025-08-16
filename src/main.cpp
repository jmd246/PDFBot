#include  <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <glad/glad.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <PDFParser.hpp>
#include <thread>



void windowResizeCallback(GLFWwindow* window, int w, int h);

int main() {
    // Initialize GLFW
    if (!glfwInit()) return -1;

    // OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "RAG GUI", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, windowResizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // Initialize OpenGL loader
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL\n";
        return -1;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    PDFParser parser;
    std::thread extractThread(
        &PDFParser::extractPDFThread, // pointer to member function
        &parser,                        // instance of the class
        "C:/Users/Owner/OneDrive/Desktop/PDFBot/assets/pdfs/learn_opengl.pdf" //argument
    );
    extractThread.detach();
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Demo window
        ImGui::Begin("Hello RAG GUI");
        ImGui::Text("This is your ImGui window!");
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void windowResizeCallback(GLFWwindow* window,int w, int h) {
    if (window == NULL) return;
    glViewport(0, 0, w, h);
}