#include "pch.h"
#include <Rice/GL/GraphicsManager.hpp>

int main() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(640, 480, "VulkanTest", nullptr, nullptr);
	Rice::Graphics::GraphicsManager graphicsMgr;
	graphicsMgr.init();
	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
