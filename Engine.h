#pragma once
#include "DataTypes.h"

namespace Engine
{
	// Core
	Context*         Init(const int width, const int height);
	void             Shutdown(Context *const context);

	// Model
	ComponentHandle  CreateModel(World *const world, const EntityHandle    handle, const Sprite sprite);
	ComponentHandle  LookupModel(const World &world, const EntityHandle    handle);
	void             DestroyModel(World *const world, const ComponentHandle handle);

	void             DrawModels(const World& world);
	void             DetectCollisions(const World& world, std::vector<CollisionInfo>* collisions);

	// Resources
	ResourceHandle   LoadSprite(IDiceInvaders *const system, Resources *const resources, const std::string& name);
	Sprite           LookupSprite(const Resources& resources, const ResourceHandle handle);
	void             DestroySprite(const IDiceInvaders *const system, Resources *const resources, ResourceHandle handle);
}
