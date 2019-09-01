#include "VulkanManager.h"

#include "Log.h"

#include "VulkanHelper.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <chrono>
#include <algorithm>

VulkanManager* VulkanManager::instance = nullptr;

VulkanManager::VulkanManager(GLFWwindow* window, VkSampleCountFlagBits suggestedMsaaSamples)
{
	if (instance != nullptr)
		Logger::Log(LogSeverity::FATAL_ERROR, "There cant be 2 VulkanManager");

	instance = this;
	this->window = window;

	Logger::Log("Start creating vulkan state");

	Logger::Log("Creating instance");
	vulkanInstance = std::unique_ptr<VulkanInstance>(new VulkanInstance(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT));

	Logger::Log("Creating surface");
	if (glfwCreateWindowSurface(vulkanInstance->GetInstance(), window, nullptr, &surface) != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to create window surface!");
	}

	Logger::Log("Creating physicalDevice");
	physicalDevice = std::unique_ptr<VulkanPhysicalDevice>(new VulkanPhysicalDevice(vulkanInstance->GetInstance(), surface, suggestedMsaaSamples));

	Logger::Log("Creating logicalDevice");
	logicalDevice = std::unique_ptr<VulkanLogicalDevice>(new VulkanLogicalDevice(physicalDevice->GetVKPhysicalDevice(), surface));

	Logger::Log("Creating commandPool");
	VulkanHelper::QueueFamilyIndices queueFamilyIndices = VulkanHelper::FindQueueFamilies(physicalDevice->GetVKPhysicalDevice(), surface);
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	if (vkCreateCommandPool(logicalDevice->GetVKDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to create graphics command pool!");
	}

	Logger::Log("Creating renderPass");
	renderPass = std::unique_ptr<VulkanRenderPass>(new VulkanRenderPass(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), surface, physicalDevice->GetMsaaSample()));

	Logger::Log("Creating swapChain");
	swapChain = std::unique_ptr<VulkanSwapChain>(new VulkanSwapChain(window, logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), surface, physicalDevice->GetMsaaSample(), commandPool, logicalDevice->GetGraphicsQueue(), renderPass->GetVkRenderPass()));

	Logger::Log("Creating drawCommandPools");
	drawCommandPool.resize(swapChain->GetSwapChainFramebuffers().size());
	for (size_t i = 0; i < swapChain->GetSwapChainFramebuffers().size(); i++)
	{
		if (vkCreateCommandPool(logicalDevice->GetVKDevice(), &poolInfo, nullptr, &drawCommandPool[i]) != VK_SUCCESS)
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "failed to create graphics command pool!");
		}
	}

	Logger::Log("Creating imgui");
	uint32_t minImageCount = VulkanHelper::QuerySwapChainSupport(physicalDevice->GetVKPhysicalDevice(), surface).capabilities.minImageCount + 1;
	imguiStuff = std::unique_ptr<ImguiStuff>(new ImguiStuff(logicalDevice->GetVKDevice(), window, vulkanInstance->GetInstance(), physicalDevice->GetVKPhysicalDevice(), renderPass->GetVkRenderPass(), queueFamilyIndices.graphicsFamily.value(), logicalDevice->GetGraphicsQueue(), commandPool, (uint32_t)(swapChain->GetVkImages().size()), minImageCount));

	Logger::Log("Creating test shader");
	// TestMesh and shader
	baseVertexShader = std::unique_ptr<VulkanShader>(new VulkanShader(logicalDevice->GetVKDevice(), "BaseVert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT));
	baseFragShader = std::unique_ptr<VulkanShader>(new VulkanShader(logicalDevice->GetVKDevice(), "BaseFrag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT));
	textureColorFragShader = std::unique_ptr<VulkanShader>(new VulkanShader(logicalDevice->GetVKDevice(), "TextureColorFrag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT));

	Logger::Log("Creating test GraphicPipeline");
	basicGraphicPipeline = std::unique_ptr <VulkanGraphicPipeline>(new VulkanGraphicPipeline());
	basicGraphicPipeline->AddShader(baseVertexShader.get());
	basicGraphicPipeline->AddShader(baseFragShader.get());
	basicGraphicPipeline->Create(logicalDevice->GetVKDevice(), swapChain->GetVkExtent2D(), renderPass->GetVkRenderPass(), physicalDevice->GetMsaaSample(), VkPolygonMode::VK_POLYGON_MODE_FILL);

	textureColorGraphicPipeline = std::unique_ptr <VulkanGraphicPipeline>(new VulkanGraphicPipeline());
	textureColorGraphicPipeline->AddShader(baseVertexShader.get());
	textureColorGraphicPipeline->AddShader(textureColorFragShader.get());
	textureColorGraphicPipeline->Create(logicalDevice->GetVKDevice(), swapChain->GetVkExtent2D(), renderPass->GetVkRenderPass(), physicalDevice->GetMsaaSample(), VkPolygonMode::VK_POLYGON_MODE_FILL);

	Logger::Log("Creating test texture");
	checkerTexture = std::unique_ptr<Texture>(new Texture(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "Checker.jpg"));
	debugTexture = std::unique_ptr<Texture>(new Texture(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "Debug.jpg"));
	debugNormalTexture = std::unique_ptr<Texture>(new Texture(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "DebugNormalMap.jpg"));
	//testNormalTexture = std::unique_ptr<Texture>(new Texture(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "TestNormalMap.png"));

	Logger::Log("Creating test skybox");
	skyboxMesh = std::unique_ptr<Mesh>(new Mesh(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "SkyBoxTest.obj", Mesh::MeshFormat::OBJ));
	//cubeMesh = std::unique_ptr<Mesh>(new Mesh(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "MultiCube.obj", Mesh::MeshFormat::OBJ));
	//planeMesh = std::unique_ptr<Mesh>(new Mesh(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "Plane.obj", Mesh::MeshFormat::OBJ));
	//
	Model* testModel = new Model(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), swapChain->GetVkImages().size(), skyboxMesh.get(), debugTexture.get(), debugNormalTexture.get(), textureColorGraphicPipeline.get());
	testModel->position = glm::vec3(0);
	AddModelToList(testModel);

	//
	//Model* cubeModel = new Model(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), swapChain->GetVkImages().size(), cubeMesh.get(), debugTexture.get(), debugNormalTexture.get(), basicGraphicPipeline.get());
	//cubeModel->position = glm::vec3(0);
	//AddModelToList(cubeModel);
	//models.push_back(cubeModel);
	//
	//Model* planeModel = new Model(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), swapChain->GetVkImages().size(), planeMesh.get(), debugTexture.get(), debugNormalTexture.get(), basicGraphicPipeline.get());
	//planeModel->position = glm::vec3(0, 0, -3);
	//AddModelToList(planeModel);

	Logger::Log("Creating Command buffer");
	CreateCommandBuffer();

	Logger::Log("Creating sync object");
	CreateSyncObject();

	Logger::Log("Vulkan created");
}

VulkanManager::~VulkanManager()
{
	skyboxMesh.reset();
	swapChain.reset();
	modelList.clear();
	vkDestroySurfaceKHR(vulkanInstance->GetInstance(), surface, nullptr);
	baseVertexShader.reset();
	baseFragShader.reset();

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		vkFreeCommandBuffers(logicalDevice->GetVKDevice(), drawCommandPool[i], 1 , &commandBuffers[i]);
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(logicalDevice->GetVKDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice->GetVKDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice->GetVKDevice(), inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(logicalDevice->GetVKDevice(), commandPool, nullptr);

	for (size_t i = 0; i < drawCommandPool.size(); i++)
	{
		vkDestroyCommandPool(logicalDevice->GetVKDevice(), drawCommandPool[i], nullptr);
	}

	for (size_t i = 0; i < meshList.size(); i++)
	{
		delete meshList[i];
	}
	meshList.clear();
	for (size_t i = 0; i < textureList.size(); i++)
	{
		delete textureList[i];
	}
	textureList.clear();

	Logger::Log("Vulkan destroyed");
}

void VulkanManager::RecreateSwapChain()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logicalDevice->GetVKDevice());

	renderPass.reset(new VulkanRenderPass(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), surface, physicalDevice->GetMsaaSample()));
	swapChain.reset(new VulkanSwapChain(window, logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), surface, physicalDevice->GetMsaaSample(), commandPool, logicalDevice->GetGraphicsQueue(), renderPass->GetVkRenderPass()));

	basicGraphicPipeline.reset(new VulkanGraphicPipeline());

	basicGraphicPipeline->AddShader(baseVertexShader.get());
	basicGraphicPipeline->AddShader(baseFragShader.get());

	basicGraphicPipeline->Create(logicalDevice->GetVKDevice(), swapChain->GetVkExtent2D(), renderPass->GetVkRenderPass(), physicalDevice->GetMsaaSample(), VkPolygonMode::VK_POLYGON_MODE_FILL);

	//TODO: make a function for that
	// Recreate stuff in model
	auto graphicPipelineIterator = modelList.begin();

	while (graphicPipelineIterator != modelList.end())
	{
		auto meshIterator = graphicPipelineIterator->second.begin();

		while (meshIterator != graphicPipelineIterator->second.end())
		{
			auto modelIterator = meshIterator->second.begin();

			while (modelIterator != meshIterator->second.end())
			{
				modelIterator->get()->Recreate(swapChain->GetVkImages().size());

				modelIterator++;
			}

			meshIterator++;
		}

		graphicPipelineIterator++;
	}

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		vkFreeCommandBuffers(logicalDevice->GetVKDevice(), drawCommandPool[i], 1, &commandBuffers[i]);
	}
	CreateCommandBuffer();
}

