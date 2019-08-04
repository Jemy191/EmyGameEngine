#include <iostream>
#include <sstream>

#include <GlfwManager.h>
#include <VulkanManager.h>
#include <FPSCounter.h>
#include <glm/gtx/rotate_vector.hpp>

bool demoWindowOpen = true;
float timeToUpdateFPS = 0.25f;
float fpsUpdateTimer = timeToUpdateFPS;

float camSpeed = 20;
float lookSpeed = 0.5f;

char modelToLoadInput[64] = "";

bool settingWindowOpen = false;

void GUI(VulkanManager* vulkanManager)
{
	ImGui::ShowDemoWindow(&demoWindowOpen);

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
	ImGui::Begin("Setting Window", &settingWindowOpen);
	{
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

		ImGui::SliderFloat("Cam speed", &camSpeed, 1, 100);
		ImGui::SliderFloat3("Light dir", &vulkanManager->lightDir.x, -1, 1);

		ImGui::SliderFloat("Light shininess", &vulkanManager->lightSetting.x, 1, 100);
		ImGui::SliderFloat("Light spec", &vulkanManager->lightSetting.y, 0.1f, 10);

		ImGui::ColorEdit3("Light color", &vulkanManager->lightColor.x);

		ImGui::InputText("ModelToLoad", modelToLoadInput, IM_ARRAYSIZE(modelToLoadInput));
		ImGui::SameLine();
		ImGui::Text(".obj");
		ImGui::SameLine();
		if (ImGui::Button("Load model"))
			vulkanManager->BasicLoadModel(std::string(modelToLoadInput));

		for (size_t i = 0; i < vulkanManager->models.size(); i++)
		{
			ImGui::SliderFloat3((std::to_string(i) + "Transform").c_str(), &vulkanManager->models[i]->position.x, -20, 20);
		}
	}
	ImGui::End();
}

#if DEBUG
int main()
#else
int WinMain()
#endif
{
	GlfwManager glfwManager = GlfwManager(1200, 600, "TestGame");
	VulkanManager vulkanManager(glfwManager.GetWindow(), VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT);

	try
	{
		double mousePosX, mousePosY;
		glfwGetCursorPos(glfwManager.GetWindow(), &mousePosX, &mousePosY);
		glm::vec2 lastMousePos = glm::vec2(mousePosX, mousePosY);

		while (!glfwWindowShouldClose(glfwManager.GetWindow()))
		{
			glfwPollEvents();
			FPSCounter::StartCounting();

			//cam movement
			glm::vec3 dir = glm::vec3(0);

			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_ESCAPE))
				break;

			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_W))
				dir.y = 1;
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_A))
				dir.x = -1;

			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_S))
				dir.y = -1;
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_D))
				dir.x = 1;

			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_Q))
				dir.z = -1;
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_E))
				dir.z = 1;

			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_PAGE_UP))
				camSpeed += 10 * FPSCounter::GetDeltaTime();
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_PAGE_DOWN))
				camSpeed += -10 * FPSCounter::GetDeltaTime();

			vulkanManager.camPos += dir.y * vulkanManager.camDir * camSpeed * FPSCounter::GetDeltaTime();
			vulkanManager.camPos += dir.x * glm::cross(vulkanManager.camDir, glm::vec3(0, 0, 1)) * camSpeed * FPSCounter::GetDeltaTime();
			vulkanManager.camPos.z += dir.z * camSpeed * FPSCounter::GetDeltaTime();

			// cam rotation
			double mousePosX, mousePosY;
			glfwGetCursorPos(glfwManager.GetWindow(), &mousePosX, &mousePosY);
			glm::vec2 mousePos = glm::vec2(mousePosX, mousePosY);

			glm::vec2 mouseDelta = lastMousePos - mousePos;

			if (glfwGetMouseButton(glfwManager.GetWindow(), GLFW_MOUSE_BUTTON_RIGHT))
			{
				vulkanManager.camDir = glm::rotate(vulkanManager.camDir, mouseDelta.x * lookSpeed * FPSCounter::GetDeltaTime(), glm::vec3(0, 0, 1));
				vulkanManager.camDir = glm::rotate(vulkanManager.camDir, mouseDelta.y * lookSpeed * FPSCounter::GetDeltaTime(), glm::cross(vulkanManager.camDir, glm::vec3(0, 0, 1)));
			}

			lastMousePos = mousePos;

			vulkanManager.GetImguiStuff()->StartFrame();

			GUI(&vulkanManager);

			vulkanManager.GetImguiStuff()->EndFrame();

			vulkanManager.Present(&glfwManager);

			FPSCounter::StopCounting();

			fpsUpdateTimer += FPSCounter::GetDeltaTime();
			if (fpsUpdateTimer >= timeToUpdateFPS)
			{
				fpsUpdateTimer = 0;
				std::stringstream ss;
				ss << "Average FPS: " << FPSCounter::GetAverageFPS()
					<< " FPS: " << FPSCounter::GetRawFPS()
					<< " DeltaTime: " << (int)FPSCounter::GetDeltaTime()
					<< " CamSpeed: " << camSpeed
					<< " Light dir: " << dir.x << " " << dir.y << " " << dir.z;
				glfwManager.SetWindowTitle(ss.str());
			}
		}
		vulkanManager.WaitForIdle();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}