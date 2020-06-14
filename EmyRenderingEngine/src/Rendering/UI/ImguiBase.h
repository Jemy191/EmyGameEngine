#pragma once
#include "Header/ImguiHeader.h"

class ImguiBase
{
public:
	virtual void StartFrame() = 0;
	virtual void EndFrame();
};

