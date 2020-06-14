#pragma once
#include "Header/GLFWHeader.h"

#include "Rendering/Vulkan/VulkanHelper.h"

#include <glm/glm.hpp>
#include <string>

class Mesh
{
public:
	enum MeshFormat
	{
		OBJ,
		GLTF
	};
private:
	static const std::string PATH;

	std::vector<VulkanHelper::Vertex> vertices;
	std::vector<uint32_t> indices;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

public:
	Mesh(std::string meshName, MeshFormat meshFormat, bool isGltfBinary = true);
	~Mesh();

	void CmdBind(VkCommandBuffer commandBuffer);
	void CmdDraw(VkCommandBuffer commandBuffer);

private:
	void GltfLoader(std::string& meshPath, bool isGltfBinary);
	void ObjLoader(std::string& meshPath);

};