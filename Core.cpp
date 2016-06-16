#include "Engine.h"
#include <cassert>

namespace Engine
{
	// Internal context
	struct _Context : Context
	{
		HMODULE _lib;
		bool _running;
	};

	struct _FrameData : FrameData
	{
		float _prev_time;
	};

	Context *g_context;

	// Internal function forward decls
	static void _LoadLibrary(_Context *const _context);

	void Init(const Config& config)
	{
		_Context *_context = new _Context;

		_LoadLibrary(_context);

		_context->config = new Config(config);
		_context->system->init(config.screen_width, config.screen_height);

		_context->resources = new Resources;
		_context->resources->handleManager.Init(10);

		_context->world = new World;
		
		_context->frame_data = new FrameData;

		g_context = reinterpret_cast<Context*>(_context);
	}

	static void _LoadLibrary(_Context *const _context)
	{
		_context->_lib = LoadLibrary("DiceInvaders.dll");
		assert(_context->_lib);

		LibraryFactoryType* factory = reinterpret_cast<LibraryFactoryType*>
			(GetProcAddress(_context->_lib, "DiceInvadersFactory"));

		_context->system = factory();
		assert(_context->system != nullptr);
	}

	void Shutdown()
	{
		_Context *const _context = reinterpret_cast<_Context*>(g_context);

		_context->system->destroy();
		FreeLibrary(_context->_lib);
	}

	bool ShouldRun()
	{
		_Context *const _context = reinterpret_cast<_Context *const>(g_context);
		return _context->_running;
	}

	void PreUpdate()
	{
		_Context *const _context = reinterpret_cast<_Context *const>(g_context);
		_FrameData *const _frame_data = reinterpret_cast<_FrameData *const>(g_context->frame_data);

		_frame_data->collisions.clear();

		const float time = _context->system->getElapsedTime();
		_frame_data->time = time;
		_frame_data->dt = time - _frame_data->_prev_time;
		_frame_data->_prev_time = time;

		_context->system->getKeyStatus(_frame_data->keys);
	}

	void RegUpdate()
	{
		
	}

	void PostUpdate()
	{
		_Context *const _context = reinterpret_cast<_Context *const>(g_context);
		_context->_running = g_context->system->update();
	}

	void AddLayer(const LayerId layer_id, const uint16_t max_items)
	{
		assert(g_context->world->layers.find(layer_id) == g_context->world->layers.end());

		Layer *const layer = new Layer;

		layer->id = layer_id;
		layer->models.Init(max_items);

		g_context->world->layers.insert(std::make_pair(layer_id, layer));
	}
}
