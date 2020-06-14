#include "Helper/Log.h"
#include <sstream>

#include <Rendering/GlfwManager.h>
#include <Rendering/Vulkan/VulkanRenderer.h>
#include <Rendering/OpenGL/OpenGLRenderer.h>  

#include <Helper/FPSCounter.h>
#include <glm/gtx/rotate_vector.hpp>
#include <json.hpp>
#include <fstream>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>
#include <Scene/Scene.h>
#include "Rendering/UI/ImguiBase.h"
#include "Imgui/imgui_internal.h"
#include <filesystem>
#include "Asset/AssetManager.h"
#include <Game/Setting.h>
#include <memory>

float camSpeed = 5;
float lookSpeed = 0.5f;

char meshToLoadInput[64] = "";
char textureToLoadInput[64] = "";
std::string sceneToLoad = "NewScene";

bool demoWindowOpen = false;
bool settingWindowOpen = false;
bool hierarchyWindowOpen = true;
bool statWindowOpen = false;

void GUI()
{
	if (demoWindowOpen)
	{
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		ImGui::ShowDemoWindow(&demoWindowOpen);
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Setting"))
				settingWindowOpen = true;
			if (ImGui::MenuItem("Stat"))
				statWindowOpen = true;
			if (ImGui::MenuItem("Demo"))
				demoWindowOpen = true;
			if (ImGui::MenuItem("Scene"))
				hierarchyWindowOpen = true;

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (settingWindowOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Setting Window", &settingWindowOpen))
		{
			ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

			if (ImGui::CollapsingHeader("Setting"))
			{
				ImGui::SliderFloat("Cam speed", &camSpeed, 1, 100);
				ImGui::SliderFloat("Look speed", &lookSpeed, 0.1f, 1);

				//ImGui::SliderFloat3("Light dir", &vulkanManager->lightDir.x, -1, 1);

				//ImGui::SliderFloat("Light shininess", &renderer->lightSetting.x, 1, 100);
				//ImGui::SliderFloat("Light spec", &renderer->lightSetting.y, 0.1f, 10);

				//ImGui::ColorEdit3("Light color", &vulkanManager->lightColor.x);
			}
		}
		ImGui::End();
	}

	if (hierarchyWindowOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Hierarchy Window", &hierarchyWindowOpen))
		{
			ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

			ImGui::InputTextWithHint("##LoadScene", "Scene to create/load", &sceneToLoad);

			if (sceneToLoad != "")
			{
				std::string buttonText = std::filesystem::exists("Assets/Scenes/" + sceneToLoad + ".json") ? "Load" : "Create";
				ImGui::SameLine();

				if (ImGui::Button(buttonText.c_str()))
				{
					if (Scene::GetCurrentScene() != nullptr)
						delete Scene::GetCurrentScene();
					new Scene(sceneToLoad);
					sceneToLoad = "";
				}
			}

			Scene* scene = Scene::GetCurrentScene();
			if (scene != nullptr)
				scene->GUI();
		}
		ImGui::End();
	}

	if (statWindowOpen)
	{
		if (ImGui::Begin("Stat", &statWindowOpen))
		{
			std::stringstream ss;
			ss << "DeltaTime: " << std::setprecision(2) << std::setw(2) << FPSCounter::GetDeltaTime();

			ImGui::Text(("FPS: " + std::to_string(FPSCounter::GetRawFPS())).c_str());
			ImGui::Text(ss.str().c_str());
		}
		ImGui::End();
	}

	AssetManager::GUI();
}

#if CONSOLE
int main()
#else
int WinMain()
#endif
{
	try
	{
		Logger::Open();
		Setting::Load();
		AssetManager::Init();

		GlfwManager glfwManager = GlfwManager(1600, 900, "TestGame");

		std::unique_ptr<Renderer> renderer = Renderer::CreateGraphicalAPI(Setting::Get("GraphicAPI", GraphicalAPI::OPENGL).get<GraphicalAPI>(), glfwManager.GetWindow());

		double mousePosX, mousePosY;
		glfwGetCursorPos(glfwManager.GetWindow(), &mousePosX, &mousePosY);
		glm::vec2 lastMousePos = glm::vec2(mousePosX, mousePosY);

		while (!glfwWindowShouldClose(glfwManager.GetWindow()))
		{
			FPSCounter::StartCounting();
			glfwPollEvents();

			//if (!ImGui::GetCurrentContext()->NavWindow)
			//{
			//	//cam movement
			//	glm::vec3 dir = glm::vec3(0);

			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_ESCAPE))
			//		break;

			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_W))
			//		dir.y = 1;
			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_A))
			//		dir.x = -1;

			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_S))
			//		dir.y = -1;
			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_D))
			//		dir.x = 1;

			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_Q))
			//		dir.z = -1;
			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_E))
			//		dir.z = 1;

			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_PAGE_UP))
			//		camSpeed += 10 * FPSCounter::GetDeltaTime();
			//	if (glfwGetKey(glfwManager.GetWindow(), GLFW_KEY_PAGE_DOWN))
			//		camSpeed += -10 * FPSCounter::GetDeltaTime();

			//	renderer->camPos += dir.y * renderer->camDir * camSpeed * FPSCounter::GetDeltaTime();
			//	renderer->camPos += dir.x * glm::cross(renderer->camDir, glm::vec3(0, 0, 1)) * camSpeed * FPSCounter::GetDeltaTime();
			//	renderer->camPos.z += dir.z * camSpeed * FPSCounter::GetDeltaTime();

			//	// cam rotation
			//	double mousePosX, mousePosY;
			//	glfwGetCursorPos(glfwManager.GetWindow(), &mousePosX, &mousePosY);
			//	glm::vec2 mousePos = glm::vec2(mousePosX, mousePosY);

			//	glm::vec2 mouseDelta = lastMousePos - mousePos;

			//	glm::vec3 rot(0);
			//	if (glfwGetMouseButton(glfwManager.GetWindow(), GLFW_MOUSE_BUTTON_RIGHT))
			//	{
			//		renderer->camDir = glm::rotate(renderer->camDir, mouseDelta.x * lookSpeed * FPSCounter::GetDeltaTime(), glm::vec3(0, 0, 1));
			//		renderer->camDir = glm::rotate(renderer->camDir, mouseDelta.y * lookSpeed * FPSCounter::GetDeltaTime(), glm::cross(renderer->camDir, glm::vec3(0, 0, 1)));
			//	}

			//	lastMousePos = mousePos;
			//}

			renderer->GetImgui()->StartFrame();

			if (Scene::GetCurrentScene() != nullptr)
				Scene::GetCurrentScene()->Update();

			GUI();

			renderer->GetImgui()->EndFrame();

			renderer->Present(&glfwManager);

			FPSCounter::StopCounting();
		}

		renderer->WaitForIdle();

		Setting::Save();
	}
	catch (const std::exception& e)
	{
		Logger::Log(LogSeverity::ERROR, e.what());
		return 1;
	}

	return 0;
}