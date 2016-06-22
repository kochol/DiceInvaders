#pragma once
#include "engine_types.h"

namespace Engine
{
	// Core
	extern Context *g_context;

	void Init(const Config& config);
	void Shutdown();
	void Update();
	void ExecuteComponentCallbacks(CallbackStage stage);

	// World
	void InitLayer(const LayerId layer_id, const uint16_t max_entities);
	inline World::LayerData* GetLayerData(const LayerId layer_id) { return &g_context->world->layers[layer_id]; }
	inline void AddCollisionMask(LayerId first, LayerId second) { g_context->world->collision_masks.push_back({ first, second }); }

	// Utility
	inline void DrawText(int x, int y, const char *const text) { g_context->system->drawText(x, y, text); }
	float Random();
	bool ShouldRun();
	inline const Config& GetConfig() { return *g_context->config; }
	inline int ScreenWidth() { return g_context->config->screen_width; }
	inline int ScreenHeight() { return g_context->config->screen_height; }

	inline const FrameData& GetFrameData() { return *g_context->frame_data; }
	inline float Time() { return g_context->frame_data->time; }
	inline float DeltaTime() { return g_context->frame_data->dt; }
	inline const KeyStatus& Keys() { return g_context->frame_data->keys; }

	// Component
	void InitComponents();
	void CleanupComponents();
	void RegisterComponentType(const ComponentType type, const std::unordered_map<CallbackStage, ComponentManager::Callback>& callbacks);
	inline ComponentManager* GetComponentManager(const ComponentType type) { return &g_context->world->components[type]; }

	ComponentHandle CreateComponent(const EntityHandle& entity, const ComponentType type);
	void DestroyComponent(const ComponentHandle& component);
	inline ComponentHandle     ConstructComponentHandle(uint16_t index, ComponentType type, LayerId layer)
	{
		ComponentHandle handle;
		handle.index = index;
		handle.header.type = static_cast<uint8_t>(type);
		handle.header.layer = static_cast<uint8_t>(layer);
		return handle;
	}
	inline ComponentHandle LookupComponent(const EntityHandle& handle, const ComponentType type) { return GetComponentManager(type)->component_map[handle]; }
	inline RosterPool* ResolveComponentPool(const ComponentType type, const LayerId layer) { return &GetComponentManager(type)->layer_data[layer].components; }

	template<typename T>
	T* ResolveComponentSegment(const ComponentHandle& component, const uint8_t segment) { return ResolveComponentPool(static_cast<ComponentType>(component.header.type), static_cast<LayerId>(component.header.layer))->Resolve<T>(component.index, segment); }
	template<typename T>
	T* ResolveComponentSegmentData(const ComponentType type, const LayerId layer, const uint8_t segment) { return ResolveComponentPool(type, layer)->Data<T>(segment); }

	// Entity
	void CleanupEntities();
	inline EntityHandle CreateEntity(const LayerId layer) { return g_context->world->handle_manager.Create(layer); }
	inline void         DestroyEntity(const EntityHandle& handle) { g_context->world->to_be_freed_entities.insert(handle); }
	inline bool         ValidEntity(const EntityHandle& handle) { return g_context->world->handle_manager.Valid(handle); }
	

	// Model
	void InitModels(ComponentManager::LayerData *const data);
	void UpdatePhysicsBroadPhase(ComponentManager::LayerData *const data);
	void UpdatePhysicsNarrowPhase(ComponentManager::LayerData *const data);
	void Render(ComponentManager::LayerData *const data);

	// Resources
	void CleanupResources();
	ResourceHandle   LoadSprite(const std::string& name);
	inline ISprite** ResolveSprite(const ResourceHandle& handle) { return g_context->resources->caches[RESOURCE_TYPE_SPRITE].Resolve<ISprite*>(handle.index); }
	inline const uint16_t* ResolveSpriteIndexes() { return g_context->resources->caches[RESOURCE_TYPE_SPRITE].Indexes(); }
	inline ISprite** ResolveSpriteData() { return g_context->resources->caches[RESOURCE_TYPE_SPRITE].Data<ISprite*>(); }
	inline void             DestroySprite(const ResourceHandle& handle) { g_context->resources->to_be_freed.insert(handle); }
}
