#include "VulkanManager.h"
#include <iostream>

#include "VulkanHelper.h"
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <algorithm>

VulkanManager::VulkanManager(GLFWwindow* window, VkSampleCountFlagBits suggestedMsaaSamples)
{
	this->window = window;

	std::cout << "Start creating vulkan state" << std::endl;

	vulkanInstance = std::unique_ptr<VulkanInstance>(new VulkanInstance(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT));

	if (glfwCreateWindowSurface(vulkanInstance->GetInstance(), window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}

	physicalDevice = std::unique_ptr<VulkanPhysicalDevice>(new VulkanPhysicalDevice(vulkanInstance->GetInstance(), surface, suggestedMsaaSamples));
	logicalDevice = std::unique_ptr<VulkanLogicalDevice>(new VulkanLogicalDevice(physicalDevice->GetVKPhysicalDevice(), surface));

	VulkanHelper::QueueFamilyIndices queueFamilyIndices = VulkanHelper::FindQueueFamilies(physicalDevice->GetVKPhysicalDevice(), surface);
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	if (vkCreateCommandPool(logicalDevice->GetVKDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics command pool!");
	}

	renderPass = std::unique_ptr<VulkanRenderPass>(new VulkanRenderPass(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), surface, physicalDevice->GetMsaaSample()));
	swapChain = std::unique_ptr<VulkanSwapChain>(new VulkanSwapChain(window, logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), surface, physicalDevice->GetMsaaSample(), commandPool, logicalDevice->GetGraphicsQueue(), renderPass->GetVkRenderPass()));

	// TestMesh and shader
	vertexShader = std::unique_ptr<VulkanShader>(new VulkanShader(logicalDevice->GetVKDevice(), "vert", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT));
	fragShader = std::unique_ptr<VulkanShader>(new VulkanShader(logicalDevice->GetVKDevice(), "frag", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT));

	basicGraphicPipeline = std::unique_ptr <VulkanGraphicPipeline>(new VulkanGraphicPipeline());

	basicGraphicPipeline->AddShader(vertexShader.get());
	basicGraphicPipeline->AddShader(fragShader.get());

	basicGraphicPipeline->Create(logicalDevice->GetVKDevice(), swapChain->GetVkExtent2D(), renderPass->GetVkRenderPass(), physicalDevice->GetMsaaSample(), VkPolygonMode::VK_POLYGON_MODE_FILL);

	checkerTexture = std::unique_ptr<Texture>(new Texture(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "Checker.jpg"));
	debugTexture = std::unique_ptr<Texture>(new Texture(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "Debug.jpg"));

	textMesh = std::unique_ptr<Mesh>(new Mesh(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "CubeSuzaneConeSphere.obj", Mesh::MeshFormat::OBJ));
	cubeMesh = std::unique_ptr<Mesh>(new Mesh(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), "Cube.obj", Mesh::MeshFormat::OBJ));

	Model* testModel = new Model(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), swapChain->GetVkImages().size(), textMesh.get(), checkerTexture.get(), basicGraphicPipeline.get());
	testModel->position = glm::vec3(0.0);
	AddModelToList(testModel);

	Model* cubeModel = new Model(logicalDevice->GetVKDevice(), physicalDevice->GetVKPhysicalDevice(), swapChain->GetVkImages().size(), cubeMesh.get(), debugTexture.get(), basicGraphicPipeline.get());
	cubeModel->position = glm::vec3(3, 0, 0);
	AddModelToList(cubeModel);

	CreateCommandBuffer();

	CreateSyncObject();

	std::cout << "Vulkan created" << std::endl;
}

VulkanManager::~VulkanManager()
{
	textMesh.reset();
	swapChain.reset();
	modelList.clear();
	vkDestroySurfaceKHR(vulkanInstance->GetInstance(), surface, nullptr);
	vertexShader.reset();
	fragShader.reset();

	vkFreeCommandBuffers(logicalDevice->GetVKDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(logicalDevice->GetVKDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice->GetVKDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice->GetVKDevice(), inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(logicalDevice->GetVKDevice(), commandPool, nullptr);

	std::cout << "Vulkan destroyed" << std::endl;
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

	basicGraphicPipeline->AddShader(vertexShader.get());
	basicGraphicPipeline->AddShader(fragShader.get());

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

	vkFreeCommandBuffers(logicalDevice->GetVKDevice(), commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	CreateCommandBuffer();
}

void VulkanManager::Draw(GlfwManager* window)
{
	isDrawing = true;

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
		throw std::runtime_error("failed to acquire swap chain image!");
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

	if (vkQueueSubmit(logicalDevice->GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
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
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	isDrawing = false;
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
	auto it = std::find(modelList[model->graphicPipeline][model->mesh].begin(), modelList[model->graphicPipeline][model->mesh].end(), std::unique_ptr<Model>(model));

	if (it != modelList[model->graphicPipeline][model->mesh].end())
		modelList[model->graphicPipeline][model->mesh].erase(it);

	if (modelList[model->graphicPipeline][model->mesh].size() <= 0)
		modelList[model->graphicPipeline].erase(model->mesh);

	if (modelList[model->graphicPipeline].size() <= 0)
		modelList.erase(model->graphicPipeline);
}

VulkanInstance* VulkanManager::GetVulkanInstance()
{
	return vulkanInstance.get();
}

VkSurfaceKHR VulkanManager::GetVkSurfaceKHR()
{
	return surface;
}

void VulkanManager::WaitForIdle()
{
	vkDeviceWaitIdle(logicalDevice->GetVKDevice());
}

void VulkanManager::CreateCommandBuffer()
{
	commandBuffers.resize(swapChain->GetSwapChainFramebuffers().size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(logicalDevice->GetVKDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
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

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
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
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void VulkanManager::UpdateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	VulkanHelper::UniformBufferObject ubo = {};
	ubo.view = glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->GetVkExtent2D().width / (float)swapChain->GetVkExtent2D().height, 0.0001f, 100000.0f);
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
				ubo.model = glm::rotate(ubo.model, time * glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));

				modelIterator->get()->UpdateUniformBuffer(currentImage, &ubo);

				modelIterator++;
			}

			meshIterator++;
		}

		graphicPipelineIterator++;
	}
}
