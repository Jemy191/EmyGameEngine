#include "OpenGLRenderer.h"
#include <Rendering\OpenGL\ImguiOpenGL.h>

OpenGLRenderer::OpenGLRenderer(GLFWwindow* window) : Renderer(window)
{
	imgui = std::unique_ptr<ImguiOpenGL>(new ImguiOpenGL(window));
}

void OpenGLRenderer::Present(GlfwManager* window)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_TRIANGLES);

	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.5f, -0.5f);

	glEnd();

	dynamic_cast<ImguiOpenGL*>(imgui.get())->Draw();

	glfwSwapBuffers(window->GetWindow());
}
