#include "OpenGLRenderer.h"
#include <Rendering\OpenGL\ImguiOpenGL.h>

OpenGLRenderer::OpenGLRenderer(GLFWwindow* window) : Renderer(window)
{
	imgui = std::unique_ptr<ImguiOpenGL>(new ImguiOpenGL(window));

	float position[6] =
	{
		-0.5f, -0.5f,
		0.0f, 0.5f,
		0.5f, -0.5f
	};

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), position, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(0);

	std::string vertexShader =
		"#version 330 core\n"
		"layout(location = 0) in vec4 position;\n"
		"void main()\n"
		"{\n"
		"gl_Position = position;\n"
		"}\n";

	std::string fragmentShader =
		"#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";

	shader = CreateShader(vertexShader, fragmentShader);

	glUseProgram(shader);
}

OpenGLRenderer::~OpenGLRenderer()
{
	glDeleteProgram(shader);
}

void OpenGLRenderer::Present(GlfwManager* window)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	dynamic_cast<ImguiOpenGL*>(imgui.get())->Draw();

	glfwSwapBuffers(window->GetWindow());
}
