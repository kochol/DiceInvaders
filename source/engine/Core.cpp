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
	static void ExecuteComponentCallbacks(CallbackStage stage)
	{
		for (auto const manager : g_context->world->components)
		{
			if (manager == nullptr)
				continue;

			if (manager->callbacks[stage] == nullptr)
				continue;

			manager->callbacks[stage](manager);
		}
	}

	void Init(const Config& config)
	{
		_Context *_context = new _Context;

		_LoadLibrary(_context);

		_context->config = new Config(config);
		_context->system->init(config.screen_width, config.screen_height);

		_context->resources = new Resources;
		_context->resources->caches[RESOURCE_TYPE_SPRITE].Init(10, { sizeof(ISprite*) });

		_context->world = new World;
		for (auto& layer : _context->world->layers)
			layer = nullptr;
		for (auto& manager : _context->world->components)
			manager = nullptr;
		
		_FrameData *const _frame_data = new _FrameData;
		_frame_data->_prev_time = _context->system->getElapsedTime();
		_context->frame_data = reinterpret_cast<FrameData*>(_frame_data);

		_context->rnd_device = new std::random_device();
		_context->rnd_engine = new std::mt19937((*_context->rnd_device)());
		_context->rnd_dist = new std::uniform_real_distribution<float>(0.f, 1.f);

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

		ExecuteComponentCallbacks(CALLBACK_STAGE_SHUTDOWN);

		delete _context->rnd_device;
		delete _context->rnd_engine;
		delete _context->rnd_dist;

		_context->system->destroy();
		FreeLibrary(_context->_lib);
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

	void InitComponents()
	{
		ExecuteComponentCallbacks(CALLBACK_STAGE_INIT);
	}

	void PreUpdate()
	{
		_Context *const _context = reinterpret_cast<_Context *const>(g_context);
		_FrameData *const _frame_data = reinterpret_cast<_FrameData *const>(g_context->frame_data);

		DetectCollisions();

		const float time = _context->system->getElapsedTime();
		_frame_data->time = time;
		_frame_data->dt = time - _frame_data->_prev_time;
		_frame_data->_prev_time = time;

		_context->system->getKeyStatus(_frame_data->keys);

		ExecuteComponentCallbacks(CALLBACK_STAGE_PRE_UPDATE);
	}

	void Update()
	{
		ExecuteComponentCallbacks(CALLBACK_STAGE_UPDATE);
	}

	void PostUpdate()
	{
		_Context *const _context = reinterpret_cast<_Context *const>(g_context);
		_context->_running = g_context->system->update();

		ExecuteComponentCallbacks(CALLBACK_STAGE_POST_UPDATE);
	}

	void InitLayer(const LayerId layer_id, const uint16_t max_items)
	{
		assert(g_context->world->layers[layer_id] == nullptr);

		Layer *const layer = new Layer;

		layer->id = layer_id;
		layer->models.Init(max_items, { sizeof(BaseComponent), sizeof(Transform), sizeof(ResourceHandle), sizeof(Collider), sizeof(Collision) });

		g_context->world->layers[layer_id] = layer;
	}

	void RegisterComponentType(const ComponentType type, const std::unordered_map<CallbackStage, ComponentManager::Callback>& callbacks)
	{
		assert(g_context->world->components[type] == nullptr);

		ComponentManager *const manager = new ComponentManager;
		
		manager->type = type;

		for (auto& callback : manager->callbacks)
			callback = nullptr;

		for (const auto callback : callbacks)
			manager->callbacks[callback.first] = callback.second;
		
		g_context->world->components[type] = manager;
	}

	ComponentHandle CreateComponent(const EntityHandle& entity, const ComponentType type, const ComponentHandle& model)
	{
		Engine::ComponentManager *const manager = Engine::GetComponentManager(type);

		Engine::ComponentHandle component;
		component.index = manager->components.Alloc();
		component.header.layer = entity.header.layer;
		component.header.type = manager->type;

		manager->componentMap[entity] = component;

		BaseComponent *const componentData = manager->components.Resolve<BaseComponent>(component.index);
		componentData->entity = entity;
		componentData->model = model;

		return component;
	}
}
