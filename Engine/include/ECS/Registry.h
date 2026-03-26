#pragma once
#include <type_traits>
#include "ComponentsStorage.h"
#include "EntityPool.h"
namespace KGR
{
	namespace ECS
	{

		template<typename Type, size_t allocatorSize, size_t entityPool = 10 * allocatorSize> requires (std::is_arithmetic_v<Type>)
			struct Registry
		{
			using type = Type;
			using view = std::vector<Type>;
			type CreateEntity();
			void DestroyEntity(const type& e);

			template<typename  Component>
			void AddComponent(const type& e);
			template<typename Component>
			void AddComponent(const type& e, Component&& c);

			template<typename... Components>
			void AddComponents(const type& e);
			template<typename... Components>
			void AddComponents(const type& e, Components&&... c);

			template<typename Component>
			void RemoveComponent(const type& e);
			template<typename... Components>
			void RemoveComponents(const type& e);

			template<typename Component>
			bool HasComponent(const type& e) const;
			template<typename... Components>
			bool HasAllComponents(const type& e) const;
			template<typename... Components>
			bool HasAnyComponents(const type& e) const;


			template<typename Component>
			Component& GetComponent(const type& e) const;
			template<typename Component>
			const std::vector<Component>& GetAllComponents();
			bool HasEntity(const type& e) const;
			const std::vector<type>& GetAllEntities() const;

			template<typename Component>
			const view& GetView() const;

			template<typename... Components>
			view GetAllComponentsView();

			template<typename... Components>
			view GetAnyComponentsView();



		private:

			Components_Container_Storage<Type, allocatorSize> m_pool;
			EntityPool<Type, entityPool> m_entityPool;
		};


		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			typename Registry<Type, allocatorSize, entityPool>::type Registry<Type, allocatorSize, entityPool>::CreateEntity()
		{
			return m_entityPool.CreateEntity();
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			void Registry<Type, allocatorSize, entityPool>::DestroyEntity(const type& e)
		{

			m_entityPool.RemoveEntity(e);
			auto& pool = m_pool.GetPools();
			for (auto& it : pool)
			{
				if (it->HasComponent(e))
					it->Remove(e);
			}
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		void Registry<Type, allocatorSize, entityPool>::AddComponent(const type& e)
		{

			m_pool.template GetAndAdd<Component>().Add(e);
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		void Registry<Type, allocatorSize, entityPool>::AddComponent(const type& e, Component&& c)
		{

			m_pool.template GetAndAdd<Component>().Add(e, std::move(c));
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		void Registry<Type, allocatorSize, entityPool>::RemoveComponent(const type& e)
		{
			m_pool.template Get<Component>().Remove(e);
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		bool Registry<Type, allocatorSize, entityPool>::HasComponent(const type& e) const
		{

			if (!m_pool.template Has<Component>())
				return false;

			if (!m_pool.template Get<Component>().HasComponent(e))
				return false;

			return true;
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename ... Components>
		bool Registry<Type, allocatorSize, entityPool>::HasAllComponents(const type& e) const
		{

			if ((... || (!m_pool.template Has<Components>() ? false : !m_pool.template Get<Components>().HasComponent(e))))
				return false;
			return true;
		}




		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename ... Components>
		bool Registry<Type, allocatorSize, entityPool>::HasAnyComponents(const type& e) const
		{

			if ((... || (m_pool.template Has<Components>() == true ? m_pool.template Get<Components>().HasComponent(e) : false)))
				return true;
			return false;
		}




		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename... Components>
		void Registry<Type, allocatorSize, entityPool>::AddComponents(const type& e)
		{

			(..., m_pool.template GetAndAdd<Components>().Add(e));
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename ... Components>
		void Registry<Type, allocatorSize, entityPool>::AddComponents(const type& e, Components&&... c)
		{

			(..., m_pool.template GetAndAdd<Components>().Add(e, std::move(c)));
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename... Components>
		void Registry < Type, allocatorSize, entityPool > ::RemoveComponents(const type& e)
		{
			(..., m_pool.template Get<Components>().Remove(e));

		}


		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		Component& Registry<Type, allocatorSize, entityPool>::GetComponent(const type& e) const
		{
			return  m_pool.template Get<Component>().GetComponent(e);
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		const std::vector<Component>& Registry<Type, allocatorSize, entityPool>::GetAllComponents()
		{
			return  m_pool.template GetAndAdd<Component>().GetAllComponents();
		}


		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			const std::vector<typename Registry<Type, allocatorSize, entityPool>::type>& Registry<Type, allocatorSize, entityPool>::GetAllEntities() const
		{
			return m_entityPool.GetEntities();
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		const typename Registry<Type, allocatorSize, entityPool>::view& Registry<Type, allocatorSize, entityPool>::GetView() const
		{
			return m_pool.template GetAndAdd<Component>().GetEntities();
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename ... Components>
		typename Registry<Type, allocatorSize, entityPool>::view Registry<Type, allocatorSize, entityPool>::GetAllComponentsView()
		{
			const std::vector<type>* smallestEntity = nullptr;
			(..., [&]()
				{
					auto& entities = m_pool.template GetAndAdd<Components>().GetEntities();
					if (!smallestEntity || entities.size() < smallestEntity->size())
						smallestEntity = &entities;
				}());
			view result;
			result.reserve(smallestEntity->size());
			for (auto& e : *smallestEntity)
			{
				if (HasAllComponents<Components...>(e))
					result.push_back(e);
			}
			return result;
		}

		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			template <typename ... Components>
		typename Registry<Type, allocatorSize, entityPool>::view Registry<Type, allocatorSize, entityPool>::GetAnyComponentsView()
		{
			std::vector<type> biggestEntity;
			(..., [&]()
				{
					auto entities = m_pool.template GetAndAdd<Components>().GetEntities();
					if (biggestEntity.empty() || entities.size() > biggestEntity.size())
						biggestEntity = entities;
				}());
			view result;
			result.reserve(biggestEntity->size());
			for (auto& e : biggestEntity)
			{
				if (HasAnyComponents<Components...>(e))
					result.push_back(e);
			}
			return result;

		}


		template <typename Type, size_t allocatorSize, size_t entityPool> requires (std::is_arithmetic_v<Type>)
			bool Registry<Type, allocatorSize, entityPool>::HasEntity(const type& e) const
		{
			return m_entityPool.HasEntity(e);
		}
	}
}
