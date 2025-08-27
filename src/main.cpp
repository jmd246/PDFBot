#include  <iostream>
#include <vector>
#include <glad/glad.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <Input.hpp>
#include<UI.hpp>

GLFWwindow* CreateWindow(int w, int h);
void windowResizeCallback(GLFWwindow* window, int w, int h);
void render(GLFWwindow* window);
void cleanup(GLFWwindow* window,UI& ui);
const uint16_t width = 1200, height = 800;
JobManager jobManager;


int main() {
    GLFWwindow* window = CreateWindow(width, height);
    if(!window){
        std::cerr << "Failed to Create Window";
        return -1;
    }
    // Initialize OpenGL loader
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL\n";
        return -1;
    }

    UI ui(window,jobManager);
    
    Input input;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        //Process input
        glfwPollEvents();
        input.ProcessInput(window);
        ui.ImGuiStartFrame();
        ui.createImGuiWindow(window, "Hello RAG GUI",ImVec2(0, 0));
        
        ImGui::Text("Ready to fine tune your ai experience ?\nGet Started by extracting a PDF or use the webscrape module.");
        ImGui::End();
        
        if (jobManager.getJobSize() < jobManager.m_max_threads) {
            ui.createImGuiWindow("Tools", ImVec2(800, 600), ImVec2(50, 100));
            ui.UIButton("Load PDF", "Choose a PDF", ".pdf");
            ImGui::End();
        }

        ui.UIButtonClick();
        
        //display progress of active jobs
        ui.DisplayProgress();

        //display completed parsers
        ui.displayCompletedParsers();

        // Rendering
        render(window);
    }
    cleanup(window,ui);
    return 0;
}

void windowResizeCallback(GLFWwindow* window, int w, int h) {
    if (window == NULL) return;
    glViewport(0, 0, w, h);
}

GLFWwindow* CreateWindow(int w,int h) {

    // Initialize GLFW
    if (!glfwInit()) return nullptr;

    // OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   GLFWwindow* window = glfwCreateWindow(w, h, "RAG GUI", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }
    
    glfwSetFramebufferSizeCallback(window, windowResizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync
    
    return window;
}


void render(GLFWwindow* window) {
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
void cleanup(GLFWwindow* window, UI& ui) {
    // Cleanup
    //ensure all threads finish before joining
    ui.ShutDown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

