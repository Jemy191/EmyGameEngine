#include "Renderer.h"
#include "OpenGL/OpenGLRenderer.h"
#include "Rendering\Vulkan\VulkanRenderer.h"
#include "Helper/Exeption.h"

bool Renderer::IsGraphicalAPI(GraphicalAPI api) { return Setting::Get("GraphicAPI", GraphicalAPI::OPENGL) == api; }

std::unique_ptr<Renderer> Renderer::CreateGraphicalAPI(GraphicalAPI api, GLFWwindow* window)
{
	switch (api)
	{
	case GraphicalAPI::OPENGL:
		return std::unique_ptr<OpenGLRenderer>(new OpenGLRenderer(window));
	case GraphicalAPI::VULKAN:
		return std::unique_ptr<VulkanRenderer>(new VulkanRenderer(window));
	case GraphicalAPI::METAL:
		throw NotImplemented();
	case GraphicalAPI::DIRECTX12:
		throw NotImplemented();
	}
}

Renderer::Renderer(GLFWwindow* window)
{
}

ImguiBase* Renderer::GetImgui() const
{
	return imgui.get();
}