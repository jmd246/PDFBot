#ifndef UI_HPP
#define UI_HPP
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <ImGuiFileDialog.h>
#include <JobManager.hpp>
#include  <iostream>


class UI {

private:
	static void setupImGUI(GLFWwindow * window);

	const uint16_t width = 1200, height = 800;
	JobManager& jobManager;

public:
	const char* windowGUIKey = "ChooseFiledlgKey";
	UI(GLFWwindow* window,JobManager& jobManager);
	void ShutDown();
	void ImGuiStartFrame();
	void createImGuiWindow(const char* title, const ImVec2& size, const ImVec2& pos);
	void createImGuiWindow(GLFWwindow* window, const char* title, const ImVec2& pos);
	void UIButton(const char* button, const char* title, const char* fileType) const;
	void UIButtonClick();
	void DisplayProgress();
	void UIProgressBar(bool completed, float progress);

	void displayCompletedParsers() {
		if (jobManager.getParserSize() > 0) {
			ImGui::Begin("PDF Processed");
			for (auto& parser : jobManager.getParsers()) {
				ImGui::Text("%s", parser->allChunks.front().text.c_str());
			}
			ImGui::End();
		}
	}

};

#endif;