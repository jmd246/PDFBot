#include <Input.hpp>

void Input::ProcessInput(GLFWwindow* window) {
    if (!window) return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }
}