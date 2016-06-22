/* ---------------------------------------------------------------------------
**
** component_system.cpp
** Component system, based on Insomniac's Dynamic Component System (DCS)
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "../engine.h"

/* -- module -----------------------------------------------------------------
**
** The component system is the underlying system for most of the engine 
** functionalities. Everything from rendering, physics, to the gameplay logic
** is implemented as a component system.
** The system is based on the pure composition pattern. Components relate to
** each other by their entity handle, and the entity is just a handle and has
** no underlying data. 
** Components are managed by their corresponding component manager. Objects in
** the engine are oraganized in "Layer"s, for betetr cash locality, eg. all the
** components of the aliens are in the same layer. Component managers register 
** callbacks for some pre-defined events. The Component system then execute them
** for each layer, for each component.
** Freeing components is a deferred operation, the actual deletion takes place
** after the post update stage.
** Components should be stored using their handle, not their raw pointer. Each
** component manager is free to lay its data an as many arrays as suitable. The
** only requirement is to store the BaseComponent in the first array.
** Component managers should initialize their memory pools and internal states,
** in the registered initialize callback. If a component is intended to work
** only in a specific layer, it should skeep all the init calls, except for that
** layer.
** Accessing component data should be done using the ResolveComponentSegment...
** helpers.
** The order in which callbacks are executed is indicated by the component type
** enumerator value, executing from 0 to n-1.
**
/* -- implementation ---------------------------------------------------------
**
** Internally the component system makes heavy use of the roster pool to handle
** component data.
**
** -------------------------------------------------------------------------*/

namespace Engine
{
	void ExecuteComponentCallbacks(CallbackStage stage)
	{
		for (unsigned i = COMPONENT_TYPE_NONE + 1; i < COMPONENT_TYPE_MAX; i++)
		{
			ComponentManager *const manager = &g_context->world->components[i];

			const ComponentManager::Callback callback = manager->callbacks[stage];
			if (callback == nullptr)
				continue;

			for (auto &layer : manager->layer_data)
			{
				callback(&layer);
			}
		}
	}

	void InitComponents()
	{
		ExecuteComponentCallbacks(CALLBACK_STAGE_INIT);
	}

	void CleanupComponents()
	{
		for (unsigned i = COMPONENT_TYPE_NONE + 1; i < COMPONENT_TYPE_MAX; i++)
		{
			ComponentManager *const manager = &g_context->world->components[i];

			for (auto &layer : manager->layer_data)
			{
				if (layer.to_be_freed.empty())
					continue;

				for (auto component_index : layer.to_be_freed)
				{
					const BaseComponent *const component_data = layer.components.Resolve<BaseComponent>(component_index, 0);
					manager->component_map.erase(component_data->entity);
					layer.components.Free(component_index);
				}
				layer.to_be_freed.clear();
			}
		}
	}

	void RegisterComponentType(const ComponentType type, const std::unordered_map<CallbackStage, ComponentManager::Callback>& callbacks)
	{
		ComponentManager *const manager = GetComponentManager(type);

		manager->type = type;

		for (unsigned i = 0; i < COMPONENT_TYPE_MAX; i++)
			manager->layer_data[i].layer_id = static_cast<LayerId>(i);

		for (auto& callback : manager->callbacks)
			callback = nullptr;

		for (const auto callback : callbacks)
			manager->callbacks[callback.first] = callback.second;
	}

	ComponentHandle CreateComponent(const EntityHandle& entity, const ComponentType type)
	{
		ComponentManager *const manager = GetComponentManager(type);
		ComponentManager::LayerData *const layer = &manager->layer_data[entity.header.layer];

		ComponentHandle component;
		component.index = layer->components.Alloc();
		component.header.layer = entity.header.layer;
		component.header.type = static_cast<uint8_t>(type);

		manager->component_map[entity] = component;

		BaseComponent *const componentData = layer->components.Resolve<BaseComponent>(component.index);
		componentData->self = component;
		componentData->entity = entity;

		const ComponentManager *const model_manager = GetComponentManager(COMPONENT_TYPE_MODEL);
		const auto model = model_manager->component_map.find(entity);
		if (model != model_manager->component_map.end())
			componentData->model = model->second;

		return component;
	}

	void DestroyComponent(const ComponentHandle& component)
	{
		ComponentManager *const manager = GetComponentManager(static_cast<ComponentType>(component.header.type));
		ComponentManager::LayerData *const layer = &manager->layer_data[component.header.layer];

		layer->to_be_freed.insert(component.index);
	}

}
