/* ---------------------------------------------------------------------------
**
** engine.h
** Minimal game engine written for the Dice Invaders game
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once
#include "engine_types.h"

namespace Engine
{
	// Core
	// Global engine context
	extern Context *g_context;

	// Core engine init / de-init routines
	void Init(const Config& config);
	void Shutdown();
	void Update();
	
	// World
	// Initialize a layer. It should be called before registering component managers
	void InitLayer(const LayerId layer_id, const uint16_t max_entities);
	inline World::LayerData* GetLayerData(const LayerId layer_id) { return &g_context->world->layers[layer_id]; }
	
	// Defines two layers as colliding
	inline void AddCollisionMask(LayerId first, LayerId second) { g_context->world->collision_masks.push_back({ first, second }); }

	// Utility

	// There is something wiered regarding the drawText function in the
	// provided library. It causes a huge slowdown, which is clear looking at
	// the stats in the top-left corner
	inline void DrawText(int x, int y, const char *const text) { g_context->system->drawText(x, y, text); }

	// Retruns a decimal random number, distributed uniformly between 0 and 1
	float Random();
	
	// The program should exit when this function returns false
	bool ShouldRun();

	inline const Config& GetConfig() { return *g_context->config; }
	inline int ScreenWidth() { return g_context->config->screen_width; }
	inline int ScreenHeight() { return g_context->config->screen_height; }

	inline const FrameData& GetFrameData() { return *g_context->frame_data; }
	inline const KeyStatus& Keys() { return g_context->frame_data->keys; }

	// Time elapsed since the startup
	inline float Time() { return g_context->frame_data->time; }
	
	// Time elapsed since the last frame
	inline float DeltaTime() { return g_context->frame_data->dt; }
	

	// Component
	// Calls init function for the registered component managers.
	// It should be called after all component managers are registered.
	void InitComponents();

	// Do the actual deletion of the components which was marked 
	// to be freed in the current frame. It is called internally 
	// afte the post update.
	void CleanupComponents();

	// Register a new component manager, and its callbacks
	void RegisterComponentType(const ComponentType type, const std::unordered_map<CallbackStage, ComponentManager::Callback>& callbacks);

	inline ComponentManager* GetComponentManager(const ComponentType type) { return &g_context->world->components[type]; }
	void ExecuteComponentCallbacks(CallbackStage stage);

	// Creates a new component of the provided type, and 
	// link it to the specified entity. It also store a handle
	// to the "Model" component if the entity has one.
	ComponentHandle CreateComponent(const EntityHandle& entity, const ComponentType type);

	// Mark a component to be freed at the end of current frame
	void DestroyComponent(const ComponentHandle& component);

	// Helper function to cunstruct a component handle from its parts
	inline ComponentHandle     ConstructComponentHandle(uint16_t index, ComponentType type, LayerId layer)
	{
		ComponentHandle handle;
		handle.index = index;
		handle.header.type = static_cast<uint8_t>(type);
		handle.header.layer = static_cast<uint8_t>(layer);
		return handle;
	}

	// Returns a handle to the component of the provided type, which 
	// is attached to the specified entity
	inline ComponentHandle GetComponentOfType(const EntityHandle& handle, const ComponentType type) { return GetComponentManager(type)->component_map[handle]; }

	// Helpers for accessing component data
	inline RosterPool* ResolveComponentPool(const ComponentType type, const LayerId layer) { return &GetComponentManager(type)->layer_data[layer].components; }
		template<typename T>
	T* ResolveComponentSegment(const ComponentHandle& component, const uint8_t segment) { return ResolveComponentPool(static_cast<ComponentType>(component.header.type), static_cast<LayerId>(component.header.layer))->Resolve<T>(component.index, segment); }
	template<typename T>
	T* ResolveComponentSegmentData(const ComponentType type, const LayerId layer, const uint8_t segment) { return ResolveComponentPool(type, layer)->Data<T>(segment); }

	// Entity

	// Do the actual deletion of the entities which was marked 
	// to be freed in the current frame. It is called internally 
	// afte the post update.
	void CleanupEntities();

	inline EntityHandle CreateEntity(const LayerId layer) { return g_context->world->handle_manager.Create(layer); }

	// Mark an entity to be freed at the end of current frame
	inline void         DestroyEntity(const EntityHandle& handle) { g_context->world->to_be_freed_entities.insert(handle); }

	// Checks whether an entity handle is valid
	inline bool         ValidEntity(const EntityHandle& handle) { return g_context->world->handle_manager.Valid(handle); }
	

	// Model

	// Callback functions of the "Model" component type
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
