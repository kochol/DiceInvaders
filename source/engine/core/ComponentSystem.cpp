#include "../Engine.h"
#include <cassert>

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

			for (auto &layer : manager->layerData)
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

			for (auto &layer : manager->layerData)
			{
				if (layer.toBeFreed.empty())
					continue;

				for (auto component_index : layer.toBeFreed)
				{
					const BaseComponent *const component_data = layer.components.Resolve<BaseComponent>(component_index, 0);
					manager->componentMap.erase(component_data->entity);
					layer.components.Free(component_index);
				}
				layer.toBeFreed.clear();
			}
		}
	}

	void RegisterComponentType(const ComponentType type, const std::unordered_map<CallbackStage, ComponentManager::Callback>& callbacks)
	{
		ComponentManager *const manager = GetComponentManager(type);

		manager->type = type;

		for (unsigned i = 0; i < COMPONENT_TYPE_MAX; i++)
			manager->layerData[i].layerId = static_cast<LayerId>(i);

		for (auto& callback : manager->callbacks)
			callback = nullptr;

		for (const auto callback : callbacks)
			manager->callbacks[callback.first] = callback.second;
	}

	ComponentHandle CreateComponent(const EntityHandle& entity, const ComponentType type)
	{
		ComponentManager *const manager = GetComponentManager(type);
		ComponentManager::LayerData *const layer = &manager->layerData[entity.header.layer];

		ComponentHandle component;
		component.index = layer->components.Alloc();
		component.header.layer = entity.header.layer;
		component.header.type = static_cast<uint8_t>(type);

		manager->componentMap[entity] = component;

		BaseComponent *const componentData = layer->components.Resolve<BaseComponent>(component.index);
		componentData->self = component;
		componentData->entity = entity;

		const ComponentManager *const model_manager = GetComponentManager(COMPONENT_TYPE_MODEL);
		const auto model = model_manager->componentMap.find(entity);
		if (model != model_manager->componentMap.end())
			componentData->model = model->second;

		return component;
	}

	void DestroyComponent(const ComponentHandle& component)
	{
		ComponentManager *const manager = GetComponentManager(static_cast<ComponentType>(component.header.type));
		ComponentManager::LayerData *const layer = &manager->layerData[component.header.layer];

		layer->toBeFreed.insert(component.index);
	}

}
