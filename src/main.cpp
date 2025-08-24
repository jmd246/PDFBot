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
#include <ImGuiFileDialog.h>
#include <future>



void windowResizeCallback(GLFWwindow* window, int w, int h);
GLFWwindow* CreateWindow(int w, int h);
void processInput(GLFWwindow* window);
void render(GLFWwindow* window);
void UIProgressBar(bool completed, float progress);
void cleanup(GLFWwindow* window);
void setupImGUI(GLFWwindow* window);
void ImGuiStartFrame();
void createImGuiWindow(GLFWwindow* window,const char* title, const ImVec2& pos);

struct PDFJob{
    std::future<bool> fut;
    std::unique_ptr<PDFParser> parser;
};
std::vector<PDFJob> jobs;
std::vector< std::unique_ptr<PDFParser> > parsers;

int main() {

 
    GLFWwindow* window = CreateWindow(1200, 800);

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
        createImGuiWindow(window,"Hello RAG GUI", ImVec2(0, 0));
        ImGui::Text("This is your ImGui window!");
        ImGui::End();


     if (ImGui::Button("Load PDF")) {
        ImGui::SetNextWindowSize(ImVec2(800,600),0);
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFiledlgKey", "Choose a pdf", ".pdf");
     }
     if (ImGuiFileDialog::Instance()->Display("ChooseFiledlgKey")) {
         //check if selected a file
         if (ImGuiFileDialog::Instance()->IsOk() && jobs.size() < 4) {
             std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
             // When user selects a file
             PDFJob job;
             job.parser = std::make_unique<PDFParser>();
             job.fut = std::async(std::launch::async, [filePath, parser = job.parser.get()]() -> bool {
                 parser->extractTextChunks(filePath);
                 return true; // dummy return
             });

             jobs.push_back(std::move(job));
             std::cout << "Selected file: " << filePath << std::endl;
             std::cout << "Max threads: " << (unsigned)std::thread::hardware_concurrency();

          
           
         }
     }
     for (auto it = jobs.begin(); it != jobs.end(); ) {
         if (it->fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
             it->fut.get(); // retrieve dummy result
             it->parser->extractionDone = true;

             // Optionally store parser somewhere else before erasing
             parsers.push_back(std::move(it->parser));


             it = jobs.erase(it); // remove job from active queue
         }
         else {
             UIProgressBar(it->parser.get()->extractionDone, it->parser.get()->progress);
             ++it;
         }
     }

   
        // Rendering
        render(window);
    }

    cleanup(window);
    std::cout << parsers.size();
    return 0;
}

void windowResizeCallback(GLFWwindow* window, int w, int h) {
    if (window == NULL) return;
    glViewport(0, 0, w, h);
}

GLFWwindow* CreateWindow(int width,int height) {

    // Initialize GLFW
    if (!glfwInit()) return nullptr;

    // OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   GLFWwindow* window = glfwCreateWindow(height, width, "RAG GUI", nullptr, nullptr);
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
void createImGuiWindow(GLFWwindow* window,const char* title, const ImVec2& pos) {
    // Demo window

    int imGUIMainScreenWidth, imGUIMainScreenHeight;
    ImGui::Begin(title);
    glfwGetWindowSize(window, &imGUIMainScreenWidth, &imGUIMainScreenHeight);
    ImGui::SetWindowSize(ImVec2(static_cast<float>(imGUIMainScreenWidth), static_cast<float> (imGUIMainScreenHeight)), 0);
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
    for (auto& job : jobs) {
        job.parser.get()->stopFlag = true;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
void UIProgressBar(bool completed, float progress) {
    if (!completed) {
        ImGui::SetNextWindowSize(ImVec2(800, 600), 0);
        ImGui::Begin("PDF Processing");
        ImGui::Text("Extracting PDF ...");
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::End();
    }
    else {
        ImGui::Begin("PDF Processing");
        ImGui::Text("Extracting PDF Complete!");
        ImGui::End();
    }
}
