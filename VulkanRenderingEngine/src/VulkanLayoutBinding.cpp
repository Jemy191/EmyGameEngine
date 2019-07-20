#include "VulkanLayoutBinding.h"

#include <stdexcept>
#include <iostream>

void VulkanLayoutBinding::AddLayoutBinding(VkDescriptorType descriptorType, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = static_cast<uint32_t>(layoutBindings.size());
	layoutBinding.descriptorCount = 1;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.stageFlags = stageFlags;

	layoutBindings.push_back(layoutBinding);
}

void VulkanLayoutBinding::Create(VkDevice device)
{
	this->device = device;
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VulkanLayoutBinding::~VulkanLayoutBinding()
{
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	std::cout << "LayoutBinding destroyed" << std::endl;

}

VkDescriptorSetLayout VulkanLayoutBinding::GetVkDescriptorSetLayout() const
{
	return descriptorSetLayout;
}
