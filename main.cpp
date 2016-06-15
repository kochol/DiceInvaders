#include <windows.h>
#include <cassert>
#include <cstdio>
#include "DiceInvaders.h"
#include <cstdint>
#include <unordered_map>
#include <array>
#include "EntityManager.h"
#include "DataTypes.h"
#include "ModelManager.h"

class DiceInvadersLib
{
public:
	explicit DiceInvadersLib(const char* libraryPath)
	{
		m_lib = LoadLibrary(libraryPath);
		assert(m_lib);

		DiceInvadersFactoryType* factory = (DiceInvadersFactoryType*)GetProcAddress(
				m_lib, "DiceInvadersFactory");
		m_interface = factory();
		assert(m_interface);
	}

	~DiceInvadersLib()
	{
		FreeLibrary(m_lib);
	}

	IDiceInvaders* get() const 
	{
		return m_interface;
	}

private: 
	DiceInvadersLib(const DiceInvadersLib&);
	DiceInvadersLib& operator=(const DiceInvadersLib&);

private: 
	IDiceInvaders* m_interface;
	HMODULE m_lib;
};


int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	DiceInvaders::Context* ctx = new DiceInvaders::Context;
	
	DiceInvadersLib lib("DiceInvaders.dll");
	ctx->system = lib.get();
	ctx->system->init(640, 480);

	ctx->resources = new DiceInvaders::Resources;
	ctx->world = new DiceInvaders::World;


	std::string playerSpriteName = "data/player.bmp";
	DiceInvaders::ResourceHandle playerSpriteHandle = DiceInvaders::LoadSprite(ctx->system, ctx->resources, playerSpriteName);
	DiceInvaders::Sprite playerSprite = DiceInvaders::LookupSprite(*ctx->resources, playerSpriteHandle);
	
	DiceInvaders::EntityHandle player = ctx->world->handleManager.Create();
	DiceInvaders::ComponentHandle playerModel = DiceInvaders::CreateModel(ctx->world, player, playerSprite);

	ctx->world->transforms[playerModel.index].y = 480 - 32;
	ctx->world->transforms[playerModel.index].x = 320;

	float lastTime = ctx->system->getElapsedTime();
	while (ctx->system->update())
	{
		float newTime = ctx->system->getElapsedTime();
		float move = (newTime - lastTime) * 160.0f;
		lastTime = newTime;
		
		IDiceInvaders::KeyStatus keys;
		ctx->system->getKeyStatus(keys);
		if (keys.right)
			ctx->world->transforms[playerModel.index].x += move;
		else if (keys.left)
			ctx->world->transforms[playerModel.index].x -= move;

		DiceInvaders::DrawModels(ctx->world->transforms, ctx->world->sprites);
	}

	ctx->system->destroy();

	return 0;
}