void VulkanManager::Draw()
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		vkResetCommandPool(logicalDevice->GetVKDevice(), drawCommandPool[i], 0);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->GetVkRenderPass();
		renderPassInfo.framebuffer = swapChain->GetSwapChainFramebuffers()[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChain->GetVkExtent2D();

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {clearColor.r, clearColor.g, clearColor.b, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//TODO: make a function for that
		// Draw model
		auto graphicPipelineIterator = modelList.begin();

		while (graphicPipelineIterator != modelList.end())
		{
			auto meshIterator = graphicPipelineIterator->second.begin();

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicPipelineIterator->first->GetVkPipeline());

			while (meshIterator != graphicPipelineIterator->second.end())
			{
				auto modelIterator = meshIterator->second.begin();

				meshIterator->first->CmdBind(commandBuffers[i]);

				while (modelIterator != meshIterator->second.end())
				{
					modelIterator->get()->Draw(commandBuffers[i], i);

					modelIterator++;
				}

				meshIterator++;
			}

			graphicPipelineIterator++;
		}

		imguiStuff->Draw(commandBuffers[i]);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "failed to record command buffer!");
		}
	}
}

void VulkanManager::Present(GlfwManager* window)
{
	// Remove model from model list
	if (modelToBeRemove.size() != 0)
	{
		for (size_t i = 0; i < modelToBeRemove.size(); i++)
		{
			RemoveModelFromList(modelToBeRemove[i]);
		}
		modelToBeRemove.clear();
	}

	vkWaitForFences(logicalDevice->GetVKDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice->GetVKDevice(), swapChain->GetVkSwapchainKHR(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to acquire swap chain image!");
	}

	UpdateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(logicalDevice->GetVKDevice(), 1, &inFlightFences[currentFrame]);
	
	Draw();
	if (vkQueueSubmit(logicalDevice->GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapChain->GetVkSwapchainKHR()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(logicalDevice->GetPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->HasWindowResize())
	{
		window->ResetWindowHasResize();
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		Logger::Log(LogSeverity::FATAL_ERROR, "failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


	vkQueueWaitIdle(logicalDevice->GetPresentQueue());
}

Model* VulkanManager::BasicLoadModel(std::string meshName, std::string textureName, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	Mesh* newMesh = new Mesh(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), meshName + ".obj", Mesh::MeshFormat::OBJ);
	meshList.push_back(newMesh);

	Texture* newTexture = new Texture(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), textureName);
	textureList.push_back(newTexture);

	Model* testModel = new Model(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), swapChain->GetVkImages().size(), newMesh, newTexture, debugNormalTexture.get(), basicGraphicPipeline.get());

	testModel->position = position;
	testModel->rotation = rotation;
	testModel->scale = scale;
	testModel->meshName = meshName;
	testModel->textureName = textureName;

	AddModelToList(testModel);

	return testModel;
}

void VulkanManager::MarkModelToBeRemove(Model* model)
{
	modelToBeRemove.push_back(model);
}

void VulkanManager::AddModelToList(Model* model)
{
	using namespace std;
	if (modelList.count(model->graphicPipeline) == 0)
	{
		modelList.insert(pair<VulkanGraphicPipeline*, map<Mesh*, vector<unique_ptr<Model>>>>(
			model->graphicPipeline, map<Mesh*, vector<unique_ptr<Model>>>()
			));
	}

	if (modelList[model->graphicPipeline].count(model->mesh) == 0)
	{
		modelList[model->graphicPipeline].insert(pair<Mesh*, vector<unique_ptr<Model>>>(
			model->mesh, vector<unique_ptr<Model>>()
			));
	}

	modelList[model->graphicPipeline][model->mesh].push_back(unique_ptr<Model>(model));
}

void VulkanManager::RemoveModelFromList(Model* model)
{
	for (size_t i = 0; i < modelList[model->graphicPipeline][model->mesh].size(); i++)
	{
		if (modelList[model->graphicPipeline][model->mesh][i].get() == model)
		{
			modelList[model->graphicPipeline][model->mesh].erase(modelList[model->graphicPipeline][model->mesh].begin() + i);
			break;
		}
	}

	if (modelList[model->graphicPipeline][model->mesh].size() <= 0)
		modelList[model->graphicPipeline].erase(model->mesh);

	if (modelList[model->graphicPipeline].size() <= 0)
		modelList.erase(model->graphicPipeline);
}

VulkanInstance* VulkanManager::GetVulkanInstance() const
{
	return vulkanInstance.get();
}

VkSurfaceKHR VulkanManager::GetVkSurfaceKHR() const
{
	return surface;
}

ImguiStuff* VulkanManager::GetImguiStuff() const
{
	return imguiStuff.get();
}

void VulkanManager::WaitForIdle()
{
	vkDeviceWaitIdle(logicalDevice->GetVKDevice());
}

VulkanManager* VulkanManager::GetInstance()
{
	return instance;
}

void VulkanManager::CreateCommandBuffer()
{
	commandBuffers.resize(swapChain->GetSwapChainFramebuffers().size());

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = drawCommandPool[i];
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(logicalDevice->GetVKDevice(), &allocInfo, &commandBuffers[i]) != VK_SUCCESS)
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "failed to allocate command buffers!");
		}
	}
}

