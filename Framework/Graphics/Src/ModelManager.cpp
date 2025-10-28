#include "Precompiled.h"
#include "ModelManager.h"
#include "ModelIO.h"

using namespace VEngine;
using namespace VEngine::Graphics;

namespace
{
	std::unique_ptr<ModelManager> sModelManager;
}

void ModelManager::StaticInitialize(const std::filesystem::path& rootPath)
{
	ASSERT(sModelManager == nullptr, "ModelManager: is already initialized");
	sModelManager = std::make_unique<ModelManager>();
	sModelManager->SetRootDirectory(rootPath);
}

void ModelManager::StaticTerminate()
{
	sModelManager.reset();
}

ModelManager* VEngine::Graphics::ModelManager::Get()
{
	ASSERT(sModelManager != nullptr, "ModelManager: Is not initialized");
	return sModelManager.get();
}

void ModelManager::SetRootDirectory(const std::filesystem::path& rootPath)
{
	mRootDirectory = rootPath;
}

ModelId VEngine::Graphics::ModelManager::GetModelId(const std::filesystem::path& filePath)
{
	return std::filesystem::hash_value(mRootDirectory / filePath);
}

ModelId VEngine::Graphics::ModelManager::LoadModel(const std::filesystem::path& filePath)
{
	const ModelId modelId = GetModelId(filePath);
	auto [iter, success] = mInventory.insert({ modelId, nullptr });
	if (success)
	{
		std::filesystem::path fullPath = mRootDirectory / filePath;
		auto& modelPtr = iter->second;
		modelPtr = std::make_unique<Model>();
		ModelIO::LoadModel(fullPath, *modelPtr);
	}
	return modelId;
}

const Model* VEngine::Graphics::ModelManager::GetModel(ModelId id)
{
	auto model = mInventory.find(id);
	if (model != mInventory.end())
	{
		return model->second.get();
	}
	return nullptr;
}
