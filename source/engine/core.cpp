#include "Engine.h"
#include <cassert>
#include <random>

namespace Engine
{
	// Internal context
	struct _Context : Context
	{
		HMODULE _lib;
		std::random_device *rnd_device;
		std::mt19937 *rnd_engine;
		std::uniform_real_distribution<float> *rnd_dist;
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
		_context->resources->caches[RESOURCE_TYPE_SPRITE].Init(10, { sizeof(ISprite*) });

		_context->world = new World;
		
		_FrameData *const _frame_data = new _FrameData;
		_frame_data->_prev_time = _context->system->getElapsedTime();
		_context->frame_data = reinterpret_cast<FrameData*>(_frame_data);

		_context->rnd_device = new std::random_device();
		_context->rnd_engine = new std::mt19937((*_context->rnd_device)());
		_context->rnd_dist = new std::uniform_real_distribution<float>(0.f, 1.f);

		g_context = reinterpret_cast<Context*>(_context);
	}

	void Shutdown()
	{
		_Context *const _context = reinterpret_cast<_Context*>(g_context);

		ExecuteComponentCallbacks(CALLBACK_STAGE_SHUTDOWN);

		delete _context->rnd_device;
		delete _context->rnd_engine;
		delete _context->rnd_dist;

		_context->system->destroy();
		FreeLibrary(_context->_lib);
	}

	void Update()
	{
		_Context *const _context = reinterpret_cast<_Context *const>(g_context);
		_FrameData *const _frame_data = reinterpret_cast<_FrameData *const>(g_context->frame_data);

		const float time = _context->system->getElapsedTime();
		_frame_data->time = time;
		_frame_data->dt = time - _frame_data->_prev_time;
		_frame_data->_prev_time = time;

		_context->system->getKeyStatus(_frame_data->keys);

		ExecuteComponentCallbacks(CALLBACK_STAGE_PHYSICS_BROAD);
		ExecuteComponentCallbacks(CALLBACK_STAGE_PHYSICS_NARROW);
		ExecuteComponentCallbacks(CALLBACK_STAGE_PRE_UPDATE);
		ExecuteComponentCallbacks(CALLBACK_STAGE_UPDATE);
		ExecuteComponentCallbacks(CALLBACK_STAGE_RENDER);
		ExecuteComponentCallbacks(CALLBACK_STAGE_POST_UPDATE);
		CleanupComponents();
		CleanupEntities();
		CleanupResources();

		const float engine_time = _context->system->getElapsedTime();

		_context->_running = g_context->system->update();

		const float library_time = _context->system->getElapsedTime();

		char debug_text[100];
		sprintf_s(debug_text, 100, "-frame_t: %.3f ms", 1000.0f * (library_time - time));
		g_context->system->drawText(0, 20, debug_text);
		sprintf_s(debug_text, 100, "--engine: %% %.2f", 100.0f * (engine_time - time) / (library_time - time));
		g_context->system->drawText(0, 40, debug_text);
		sprintf_s(debug_text, 100, "--library: %% %.2f", 100.0f * (library_time - engine_time) / (library_time - time));
		g_context->system->drawText(0, 60, debug_text);
	}

	void InitLayer(const LayerId layer_id, const uint16_t max_items)
	{
		World::LayerData *const layer = GetLayerData(layer_id);

		layer->layer_id = layer_id;
		layer->max_entities = max_items;
	}

	void CleanupEntities()
	{
		World *const world = g_context->world;
		for (const auto entity : world->to_be_freed_entities)
		{
			world->handle_manager.Destroy(entity);
		}

		world->to_be_freed_entities.clear();
	}

	float Random()
	{
		_Context *const _context = reinterpret_cast<_Context*>(g_context);
		return (*_context->rnd_dist)(*_context->rnd_engine);
	}

	bool ShouldRun()
	{
		_Context *const _context = reinterpret_cast<_Context *const>(g_context);
		return _context->_running;
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
}
