#include "Precompiled.h"
#include "GameWorld.h"

using namespace VEngine;

void VEngine::GameWorld::Initialize(uint32_t capacity)
{
	ASSERT(!mInitialized, "GameWorld: is already initialized");

	mGameObjectSlots.resize(capacity);
	mFreeSlots.resize(capacity);
	std::iota(mFreeSlots.begin(), mFreeSlots.end(), 0);

	mInitialized = true;

}

void VEngine::GameWorld::Terminate()
{
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			slot.gameObject->Terminate();
			slot.gameObject.reset();
		}
	}
	mGameObjectSlots.clear();
	mFreeSlots.clear();
	mToBeDestroyed.clear();
	mInitialized = false;
}

void VEngine::GameWorld::Update(float deltaTime)
{
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			slot.gameObject->Update(deltaTime);
		}
	}
}

void VEngine::GameWorld::Render()
{
}

void VEngine::GameWorld::DebugUI()
{
	for (Slot& slot : mGameObjectSlots)
	{
		if (slot.gameObject != nullptr)
		{
			slot.gameObject->DebugUI();
		}
	}
}

GameObject* GameWorld::CreateGameObject(std::string name)
{
	ASSERT(mInitialized, "GameWorld: is not initialized");
	if (mFreeSlots.empty())
	{
		ASSERT(false, "GameWorld: no free slots available");
		return nullptr;
	}

	const uint32_t freeSlot = mFreeSlots.back();
	mFreeSlots.pop_back();

	Slot& slot = mGameObjectSlots[freeSlot];
	slot.gameObject = std::make_unique<GameObject>();
	slot.gameObject->SetName(name);
	slot.gameObject->mHandle.mIndex = freeSlot;
	slot.gameObject->mHandle.mGeneration = slot.ganeration;
	return slot.gameObject.get();
}

void VEngine::GameWorld::DestroyGameObject(const GameObjectHandle& handle)
{
	if (!IsValid(handle))
	{
		return;
	}

	Slot& slot = mGameObjectSlots[handle.mIndex];
	++slot.ganeration;
	mToBeDestroyed.push_back(handle.mIndex);
}

bool VEngine::GameWorld::IsValid(const GameObjectHandle& handle)
{
	if (handle.mIndex < 0 || handle.mIndex >= mGameObjectSlots.size())
	{
		return false;
	}
	if (mGameObjectSlots[handle.mIndex].ganeration != handle.mGeneration)
	{
		return false;
	}
	return true;
}

void VEngine::GameWorld::ProcessDestroyList()
{
	for (uint32_t index : mToBeDestroyed)
	{
		Slot& slot = mGameObjectSlots[index];
		GameObject* gameObject = slot.gameObject.get();
		ASSERT(!IsValid(gameObject->GetHandle()), "GameWorld: gameObject is still alive");

		gameObject->Terminate();
		slot.gameObject.reset();
		mFreeSlots.push_back(index);
	}
	mToBeDestroyed.clear();
}
