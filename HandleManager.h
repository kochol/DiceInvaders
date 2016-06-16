#pragma once
#include <deque>
#include <vector>
#include <unordered_map>

namespace Engine
{
	struct Handle
	{
		unsigned index : 24;
		unsigned salt : 8;
	};

	struct HandleHasher
	{
		size_t operator()(const Handle& t) const
		{
			return *(size_t*)(&t);
		}
	};

	struct HandleComparer
	{
		bool operator()(const Handle& a, const Handle& b) const
		{
			return *(size_t*)(&a) == *(size_t*)(&b);
		}
	};

	template<typename T>
	using HandleHashMap = std::unordered_map<
		Handle,
		T,
		HandleHasher,
		HandleComparer>;

	class HandleManager
	{
	private:
		std::deque<Handle> _free_ids;
		std::vector<uint8_t> _salts;

	public:
		Handle Create();
		bool Valid(const Handle handle) const;
		void Destroy(const Handle handle);
	};
}
