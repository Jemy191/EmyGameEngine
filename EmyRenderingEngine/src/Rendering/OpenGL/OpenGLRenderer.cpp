#include "OpenGLRenderer.h"
#include <Rendering\OpenGL\ImguiOpenGL.h>
#include "Helper/FPSCounter.h"

OpenGLRenderer::OpenGLRenderer(GLFWwindow* window) : Renderer(window)
{
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLCallback, nullptr);

	imgui = std::unique_ptr<ImguiOpenGL>(new ImguiOpenGL(window));

	float position[] =
	{
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
	};

	uint32_t indices[] =
	{
		0,1,2,
		2,3,0
	};

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), position, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);

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

	glUniform4f(1, 0, 0, 0, 1.0f);
}

OpenGLRenderer::~OpenGLRenderer()
{
	glDeleteProgram(shader);
}

void OpenGLRenderer::Present(GlfwManager* window)
{
	glClear(GL_COLOR_BUFFER_BIT);

	timer += FPSCounter::GetDeltaTime() * 4;
	if (timer > 1)
		timer = 0;

	glUniform4f(1, timer, timer, timer, 1.0f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	dynamic_cast<ImguiOpenGL*>(imgui.get())->Draw();

	if (window->HasWindowResize())
	{
		window->ResetWindowHasResize();

		ResizeWindow(window->GetWindow());
	}

	glfwSwapBuffers(window->GetWindow());
}

void OpenGLRenderer::ResizeWindow(GLFWwindow* window)
{
	int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	glViewport(0, 0, width, height);
}

void OpenGLRenderer::GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	std::string sourceText;
	std::string typeText;
	std::string severityText;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		sourceText = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		sourceText = "WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		sourceText = "SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		sourceText = "THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		sourceText = "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		sourceText = "OTHER";
		break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		typeText = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeText = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeText = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeText = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeText = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		typeText = "OTHER";
		break;
	case GL_DEBUG_TYPE_MARKER:
		typeText = "MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		typeText = "PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		typeText = "POP_GROUP";
		break;
	}

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		severityText = "HIGH";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severityText = "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		severityText = "LOW";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severityText = "NOTIFICATION";
		break;
	}

	Logger::Log(LogSeverity::ERROR, sourceText + "-" + typeText + "-" + severityText + ": " + message);
}