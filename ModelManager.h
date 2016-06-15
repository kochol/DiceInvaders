#pragma once
#include "DataTypes.h"

namespace DiceInvaders
{
	ComponentHandle  CreateModel      (World *const world, const EntityHandle    handle, const Sprite sprite);
	ComponentHandle  LookupModel      (const World &world, const EntityHandle    handle);
	void             DestroyModel     (World *const world, const ComponentHandle handle);

	void             DrawModels       (const std::vector<Transform>& transforms, std::vector<Sprite>& sprites);
	void             DetectCollisions (const std::vector<Transform>& transforms, std::vector<CollisionInfo>* collisions);

	ResourceHandle   LoadSprite       (IDiceInvaders *const system, Resources *const resources, const std::string& name);
	Sprite           LookupSprite     (const Resources& resources, const ResourceHandle handle);
	void             DestroySprite    (const IDiceInvaders *const system, Resources *const resources, ResourceHandle handle);
}
