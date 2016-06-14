#include <windows.h>
#include <cassert>
#include <cstdio>
#include "DiceInvaders.h"
#include <cstdint>
#include <unordered_map>
#include <array>

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



struct Transform
{
	float x;
	float y;
};

struct Player
{
	Transform *transform;
	ISprite *sprite;

	float lastFired;
	uint8_t health;
};

struct Rocket
{
	Transform *transform;
	ISprite *sprite;
};

struct Alien
{
	Transform *transform;
	ISprite *sprite;
	float lastDroped;
};

struct Bomb
{
	Transform *transform;
	ISprite *sprite;
};

struct GlobalMemory
{
	std::array<Transform, 100> transforms;
	std::array<ISprite**, 100> sprites;

	std::array<Player, 1> players;
	std::array<Rocket, 100> rockets;
	std::array<Alien, 100> aliens;
	std::array<Bomb, 100> bombs;

	std::unordered_map<std::string, ISprite*> spriteMap;

	GlobalMemory()
	{
	}
};

void CreateDrawable(const char* const spriteName, const Transform** outTransform, const ISprite** outSprite)
{

}

void DrawScene(const Transform* firstTransform, ISprite* firstSprite, const uint16_t count)
{
	for (uint16_t i = 0; i < count; ++i)
	{
		const Transform* lTransform = firstTransform + i;
		(firstSprite + i)->draw(lTransform->x, lTransform->y);
	}
}

int APIENTRY WinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	DiceInvadersLib lib("DiceInvaders.dll");
	IDiceInvaders* system = lib.get();

	system->init(640, 480);

	ISprite* sprite = system->createSprite("data/player.bmp");

	float horizontalPosition = 320;
	float lastTime = system->getElapsedTime();
	while (system->update())
	{
		sprite->draw(int(horizontalPosition), 480-32);

		float newTime = system->getElapsedTime();
		float move = (newTime - lastTime) * 160.0f;
		lastTime = newTime;
		
		IDiceInvaders::KeyStatus keys;
		system->getKeyStatus(keys);
		if (keys.right)
			horizontalPosition += move;
		else if (keys.left)
			horizontalPosition -= move;
	}

	sprite->destroy();
	system->destroy();

	return 0;
}



