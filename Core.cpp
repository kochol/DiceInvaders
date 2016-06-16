#include "Engine.h"
#include <cassert>

namespace Engine
{
	// Internal context
	struct _Context : Context
	{
		HMODULE _lib;
	};

	// Internal function forward decls
	static void _LoadLibrary(_Context *const _context);

	Context* Init(const int width, const int height)
	{
		_Context *_context = new _Context;

		_LoadLibrary(_context);

		_context->width = width;
		_context->height = height;
		_context->system->init(width, height);

		_context->resources = new Resources;
		_context->world = new World;

		return reinterpret_cast<Context*>(_context);
	}

	static void _LoadLibrary(_Context *const _context)
	{
		_context->_lib = LoadLibrary("DiceInvaders.dll");
		assert(_context->_lib);

		DiceInvadersFactoryType* factory = reinterpret_cast<DiceInvadersFactoryType*>
			(GetProcAddress(_context->_lib, "DiceInvadersFactory"));

		_context->system = factory();
		assert(_context->system != nullptr);
	}

	void Shutdown(Context *const context)
	{
		_Context* _context = reinterpret_cast<_Context*>(context);

		_context->system->destroy();
		FreeLibrary(_context->_lib);

	}
}
