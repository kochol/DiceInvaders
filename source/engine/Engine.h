#pragma once
#include "EngineTypes.h"

namespace Engine
{
	// Core
	extern Context *g_context;

	void Init(const Config& config);
	void Shutdown();
	bool ShouldRun();

	void InitComponents();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void ShutdownComponents();

	void UpdateBoundingBoxes();
	void DetectCollisions();
	void Render();

	// World
	void InitLayer(const LayerId layer_id, const uint16_t max_entities);
	inline Layer* ResolveLayer(const LayerId layer) { return g_context->world->layers[layer]; }
	inline Layer* ResolveLayer(const ComponentHandle& handle) { return ResolveLayer(static_cast<LayerId>(handle.header.layer)); }
	inline Layer* ResolveLayer(const EntityHandle& handle) { return ResolveLayer(static_cast<LayerId>(handle.header.layer)); }

	// Utility
	float Random();
	inline const Config& GetConfig() { return *g_context->config; }
	inline int ScreenWidth() { return g_context->config->screen_width; }
	inline int ScreenHeight() { return g_context->config->screen_height; }

	inline const FrameData& GetFrameData() { return *g_context->frame_data; }
	inline float Time() { return g_context->frame_data->time; }
	inline float DeltaTime() { return g_context->frame_data->dt; }
	inline const KeyStatus& Keys() { return g_context->frame_data->keys; }

	// Component
	void RegisterComponentType(const ComponentType type, const std::unordered_map<CallbackStage, ComponentManager::Callback>& callbacks);
	inline ComponentManager* GetComponentManager(const ComponentType type) { return g_context->world->components[type]; }
	ComponentHandle CreateComponent(const EntityHandle& entity, const ComponentType type, const ComponentHandle& model);
	void DestroyComponent(const ComponentHandle& component);
	inline ComponentHandle     GetComponentHandle(uint16_t index, ComponentType type, LayerId layer)
	{
		ComponentHandle handle;
		handle.index = index;
		handle.header.type = type;
		handle.header.layer = layer;

		return handle;
	}

	// Entity
	inline EntityHandle CreateEntity(const LayerId layer) { return g_context->world->handleManager.Create(layer); }
	inline void         DestroyEntity(const EntityHandle& handle) { g_context->world->handleManager.Destroy(handle); }
	inline bool         ValidEntity(const EntityHandle& handle) { return g_context->world->handleManager.Valid(handle); }

	// Model
	ComponentHandle        CreateModel(const EntityHandle& handle, const ResourceHandle& sprite);
	inline ComponentHandle LookupModel(const EntityHandle& handle) { return g_context->world->modelMap[handle]; }
	void                   DestroyModel(const ComponentHandle& handle);
	
	inline  uint16_t          ResolveModelCount(const Layer& layer) { return layer.models.Size(); }
	inline  const uint16_t*   ResolveModelIndexes(const Layer& layer) { return layer.models.Indexes(); }
	inline  BaseComponent*    ResolveModelComponentData(const Layer& layer) { return layer.models.Data<BaseComponent>(0); }
	inline  Transform*        ResolveModelTransformData(const Layer& layer) { return layer.models.Data<Transform>(1); }
	inline  ResourceHandle*   ResolveModelSpriteData(const Layer& layer) { return layer.models.Data<ResourceHandle>(2); }
	inline  Collider*         ResolveModelColliderData(const Layer& layer) { return layer.models.Data<Collider>(3); }
	inline  Collision*    ResolveModelCollisionData(const Layer& layer) { return layer.models.Data<Collision>(4); }

	inline  uint16_t          ResolveModelCount(const LayerId layer) { return ResolveModelCount(*ResolveLayer(layer)); }
	inline  const uint16_t*   ResolveModelIndexes(const LayerId layer) { return ResolveModelIndexes(*ResolveLayer(layer)); }
	inline  BaseComponent*    ResolveModelComponentData(const LayerId layer) { return ResolveModelComponentData(*ResolveLayer(layer)); }
	inline  Transform*        ResolveModelTransformData(const LayerId layer) { return ResolveModelTransformData(*ResolveLayer(layer)); }
	inline  ResourceHandle*   ResolveModelSpriteData(const LayerId layer) { return ResolveModelSpriteData(*ResolveLayer(layer)); }
	inline  Collider*         ResolveModelColliderData(const LayerId layer) { return ResolveModelColliderData(*ResolveLayer(layer)); }
	inline  Collision*    ResolveModelCollisionData(const LayerId layer) { return ResolveModelCollisionData(*ResolveLayer(layer)); }

	inline  BaseComponent*  ResolveModelComponent(const ComponentHandle& handle) { return ResolveLayer(handle)->models.Resolve<BaseComponent>(handle.index, 0); }
	inline  Transform*  ResolveModelTransform(const ComponentHandle& handle) { return ResolveLayer(handle)->models.Resolve<Transform>(handle.index, 1); }
	inline  ResourceHandle*  ResolveModelSprite(const ComponentHandle& handle) { return ResolveLayer(handle)->models.Resolve<ResourceHandle>(handle.index, 2); }
	inline  Collider*  ResolveModelCollider(const ComponentHandle& handle) { return ResolveLayer(handle)->models.Resolve<Collider>(handle.index, 3); }
	inline  Collision*  ResolveModelCollision(const ComponentHandle& handle) { return ResolveLayer(handle)->models.Resolve<Collision>(handle.index, 4); }

	// Resources
	ResourceHandle   LoadSprite(const std::string& name);
	inline ISprite** ResolveSprite(const ResourceHandle& handle) { return g_context->resources->caches[RESOURCE_TYPE_SPRITE].Resolve<ISprite*>(handle.index); }
	inline const uint16_t* ResolveSpriteIndexes() { return g_context->resources->caches[RESOURCE_TYPE_SPRITE].Indexes(); }
	inline ISprite** ResolveSpriteData() { return g_context->resources->caches[RESOURCE_TYPE_SPRITE].Data<ISprite*>(); }
	void             DestroySprite(const ResourceHandle& handle);
}
