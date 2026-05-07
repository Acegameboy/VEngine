#pragma once

namespace VEngine
{
    class GameObject;
    class GameWorld;
    class Component;

    namespace GameObjectFactory
    {
        void Make(const std::filesystem::path& templatePath, GameObject& gameObject, GameWorld& gameWorld);
    }
}
