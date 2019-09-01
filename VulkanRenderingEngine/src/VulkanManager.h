#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <map>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicPipeline.h"
#include "VulkanShader.h"
#include "Texture.h"
#include "Mesh.h"
#include "VulkanDescriptor.h"
#include "GlfwManager.h"
#include "Model.h"
#include "ImguiStuff.h"

class VulkanManager
{
public:
	static const int MAX_FRAMES_IN_FLIGHT = 2;
	glm::vec3 clearColor = glm::vec3(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f);

	glm::vec3 camPos = glm::vec3(0, 5.0f, 0.0f);
	glm::vec3 camDir = glm::vec3(0, -1, 0);
	glm::vec3 lightDir = glm::vec3(0.1f, 1, 1);
	glm::vec2 lightSetting = glm::vec2(5, 1);
	glm::vec3 lightColor = glm::vec3(1);

private:
	static VulkanManager* instance;

	GLFWwindow* window;

	std::unique_ptr<VulkanInstance> vulkanInstance;
	VkSurfaceKHR surface = nullptr;
	std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
	std::unique_ptr<VulkanLogicalDevice> logicalDevice;
	std::unique_ptr<VulkanSwapChain> swapChain;
	std::unique_ptr<VulkanRenderPass> renderPass;

	//TODO: Change this so we can have multiple and take ref in mesh
	std::unique_ptr<VulkanShader> baseVertexShader;
	std::unique_ptr<VulkanShader> baseFragShader;
	std::unique_ptr<VulkanShader> textureColorFragShader;
	std::unique_ptr <VulkanGraphicPipeline> basicGraphicPipeline;
	std::unique_ptr <VulkanGraphicPipeline> textureColorGraphicPipeline;

	std::unique_ptr<ImguiStuff> imguiStuff;

	std::map<VulkanGraphicPipeline*, std::map<Mesh*, std::vector<std::unique_ptr<Model>>>> modelList;

	std::unique_ptr<Texture> checkerTexture;
	std::unique_ptr<Texture> debugTexture;
	std::unique_ptr<Texture> debugNormalTexture;
	std::unique_ptr<Texture> testNormalTexture;
	std::unique_ptr<Mesh> skyboxMesh;
	std::unique_ptr<Mesh> cubeMesh;
	std::unique_ptr<Mesh> planeMesh;

	std::vector<Mesh*> meshList = std::vector<Mesh*>();
	std::vector<Texture*> textureList = std::vector<Texture*>();

	//TODO: Move this
	std::vector <VkCommandPool> drawCommandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkCommandPool commandPool;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	size_t currentFrame = 0;

	std::vector<Model*> modelToBeRemove;

public:
	VulkanManager(GLFWwindow* window, VkSampleCountFlagBits msaaSamples);
	~VulkanManager();

	void RecreateSwapChain();

	void Present(GlfwManager* window);

	Model* BasicLoadModel(std::string meshName, std::string textureName, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	void MarkModelToBeRemove(Model* model);

	void AddModelToList(Model* model);

	VulkanInstance* GetVulkanInstance() const;
	VkSurfaceKHR GetVkSurfaceKHR() const;
	ImguiStuff* GetImguiStuff() const;
	void WaitForIdle();

	static VulkanManager* GetInstance();

private:
	void RemoveModelFromList(Model* model);
	void Draw();

	void CreateCommandBuffer();// TODO: Not here?
	void CreateSyncObject();// TODO: Not here?
	void UpdateUniformBuffer(uint32_t currentImage);// TODO: Not here?
};