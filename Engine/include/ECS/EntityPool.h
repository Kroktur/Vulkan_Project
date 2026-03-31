#pragma once
#include <type_traits>
#include "Sparse.h"
#include <stdexcept>
namespace KGR
{
	namespace ECS
	{

		template<typename Type, size_t allocatorSize = 1000> requires std::is_arithmetic_v<Type>
		struct EntityPool
		{
		public:
			using type = Type;
			using storage = Sparse_Set<Type, allocatorSize>;
			using unsigned_type = typename storage::unsigned_type;
			EntityPool();
			bool HasEntity(const type& e) const;
			type CreateEntity();
			void RemoveEntity(const type& e);
			const std::vector<type>& GetEntities() const;
			unsigned_type Size() const;
		private:
			void FillWithId();
			type m_lastEntityCreated;
			std::vector<type> m_freeEntities;
			storage m_storage;
		};

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		EntityPool<Type, allocatorSize>::EntityPool() : m_lastEntityCreated(static_cast<type>(-1))
		{
		}

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		bool EntityPool<Type, allocatorSize>::HasEntity(const type& e) const
		{
			return m_storage.Has(e);
		}

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		typename EntityPool<Type, allocatorSize>::type EntityPool<Type, allocatorSize>::CreateEntity()
		{
			if (m_freeEntities.empty())
				FillWithId();
			auto id = m_freeEntities.back();
			m_freeEntities.pop_back();
			m_storage.Add(id);
			return id;
		}

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		void EntityPool<Type, allocatorSize>::RemoveEntity(const type& e)
		{
			type index = m_storage.GetIndex(e);
			m_storage.Remove(e);
			m_freeEntities.push_back(e);
		}

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		const std::vector<typename EntityPool<Type, allocatorSize>::type>& EntityPool<Type, allocatorSize>::GetEntities() const
		{
			return m_storage.GetEntities();
		}

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		typename EntityPool<Type, allocatorSize>::unsigned_type EntityPool<Type, allocatorSize>::Size() const
		{
			return m_storage.Size();
		}

		template <typename Type, size_t offset> requires std::is_arithmetic_v<Type>
		void EntityPool<Type, offset>::FillWithId()
		{
			m_freeEntities.resize(offset);
			m_lastEntityCreated += offset;
			type currentEntity = m_lastEntityCreated;
			for (size_t i = 0; i < offset; ++i)
			{
				m_freeEntities[i] = currentEntity--;
			}
		}

	}
}