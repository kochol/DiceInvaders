/* ---------------------------------------------------------------------------
**
** model_manager.cpp
** "Model" component initialization routine
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "engine.h"

namespace Engine
{
	void InitModels(ComponentManager::LayerData *const data)
	{
		const World::LayerData *const world_layer_data = GetLayerData(data->layer_id);
		data->components.Init(world_layer_data->max_entities,
		{ sizeof(BaseComponent), sizeof(Transform), sizeof(ResourceHandle), sizeof(Collider), sizeof(Collision) });
	}
}
