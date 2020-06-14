#include "ImguiBase.h"

void ImguiBase::EndFrame()
{
	ImGui::Render(); // Also endFrame. Do not draw. Just setup draw data.
}