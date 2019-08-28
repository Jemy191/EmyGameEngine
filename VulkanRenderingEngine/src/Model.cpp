#include "Model.h"
#include "Log.h"

Model::Model(VkDevice device, VkPhysicalDevice physicalDevice, size_t swapchainImagesSize, Mesh* mesh, Texture* texture, Texture* normalTexture, VulkanGraphicPipeline* graphicPipeline)
	: device(device), physicalDevice(physicalDevice), mesh(mesh), texture(texture), normalTexture(normalTexture), graphicPipeline(graphicPipeline)
{
	Create(swapchainImagesSize);
}

Model::~Model()
{
	Cleanup();
	Logger::Log("Model deleted");
}

void Model::Draw(VkCommandBuffer commandBuffer, int i)
{
	descriptor->CmdBind(commandBuffer, graphicPipeline->GetVkPipelineLayout(), i);
	mesh->CmdDraw(commandBuffer);
}

void Model::UpdateUniformBuffer(uint32_t currentImage, VulkanHelper::UniformBufferObject* ubo)
{
	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(*ubo), 0, &data);
	memcpy(data, ubo, sizeof(*ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}

void Model::Recreate(size_t swapchainImagesSize)
{
	Cleanup();
	Create(swapchainImagesSize);
}

void Model::Create(size_t swapchainImagesSize)
{
	// Uniform buffer
	VkDeviceSize bufferSize = sizeof(VulkanHelper::UniformBufferObject);

	uniformBuffers.resize(swapchainImagesSize);
	uniformBuffersMemory.resize(swapchainImagesSize);

	for (size_t i = 0; i < swapchainImagesSize; i++)
	{
		VulkanHelper::CreateBuffer(device, physicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}


	descriptor = std::unique_ptr<VulkanDescriptor>(new VulkanDescriptor(device, swapchainImagesSize, uniformBuffers, graphicPipeline->layoutBinding.GetVkDescriptorSetLayout(), texture, normalTexture));
}

void Model::Cleanup()
{
	for (size_t i = 0; i < uniformBuffers.size(); i++)
	{
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}
	descriptor.reset();
}
