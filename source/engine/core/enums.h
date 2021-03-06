/* ---------------------------------------------------------------------------
**
** enums.h
** Global engine enums
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#pragma once

namespace Engine
{
	enum ResourceType
	{
		RESOURCE_TYPE_NONE = 0,
		RESOURCE_TYPE_SPRITE = 1,

		RESOURCE_TYPE_MAX = RESOURCE_TYPE_SPRITE + 1
	};

	enum LayerId
	{
		LAYER_ID_NONE = 0,
		LAYER_ID_PLAYER = 1,
		LAYER_ID_ALIEN = 2,
		LAYER_ID_ROCKET = 3,
		LAYER_ID_BOMB = 4,

		LAYER_ID_MAX = LAYER_ID_BOMB + 1
	};

	enum ComponentType
	{
		COMPONENT_TYPE_NONE = 0,
		
		COMPONENT_TYPE_MODEL = 1,

		COMPONENT_TYPE_PLAYER = 2,
		COMPONENT_TYPE_ALIEN = 3,
		COMPONENT_TYPE_ROCKET = 4,
		COMPONENT_TYPE_BOMB = 5,

		COMPONENT_TYPE_MAX = COMPONENT_TYPE_BOMB + 1
	};
	
	enum ModelDataSegment
	{
		MODEL_DATA_COMPONENT = 0,
		MODEL_DATA_TRANSFORM = 1,
		MODEL_DATA_SPRITE = 2,
		MODEL_DATA_COLLIDER = 3,
		MODEL_DATA_COLLISION = 4,

		MODEL_DATA_MAX = MODEL_DATA_COLLISION + 1
	};

	enum CallbackStage
	{
		CALLBACK_STAGE_NONE = 0,
		CALLBACK_STAGE_INIT = 1,
		CALLBACK_STAGE_PHYSICS_BROAD = 2,
		CALLBACK_STAGE_PHYSICS_NARROW = 3,
		CALLBACK_STAGE_PRE_UPDATE = 4,
		CALLBACK_STAGE_UPDATE = 5,
		CALLBACK_STAGE_RENDER = 6,
		CALLBACK_STAGE_POST_UPDATE = 7,
		CALLBACK_STAGE_SHUTDOWN = 8,

		CALLBACK_STAGE_MAX = CALLBACK_STAGE_SHUTDOWN + 1
	};
}