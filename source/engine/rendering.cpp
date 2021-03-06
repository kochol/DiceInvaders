/* ---------------------------------------------------------------------------
**
** rendering.cpp
** Rendering stuff, implemented as a part of "Model" component
**
** Author: Ali Salehi
** -------------------------------------------------------------------------*/

#include "engine.h"

namespace Engine
{
	static void _RenderLayer(
		const uint16_t *const indexes,
		const Transform *const transforms,
		const ResourceHandle *const sprites,
		const uint16_t count,
		const uint16_t *const i_sprites_indexes,
		ISprite **const i_sprites)
	{
		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t iSpriteIndex = i_sprites_indexes[sprites[index].index];
			ISprite *const sprite = i_sprites[iSpriteIndex];
			sprite->draw(
				static_cast<int>(transforms[index].position.x),
				static_cast<int>(transforms[index].position.y));
		}
	}

	void Render(ComponentManager::LayerData *const data)
	{
		ISprite **const i_sprites = ResolveSpriteData();
		const uint16_t *const i_sprites_indexes = ResolveSpriteIndexes();

		const uint16_t count = data->components.Size();
		if (count == 0)
			return;

		const uint16_t *const indexes = data->components.Indexes();
		const Transform *const transforms = data->components.Data<Transform>(MODEL_DATA_TRANSFORM);
		const ResourceHandle *const sprites = data->components.Data<ResourceHandle>(MODEL_DATA_SPRITE);

		_RenderLayer(
			indexes,
			transforms,
			sprites,
			count,
			i_sprites_indexes,
			i_sprites);
	}
}