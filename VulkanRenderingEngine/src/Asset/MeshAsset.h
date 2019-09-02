#pragma once
#include "Asset.h"

class MeshAsset : public Asset
{
public:
	virtual std::string GetType() override;
	virtual void GUI() override;

	virtual void Load(nlohmann::json data);

protected:
	virtual nlohmann::json Save();

};