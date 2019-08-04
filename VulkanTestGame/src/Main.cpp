#include <iostream>
#include <sstream>

#include <GlfwManager.h>
#include <VulkanManager.h>
#include <FPSCounter.h>
#include <glm/gtx/rotate_vector.hpp>

bool demoWindowOpen = true;
void GUI()
{
	ImGui::ShowDemoWindow(&demoWindowOpen);
}

#if DEBUG
int main()
#else
int WinMain()
#endif
{

	try
	{
		GlfwManager glfwManager = GlfwManager(800, 600, "TestGame");
		VulkanManager vulkanManager(glfwManager.GetWindow(), VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT);
		float timeToUpdateFPS = 0.25f;
		float fpsUpdateTimer = timeToUpdateFPS;

		float camSpeed = 20;
		float lookSpeed = 0.5f;

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

			// light movement
			dir = glm::vec3(0);
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_I))
				dir.y = 1;
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_J))
				dir.x = -1;

			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_K))
				dir.y = -1;
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_L))
				dir.x = 1;

			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_U))
				dir.z = -1;
			if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_O))
				dir.z = 1;

			vulkanManager.lightPos += dir * camSpeed * FPSCounter::GetDeltaTime();

			vulkanManager.GetImguiStuff()->StartFrame();

			GUI();

			vulkanManager.GetImguiStuff()->EndFrame();

			vulkanManager.Present(&glfwManager);

			FPSCounter::StopCounting();

			fpsUpdateTimer += FPSCounter::GetDeltaTime();
			if (fpsUpdateTimer >= timeToUpdateFPS)
			{
				fpsUpdateTimer = 0;
				std::stringstream ss;
				ss  << "Average FPS: " << FPSCounter::GetAverageFPS()
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