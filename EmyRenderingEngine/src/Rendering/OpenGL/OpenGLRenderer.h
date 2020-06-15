#pragma once
#include "Rendering/Renderer.h"
#include <Rendering/GlfwManager.h>
#include "Helper/Log.h"
#include <fstream>
#include <random>

class OpenGLRenderer : public Renderer
{
private:
	GLuint buffer;
	GLuint ibo;
	GLuint shader;

	float timer = 0;

	static std::vector<char> LoadShader(std::string fileName)
	{
		std::ifstream file("Assets/Shaders/" + fileName + ".spv", std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	static GLuint CompileShader(GLuint type, std::vector<char> source)
	{
		GLuint id = glCreateShader(type);
		glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, source.data(), source.size());
		glSpecializeShader(id, "main", 0, nullptr, nullptr);
		//glShaderSource(id, 1, &src, );
		//glCompileShader(id);

		GLint result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			GLint lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			char* message = (char*)alloca(lenght * sizeof(char));
			glGetShaderInfoLog(id, lenght, &lenght, message);

			Logger::Log("Failed to compile shader");
			Logger::Log(std::string(message));

			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	static GLuint CreateShader(const std::string& vert, const std::string& frag)
	{
		GLuint program = glCreateProgram();
		GLuint vs = CompileShader(GL_VERTEX_SHADER, LoadShader("TutoGLVert"));
		GLuint fs = CompileShader(GL_FRAGMENT_SHADER, LoadShader("TutoGLFrag"));

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}

public:
	OpenGLRenderer(GLFWwindow* window);
	~OpenGLRenderer();

	void Present(GlfwManager* window) override;

private:
	void ResizeWindow(GLFWwindow* window);

	static void APIENTRY GLCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam);
};