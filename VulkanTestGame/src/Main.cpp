#include <iostream>
#include <sstream>

#include <GlfwManager.h>
#include <VulkanManager.h>
#include <FPSCounter.h>

#if DEBUG
int main()
#else
int WinMain()
#endif
{

	try
	{
		GlfwManager glfwManager = GlfwManager(800, 600, "TestGame");
		VulkanManager vulkanManager(glfwManager.GetWindow(), VkSampleCountFlagBits::VK_SAMPLE_COUNT_64_BIT);
		float timeToUpdateFPS = 0.25f;
		float fpsUpdateTimer = timeToUpdateFPS;

		while (!glfwWindowShouldClose(glfwManager.GetWindow()))
		{
			glfwPollEvents();
			FPSCounter::StartCounting();

			vulkanManager.Draw(&glfwManager);

			FPSCounter::StopCounting();

			fpsUpdateTimer += FPSCounter::GetDeltaTime();
			if (fpsUpdateTimer >= timeToUpdateFPS)
			{
				fpsUpdateTimer = 0;
				std::stringstream ss;
				ss << "Average FPS: " << FPSCounter::GetAverageFPS() << "FPS: " << FPSCounter::GetRawFPS() << "DeltaTime: " << (int)FPSCounter::GetDeltaTime();
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
	return 0;
}