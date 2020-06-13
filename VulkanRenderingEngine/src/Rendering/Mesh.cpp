#include "Rendering/Mesh.h"

#include "Helper/Log.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION // Already in Texture.h
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <unordered_map>
#include <array>
#include "Rendering/Vulkan/VulkanManager.h"

const std::string Mesh::PATH = "Assets/Meshs/";

Mesh::Mesh(std::string meshName, MeshFormat meshFormat, bool isGltfBinary)
{
	VkDevice device = VulkanManager::GetInstance()->GetLogicalDevice()->GetVk();
	VkPhysicalDevice physicalDevice = VulkanManager::GetInstance()->GetPhysicalDevice()->GetVk();
	VkQueue graphicQueue = VulkanManager::GetInstance()->GetLogicalDevice()->GetGraphicsQueue();
	VkCommandPool globalCommandPool = VulkanManager::GetInstance()->GetGlobalCommandPool();

	// load mesh
	std::string meshPath = PATH + meshName;

	switch (meshFormat)
	{
		case Mesh::OBJ:
			ObjLoader(meshPath);
			break;
		case Mesh::GLTF:
			GltfLoader(meshPath, isGltfBinary); // Not finished do not load
			break;
	}

	//Create buffer

	// vertex
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanHelper::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	VulkanHelper::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	VulkanHelper::CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

	// Index buffer
	bufferSize = sizeof(indices[0]) * indices.size();

	VulkanHelper::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	VulkanHelper::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	VulkanHelper::CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void Mesh::GltfLoader(std::string& meshPath, bool isGltfBinary)
{
	/*using namespace tinygltf;
	Model model;
	TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret;

	if (isGltfBinary)
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, meshPath);
	else
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, meshPath);

	if (!warn.empty())
		Logger::Log(LogSeverity::FATAL_ERROR, "Warn: " + warn);
	if (!err.empty())
		Logger::Log(LogSeverity::FATAL_ERROR, "Err: " + err);
	if (!ret)
		Logger::Log(LogSeverity::FATAL_ERROR, "Failed to parse glTF");*/

	//model.meshes[0].primitives[0].
}

void Mesh::ObjLoader(std::string& meshPath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, meshPath.c_str()))
	{
		Logger::Log(LogSeverity::FATAL_ERROR, warn + err);
	}

	std::unordered_map<VulkanHelper::Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			VulkanHelper::Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (attrib.normals.size() <= 0)
				vertex.normal = glm::vec3(1);
			else
			{
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (attrib.texcoords.size() <= 0)
				vertex.texCoord = {0.0f, 1.0f};
			else
			{
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			// No vertex color
			vertex.color = {1.0f, 1.0f, 1.0f};

			vertex.tangent = glm::vec3(0);
			vertex.biTangent = glm::vec3(0);

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	std::vector<std::vector<glm::vec3>> tangents = std::vector<std::vector<glm::vec3>>(vertices.size(), std::vector<glm::vec3>());
	std::vector<std::vector<glm::vec3>> biTangents = std::vector<std::vector<glm::vec3>>(vertices.size(), std::vector<glm::vec3>());

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		VulkanHelper::Vertex& vertex0 = vertices[indices[i + 0]];
		VulkanHelper::Vertex& vertex1 = vertices[indices[i + 1]];
		VulkanHelper::Vertex& vertex2 = vertices[indices[i + 2]];

		// Edges of the triangle : position delta
		glm::vec3 deltaPos1 = vertex1.pos - vertex0.pos;
		glm::vec3 deltaPos2 = vertex2.pos - vertex0.pos;

		// UV delta
		glm::vec2 deltaUV1 = vertex1.texCoord - vertex0.texCoord;
		glm::vec2 deltaUV2 = vertex2.texCoord - vertex0.texCoord;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 biTangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents[indices[i + 0]].push_back(tangent);
		tangents[indices[i + 1]].push_back(tangent);
		tangents[indices[i + 2]].push_back(tangent);
		biTangents[indices[i + 0]].push_back(biTangent);
		biTangents[indices[i + 1]].push_back(biTangent);
		biTangents[indices[i + 2]].push_back(biTangent);
	}

	for (size_t i = 0; i < tangents.size(); i++)
	{
		glm::vec3 average = glm::vec3(0);
		for (size_t y = 0; y < tangents[i].size(); y++)
		{
			average += tangents[i][y];
		}
		vertices[i].tangent = average / (float)tangents[i].size();

		average = glm::vec3(0);
		for (size_t y = 0; y < biTangents[i].size(); y++)
		{
			average += biTangents[i][y];
		}
		vertices[i].biTangent = average / (float)biTangents[i].size();
	}
}

Mesh::~Mesh()
{
	VkDevice device = VulkanManager::GetInstance()->GetLogicalDevice()->GetVk();

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	Logger::Log("Mesh destroyed");
}

void Mesh::CmdBind(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = {vertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::CmdDraw(VkCommandBuffer commandBuffer)
{
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}
