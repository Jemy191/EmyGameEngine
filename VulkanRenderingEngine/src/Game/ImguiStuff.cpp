#include "Game/ImguiStuff.h"

#include "Rendering/Vulkan/VulkanHelper.h"
#include "Helper/Log.h"
#include "Rendering/Vulkan/VulkanManager.h"

ImguiStuff::ImguiStuff(GLFWwindow* window, uint32_t queueFamily)
{
	Logger::Log("Creating imgui");
	VkInstance instance = VulkanManager::GetInstance()->GetVulkanInstance()->GetInstance();
	VkDevice device = VulkanManager::GetInstance()->GetLogicalDevice()->GetVKDevice();
	VkPhysicalDevice physicalDevice = VulkanManager::GetInstance()->GetPhysicalDevice()->GetVKPhysicalDevice();
	VkRenderPass renderPass = VulkanManager::GetInstance()->GetRenderPass()->GetVkRenderPass();
	VkQueue graphicQueue = VulkanManager::GetInstance()->GetLogicalDevice()->GetGraphicsQueue();

	// Create Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		if (vkCreateDescriptorPool(device, &pool_info, nullptr, &g_DescriptorPool) != VK_SUCCESS)
		{
			Logger::Log(LogSeverity::FATAL_ERROR, "Failed to create imgui descriptorPool");
		}
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = "Settings/imgui.ini";
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForVulkan(window, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance;
	init_info.PhysicalDevice = physicalDevice;
	init_info.Device = device;
	init_info.QueueFamily = queueFamily;
	init_info.Queue = graphicQueue;
	init_info.PipelineCache = g_PipelineCache;
	init_info.DescriptorPool = g_DescriptorPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = VulkanHelper::QuerySwapChainSupport().capabilities.minImageCount + 1;;
	init_info.ImageCount = VulkanManager::GetInstance()->GetSwapChain()->GetVkImages().size();
	init_info.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&init_info, renderPass);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	io.Fonts->AddFontDefault();

	// Upload Fonts
	{
		VkCommandBuffer commandBuffer = VulkanHelper::BeginSingleTimeCommands();

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		VulkanHelper::EndSingleTimeCommands(commandBuffer);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

ImguiStuff::~ImguiStuff()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	vkDestroyDescriptorPool(VulkanManager::GetInstance()->GetLogicalDevice()->GetVKDevice(), g_DescriptorPool, nullptr);
}

void ImguiStuff::StartFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImguiStuff::EndFrame()
{
	ImGui::Render(); // Also endFrame. Do not draw. Just setup draw data.
}

void ImguiStuff::Draw(VkCommandBuffer commandBuffer)
{
	// Record Imgui Draw Data and draw funcs into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}
