#pragma once

namespace VEngine
{
	class GameObjectHandle
	{
	public:
		GameObjectHandle() = default;
	private:
		friend class GameWorld;
		int mIndex = -1; //is going to be the index of the slots in game world
		int mGeneration = -1; //to verify if item is already tagged of deletion/removed
	};
}
