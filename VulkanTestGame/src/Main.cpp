#include <iostream>
#include <sstream>

#include <GlfwManager.h>
#include <VulkanManager.h>
#include <FPSCounter.h>
#include <glm/gtx/rotate_vector.hpp>
#include <json.hpp>
#include <fstream>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>

bool demoWindowOpen = true;
float timeToUpdateFPS = 0.25f;
float fpsUpdateTimer = timeToUpdateFPS;

float camSpeed = 20;
float lookSpeed = 0.5f;

char meshToLoadInput[64] = "";
char textureToLoadInput[64] = "";

bool showDemoWindow = false;
void GUI(VulkanManager* vulkanManager)
{
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
	ImGui::ShowDemoWindow(&demoWindowOpen);

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Setting Window"))
	{
		ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

		if (ImGui::CollapsingHeader("Setting"))
		{
			ImGui::SliderFloat("Cam speed", &camSpeed, 1, 100);
			ImGui::SliderFloat("Light dir", &vulkanManager->lightDir.x, -1, 1);
			//ImGui::SliderFloat("Light shininess", &vulkanManager->lightSetting.x, 1, 100);
			//ImGui::SliderFloat("Light spec", &vulkanManager->lightSetting.y, 0.1f, 10);
			ImGui::ColorEdit3("Light color", &vulkanManager->lightColor.x);
		}

		if (ImGui::CollapsingHeader("Import model"))
		{
			ImGui::InputText("MeshToLoad", meshToLoadInput, IM_ARRAYSIZE(meshToLoadInput));
			ImGui::InputText("TextureToLoad", textureToLoadInput, IM_ARRAYSIZE(textureToLoadInput));
			if (ImGui::Button("Load model"))
				vulkanManager->BasicLoadModel(std::string(meshToLoadInput), std::string(textureToLoadInput), glm::vec3(0), glm::vec3(0), glm::vec3(1));
		}

		if (ImGui::CollapsingHeader("Model setting"))
		{
			for (size_t i = 0; i < vulkanManager->models.size(); i++)
			{
				string label = "Model: " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					ImGui::DragFloat3("Position", &vulkanManager->models[i]->position.x, 0.1f, -20, 20);
					ImGui::DragFloat3("rotation", &vulkanManager->models[i]->rotation.x, 0.1f, 0, 360);
					ImGui::DragFloat3("scale", &vulkanManager->models[i]->scale.x, 0.1f, -5, 5);
					ImGui::TreePop();
				}
			}
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

	//Load scene
	using json = nlohmann::json;

	std::ifstream input("Assets/Scenes/Test.json");

	json scene;
	input >> scene;

	for (auto model : scene["scene"]["models"])
	{
		glm::vec3 position(model["position"]["x"], model["position"]["y"], model["position"]["z"]);
		glm::vec3 rotation(model["rotation"]["x"], model["rotation"]["y"], model["rotation"]["z"]);
		glm::vec3 scale(model["scale"]["x"], model["scale"]["y"], model["scale"]["z"]);

		vulkanManager.BasicLoadModel(model["mesh"], model["texture"], position, rotation, scale);
	}

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
					<< " DeltaTime: " << (int)FPSCounter::GetDeltaTime();
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

	// Save scene
	json outScene;

	for (size_t i = 0; i < vulkanManager.models.size(); i++)
	{
		json model;
		model["position"]["x"] = vulkanManager.models[i]->position.x;
		model["position"]["y"] = vulkanManager.models[i]->position.y;
		model["position"]["z"] = vulkanManager.models[i]->position.z;

		model["rotation"]["x"] = vulkanManager.models[i]->rotation.x;
		model["rotation"]["y"] = vulkanManager.models[i]->rotation.y;
		model["rotation"]["z"] = vulkanManager.models[i]->rotation.z;

		model["scale"]["x"] = vulkanManager.models[i]->scale.x;
		model["scale"]["y"] = vulkanManager.models[i]->scale.y;
		model["scale"]["z"] = vulkanManager.models[i]->scale.z;

		model["mesh"] = vulkanManager.models[i]->meshName;
		model["texture"] = vulkanManager.models[i]->textureName;

		outScene["scene"]["models"].push_back(model);
	}
	
	std::ofstream output("Assets/Scenes/Test.json");
	output << std::setw(4) << outScene;

	return 0;
}