#pragma once
#include "Rendering/Renderer.h"
#include <Rendering\GlfwManager.h>

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer(GLFWwindow* window);

    void Present(GlfwManager* window) override;
};