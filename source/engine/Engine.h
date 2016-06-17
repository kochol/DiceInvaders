#pragma once
#include "EngineTypes.h"

namespace Engine
{
	// Core
	extern Context *g_context;
	void             Init(const Config& config);
	void             Shutdown();
	bool      ShouldRun();

	void InitComponents();

	void PreUpdate();
	void RegUpdate();
	void PostUpdate();

	void DetectCollisions();
	void Render();

	void AddLayer(const LayerId layer_id, const uint16_t max_entities);

	float GetRandom();

	// Component
	void RegisterComponentType(ComponentType type, const std::unordered_map<CallbackStage, ComponentManager::Callback>& callbacks);
	inline ComponentManager* GetComponentManager(const ComponentType type) { return g_context->world->components[type]; }
	ComponentHandle CreateComponent(const EntityHandle entity, const ComponentType type);
	void DestroyComponent(const ComponentHandle component);

	// Entity
	EntityHandle        CreateEntity(const LayerId layer);
	void                DestroyEntity(const EntityHandle handle);
	bool                ValidEntity(const EntityHandle handle);
	inline  Layer* ResolveLayer(const LayerId layer) { return g_context->world->layers[layer]; }
	inline  Layer* ResolveLayer(const ComponentHandle handle) { return ResolveLayer(handle.header.layer); }
	inline  Layer* ResolveLayer(const EntityHandle handle) { return ResolveLayer(handle.header.layer); }

	// Model
	ComponentHandle     CreateModel(const EntityHandle    handle, const ResourceHandle sprite);
	inline ComponentHandle     LookupModel(const EntityHandle    handle) { return g_context->world->modelMap[handle]; }
	inline ComponentHandle&     GetComponentHandle(uint16_t index, ComponentType type, LayerId layer)
	{
		ComponentHandle handle;
		handle.index = index;
		handle.header.type = type;
		handle.header.layer = layer;

		return handle;
	}
	void                DestroyModel(const ComponentHandle handle);
	inline  Transform*  ResolveTransform(const ComponentHandle handle) { return ResolveLayer(handle)->models.Resolve<Transform>(handle.index, 0); }
	inline  ResourceHandle*  ResolveSprite(const ComponentHandle handle) { return ResolveLayer(handle)->models.Resolve<ResourceHandle>(handle.index, 1); }
	inline  Collider*  ResolveCollider(const ComponentHandle handle) { return ResolveLayer(handle)->models.Resolve<Collider>(handle.index, 2); }

	// Resources
	ResourceHandle   LoadSprite(const std::string& name);
	inline ISprite*  LookupSprite(const ResourceHandle handle) { return *g_context->resources->spriteCache.Resolve<ISprite*>(handle.index); }
	void             DestroySprite(ResourceHandle handle);
}
