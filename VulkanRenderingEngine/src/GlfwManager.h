#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class GlfwManager
{
private:
	GLFWwindow* window;

	bool windowHasResize = false;

public:
	GlfwManager(int width, int height, std::string windowName);
	~GlfwManager();

	void SetWindowTitle(std::string name);
	GLFWwindow* GetWindow() const;
	bool HasWindowResize() const;
	void ResetWindowHasResize();

private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
};