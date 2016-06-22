#include "Engine.h"
#include <cassert>

namespace Engine
{
	void InitModels(ComponentManager::LayerData *const data)
	{
		const World::LayerData *const world_layer_data = GetLayerData(data->layerId);
		data->components.Init(world_layer_data->maxEntities,
		{ sizeof(BaseComponent), sizeof(Transform), sizeof(ResourceHandle), sizeof(Collider), sizeof(Collision) });
	}

	/*ComponentHandle CreateModel(const EntityHandle& handle, const ResourceHandle& sprite)
	{
		World *const world = g_context->world;

		assert(world->layers[handle.header.layer] != nullptr);

		Layer *const layer = ResolveLayer(handle);

		ComponentHandle compHandle;
		compHandle.index = layer->models.Alloc();
		compHandle.header.type = COMPONENT_TYPE_MODEL;
		compHandle.header.layer = handle.header.layer;

		*ResolveModelComponent(compHandle) = { handle, compHandle };
		ResolveModelTransform(compHandle)->position = { 0.f, 0.f };
		*ResolveModelSprite(compHandle) = sprite;
		Collider *collider = ResolveModelCollider(compHandle);
		collider->localBb.center = { 16.f, 16.f };
		collider->localBb.halfSize = { 16.f, 16.f };
		*ResolveModelCollision(compHandle) = { LAYER_ID_NONE };
		
		world->modelMap[handle] = compHandle;

		return compHandle;
	}

	void DestroyModel(const ComponentHandle& handle)
	{
		assert(handle.header.type == COMPONENT_TYPE_MODEL);

		World *const world = g_context->world;

		// TODO: make it deferred

		const BaseComponent *const component = ResolveModelComponent(handle);
		world->modelMap.erase(component->entity);

		Layer *const layer = ResolveLayer(handle);

		layer->models.Free(handle.index);
	}*/

	
}