void VulkanManager::CreateSyncObject()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(logicalDevice->GetVKDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice->GetVKDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice->GetVKDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "failed to create synchronization objects for a frame!");
		}
	}
}

void VulkanManager::UpdateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	VulkanHelper::UniformBufferObject ubo = {};
	ubo.viewPos = camPos;
	ubo.lightDir = lightDir;
	ubo.lightColor = lightColor;
	ubo.lightSetting = lightSetting;
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->GetVkExtent2D().width / (float)swapChain->GetVkExtent2D().height, 0.0001f, 100000.0f);
	ubo.view = glm::lookAt(camPos, camPos + glm::normalize(camDir), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj[1][1] *= -1;

	//TODO: make a function for that
	// Recreate stuff in model
	auto graphicPipelineIterator = modelList.begin();

	while (graphicPipelineIterator != modelList.end())
	{
		auto meshIterator = graphicPipelineIterator->second.begin();

		while (meshIterator != graphicPipelineIterator->second.end())
		{
			auto modelIterator = meshIterator->second.begin();

			while (modelIterator != meshIterator->second.end())
			{
				ubo.model = glm::translate(glm::mat4(1.0), modelIterator->get()->position);
				ubo.model *= glm::mat4_cast(glm::quat(glm::radians(modelIterator->get()->rotation)));
				ubo.model = glm::scale(ubo.model, modelIterator->get()->scale);

				modelIterator->get()->UpdateUniformBuffer(currentImage, &ubo);

				modelIterator++;
			}

			meshIterator++;
		}

		graphicPipelineIterator++;
	}
}
