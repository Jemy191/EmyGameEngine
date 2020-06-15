#include "Rendering/GlfwManager.h"
#include "Helper/Log.h"
#include "Game/Setting.h"
#include "Renderer.h"

GlfwManager::GlfwManager(int width, int height, std::string windowName)
{
	glfwInit();

	glfwWindowHint(GLFW_MAXIMIZED, true);

	if (Renderer::IsGraphicalAPI(GraphicalAPI::OPENGL))
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	if (Renderer::IsGraphicalAPI(GraphicalAPI::VULKAN))
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

	if (Renderer::IsGraphicalAPI(GraphicalAPI::OPENGL))
	{
		glfwMakeContextCurrent(window);
		if (glewInit() != GLEW_OK)
			Logger::Log(ERROR, "Failed to init glew");

		glfwSwapInterval(1);
	}

	Logger::Log("GLFW manager init");
}

GlfwManager::~GlfwManager()
{
	glfwDestroyWindow(window);
	glfwTerminate();

	Logger::Log("GLFW destroyed");
}

void GlfwManager::SetWindowTitle(std::string name)
{
	glfwSetWindowTitle(window, name.c_str());
}

GLFWwindow* GlfwManager::GetWindow() const
{
	return window;
}

bool GlfwManager::HasWindowResize() const
{
	return windowHasResize;
}

void GlfwManager::ResetWindowHasResize()
{
	windowHasResize = false;
}

void GlfwManager::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<GlfwManager*>(glfwGetWindowUserPointer(window));
	app->windowHasResize = true;
}