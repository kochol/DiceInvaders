#include "Engine.h"

namespace Engine
{
	static void _RenderLayer(
		const uint16_t *const indexes,
		const Transform *const transforms,
		const ResourceHandle *const sprites,
		const uint16_t count,
		const uint16_t *const iSpritesIndexes,
		ISprite **const iSprites)
	{
		for (uint16_t i = 0; i < count; ++i)
		{
			const uint16_t index = indexes[i];
			const uint16_t iSpriteIndex = iSpritesIndexes[sprites[index].index];
			ISprite *const sprite = iSprites[iSpriteIndex];
			sprite->draw(
				static_cast<int>(transforms[index].position.x),
				static_cast<int>(transforms[index].position.y));
		}
	}

	void Render(ComponentManager::LayerData *const data)
	{
		ISprite **const iSprites = ResolveSpriteData();
		const uint16_t *const iSpritesIndexes = ResolveSpriteIndexes();

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
			iSpritesIndexes,
			iSprites);
	}
}