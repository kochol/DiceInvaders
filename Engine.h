#pragma once
#include "EngineTypes.h"

namespace Engine
{
	// Core
	extern Context *g_context;
	void             Init(const Config& config);
	void             Shutdown();
	bool      ShouldRun();


	void PreUpdate();
	void RegUpdate();
	void PostUpdate();

	void DetectCollisions();
	void Render();

	void AddLayer(const LayerId layer_id, const uint16_t max_entities);

	// Component
	void RegisterComponentType(ComponentType type, const ComponentManager::Callbacks callbacks);

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
	void                DestroyModel(const ComponentHandle handle);
	inline  Transform*  ResolveTransform(const ComponentHandle handle) { return ResolveLayer(handle)->models.ResolveFirst(handle.index); }

	// Resources
	ResourceHandle   LoadSprite(const std::string& name);
	inline ISprite*  LookupSprite(const ResourceHandle handle) { return g_context->resources->spriteMap[handle]; }
	void             DestroySprite(ResourceHandle handle);
}
