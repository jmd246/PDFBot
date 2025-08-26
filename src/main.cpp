#include  <iostream>
#include <vector>
#include <glad/glad.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <PDFParser.hpp>
#include <JobManager.hpp>
#include <ImGuiFileDialog.h>



void windowResizeCallback(GLFWwindow* window, int w, int h);
GLFWwindow* CreateWindow(int w, int h);
void processInput(GLFWwindow* window);
void render(GLFWwindow* window);
void cleanup(GLFWwindow* window);
void setupImGUI(GLFWwindow* window);
void ImGuiStartFrame();
void createImGuiWindow(const char* title, const ImVec2& size,const ImVec2& pos);
void createImGuiWindow(GLFWwindow* window, const char* title, const ImVec2& pos);


const char* windowGUIKey = "ChooseFiledlgKey";
const uint16_t width = 1200, height = 800;
const uint8_t maxThreads = 4;
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
    setupImGUI(window);
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        //Process input
        glfwPollEvents();
        processInput(window);
        ImGuiStartFrame();

        createImGuiWindow(window, "Hello RAG GUI",ImVec2(0, 0));
        ImGui::Text("Ready to fine tune your ai experience ?\nGet Started by extracting a PDF or use the webscrape module.");
        ImGui::End();
        if (jobManager.getJobSize() < maxThreads) {
            createImGuiWindow("Tools", ImVec2(800, 600), ImVec2(50, 100));
            if (ImGui::Button("Load PDF")) {
                ImGui::SetNextWindowSize(ImVec2(width, height), 0);
                ImGuiFileDialog::Instance()->OpenDialog(windowGUIKey, "Choose a pdf", ".pdf");
            }
            ImGui::End();
        }
        if (ImGuiFileDialog::Instance()->Display(windowGUIKey)) {
            //check if selected a file
            if (ImGuiFileDialog::Instance()->IsOk() && jobManager.getJobSize() < maxThreads) {
                std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                jobManager.extractPDF(filePath);
                ImGuiFileDialog::Instance()->Close();
            }
            else {
                ImGuiFileDialog::Instance()->Close();
            }
        }

        //display progress of active jobs
        jobManager.displayProgress();
        if (jobManager.getParserSize() > 0) {
            jobManager.displayCompletedParsers();
        }
        // Rendering
        render(window);
    }
    cleanup(window);
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
void ImGuiStartFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void createImGuiWindow(GLFWwindow* window,const char* title,const ImVec2& pos) {
    int imGUIMainScreenWidth, imGUIMainScreenHeight;
    ImGui::Begin(title);
    glfwGetWindowSize(window, &imGUIMainScreenWidth, &imGUIMainScreenHeight);
    ImGui::SetWindowSize(ImVec2(static_cast<float>(imGUIMainScreenWidth), static_cast<float> (imGUIMainScreenHeight)), 0);
    ImGui::SetWindowPos(pos, 0);
}
void createImGuiWindow(const char* title, const ImVec2& size,const ImVec2& pos) {
    ImGui::Begin(title);
    ImGui::SetWindowSize(size, 0);
    ImGui::SetWindowPos(pos, 0);
}
void setupImGUI(GLFWwindow* window) {
    if (!window) {
        std::cerr << "Window is uninitialized";
        return;
    }
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
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

void processInput(GLFWwindow* window) {
    if (!window) return;
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }
}
void cleanup(GLFWwindow* window) {
    // Cleanup
    //ensure all threads finish before joining
    jobManager.endJobs();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

