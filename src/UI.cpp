#include <UI.hpp>


UI::UI(GLFWwindow* window, JobManager& jobManager): jobManager(jobManager) {
    setupImGUI(window);
}

void UI::ShutDown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    jobManager.endJobs();
}

void UI::ImGuiStartFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void UI::createImGuiWindow(GLFWwindow* window, const char* title, const ImVec2& pos) {
    int imGUIMainScreenWidth, imGUIMainScreenHeight;
    ImGui::Begin(title);
    glfwGetWindowSize(window, &imGUIMainScreenWidth, &imGUIMainScreenHeight);
    ImGui::SetWindowSize(ImVec2(static_cast<float>(imGUIMainScreenWidth), static_cast<float> (imGUIMainScreenHeight)), 0);
    ImGui::SetWindowPos(pos, 0);
}
void UI::createImGuiWindow(const char* title, const ImVec2& size, const ImVec2& pos) {
    ImGui::Begin(title);
    ImGui::SetWindowSize(size, 0);
    ImGui::SetWindowPos(pos, 0);
}
void UI::setupImGUI(GLFWwindow* window) {
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
void UI::UIButton(const char* button, const char* title, const char* fileType) const {
    if (ImGui::Button(button)) {
        ImGui::SetNextWindowSize(ImVec2(width, height), 0);
        ImGuiFileDialog::Instance()->OpenDialog(windowGUIKey, title, fileType);
    }
}
void UI::UIButtonClick() {

    if (ImGuiFileDialog::Instance()->Display(windowGUIKey)) {
        //check if selected a file
        if (ImGuiFileDialog::Instance()->IsOk() && jobManager.getJobSize() < jobManager.m_max_threads) {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            jobManager.extractPDF(filePath);
            ImGuiFileDialog::Instance()->Close();
        }
        else {
            ImGuiFileDialog::Instance()->Close();
        }
    }
}
void UI::UIProgressBar(bool completed, float progress) {
    if (!completed) {
        ImGui::SetNextWindowSize(ImVec2(800, 600), 0);
        ImGui::Begin("PDF Processing");
        ImGui::Text("Extracting PDF ...");
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        ImGui::End();
    }
}
void UI::DisplayProgress() {
    if (jobManager.getJobSize() == 0) return;
    for (auto iterator =  jobManager.getJobs().begin(); iterator != jobManager.getJobs().end(); ) {
        if (iterator->fut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            jobManager.completeJob(iterator);
        }
        else {
            UIProgressBar(iterator->parser->extractionDone, iterator->parser->progress);
            ++iterator;
        }
    }
}