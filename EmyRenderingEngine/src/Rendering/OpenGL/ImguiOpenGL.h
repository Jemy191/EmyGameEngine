#pragma once
#include "Header/ImguiHeader.h"
#include "Imgui/imgui_impl_opengl3.h"

#include "Rendering/UI/ImguiBase.h"

class ImguiOpenGL : public ImguiBase
{
public:
	ImguiOpenGL(GLFWwindow* window);
	~ImguiOpenGL();

	// Inherited via ImguiBase
	virtual void StartFrame() override;

	void Draw();
};

