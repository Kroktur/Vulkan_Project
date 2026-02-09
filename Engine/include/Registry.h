#pragma once
#include <type_traits>
#include "ComponentsStorage.h"
#include "EntityPool.h"
#include "Filtre.h"
#include "View.h"

namespace KGR
{
	namespace ECS
	{

		template<typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
			struct Registry
		{
			using type = Type;
			type CreateEntity();

			void DestroyEntity(const type& e);


			template<CompType Component>
			void AddComponent(const type& e);

			template<CompType... Component>
			void AddComponents(const type& e);

			

			template<CompType Components>
			void AddComponent(const type& e, Components&& c);

			template<CompType... Components>
			void AddComponents(const type& e,Components&&... c);

			template<CompType Component>
			void RemoveComponent(const type& e);


			template<CompType... Component>
			void RemoveComponents(const type& e);

			template<CompType Component>
			bool HasComponent(const type& e) const;


			template<CompType... Component>
			bool HasAllComponents(const type& e) const;


			template<CompType... Component>
			bool HasAnyComponents(const type& e) const ;

			template<CompType Component>
			Component& GetComponent(const type& e);

			const std::vector<type>& GetAllEntities() const;

			template<CompType... Components>
			Filter<Type,offset> GetAllComponentsFilter();

			template<CompType... Components>
			Filter<Type, offset> GetAnyComponentsFilter();

			template<CompType Components>
			Filter<Type, offset> GetFilter();

			template<CompType Components>
			View<Type,offset> GetView();

			template<CompType... Components>
			View<Type, offset> GetAllComponentsView();

			template<CompType... Components>
			View<Type, offset> GetAnyComponentsView();

		private:
			bool HasEntity(const type& e) const ;
			Components_Container_Storage<Type, offset> m_pool;
			EntityPool<Type, offset * 10> m_entityPool;
		};


		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		typename Registry<Type, offset>::type Registry<Type, offset>::CreateEntity()
		{
			return m_entityPool.CreateEntity();
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		void Registry<Type, offset>::DestroyEntity(const type& e)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");
			m_entityPool.RemoveEntity(e);
			auto& pool = m_pool.GetPools();
			for (auto& it : pool)
			{
				if (it->HasComponent(e))
					it->Remove(e);
			}
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Component>
		void Registry<Type, offset>::AddComponent(const type& e)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");

			m_pool.template GetAndAdd<Component>().AddComponent(e);
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Components>
		void Registry<Type, offset>::AddComponent(const type& e, Components&& c)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");
			m_pool.template GetAndAdd<Components>().AddComponent(e, std::move(c));
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Component>
		void Registry<Type, offset>::RemoveComponent(const type& e)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");

			m_pool.template Get<Component>().Remove(e);
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Component>
		bool Registry<Type, offset>::HasComponent(const type& e) const 
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");

			if ( !m_pool.template Has<Component>())
				return false;

			if ( !m_pool.template Get<Component>().HasComponent(e))
				return false;

			return true;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType ... Component>
		bool Registry<Type, offset>::HasAllComponents(const type& e) const
		{
			{
				if (!HasEntity(e))
					throw std::out_of_range("entity not stored");

				if ((... || !m_pool.template Has<Component>()))
					return false;

				if ((... || !m_pool.template Get<Component>().HasComponent(e)))
					return false;

				return true;
			}
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Components>
		View<Type, offset> Registry<Type, offset>::GetView()
		{
			View<Type, offset> view;
			for (auto& e : m_entityPool.GetEntities())
			{
				if (HasComponent<Components>(e))
					view.AddEntity(e);
			}
			return view;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType ... Components>
		View<Type, offset> Registry<Type, offset>::GetAllComponentsView()
		{
			View<Type, offset> view;
			for (auto& e : m_entityPool.GetEntities())
			{
				if (HasAllComponents<Components...>(e))
					view.AddEntity(e);
			}
			return view;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType ... Components>
		View<Type, offset> Registry<Type, offset>::GetAnyComponentsView()
		{
			View<Type, offset> view;
			for (auto& e : m_entityPool.GetEntities())
			{
				if (HasAnyComponents<Components...>(e))
					view.AddEntity(e);
			}
			return view;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType ... Component>
		bool Registry<Type, offset>::HasAnyComponents(const type& e) const 
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");

			if ((... || (m_pool.template Has<Component>() == true ? m_pool.template Get<Component>().HasComponent(e) : false)))
				return true;

			return false;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType ... Components>
		Filter<Type, offset> Registry<Type, offset>::GetAnyComponentsFilter()
		{
			Filter<Type, offset> filter(m_entityPool.Size());
			(..., (filter.template Add<Components>(&m_pool.template GetAndAdd<Components>())));
			for (auto& e : m_entityPool.GetEntities())
			{
				if (HasAnyComponents<Components...>())
					filter.AddEntity(e);
			}
			return filter;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Components>
		Filter<Type, offset> Registry<Type, offset>::GetFilter()
		{
			Filter<Type, offset> filter(m_entityPool.Size());
			filter.template Add<Components>(&m_pool.template GetAndAdd<Components>());
			for (auto& e : m_entityPool.GetEntities())
			{
				if (HasComponent<Components>(e))
					filter.AddEntity(e);
			}
			return filter;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType... Component>
		void Registry<Type, offset>::AddComponents(const type& e)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");

			(..., m_pool.template GetAndAdd<Component>().AddComponent(e));
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType ... Components>
		void Registry<Type, offset>::AddComponents(const type& e, Components&&... c)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");
			(..., m_pool.template GetAndAdd<Components>().AddComponent(e, std::move(c)));
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType... Component>
		void Registry<Type, offset>::RemoveComponents(const type& e)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");

			(..., m_pool.template Get<Component>().Remove(e));
			
		}


		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Component>
		Component& Registry<Type, offset>::GetComponent(const type& e)
		{
			if (!HasEntity(e))
				throw std::out_of_range("entity not stored");

			auto& component = m_pool.template Get<Component>();
			return component.GetComponent(e);
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		const std::vector<typename Registry<Type, offset>::type>& Registry<Type, offset>::GetAllEntities() const
		{
			return m_entityPool.GetEntities();
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType ... Components>
		Filter<Type, offset> Registry<Type, offset>::GetAllComponentsFilter()
		{
			Filter<Type, offset> filter(m_entityPool.Size());
			(..., (filter.template Add<Components>( &m_pool.template Get<Components>() ) ) );
			for (auto& e : m_entityPool.GetEntities())
			{
				if (HasAllComponents<Components...>(e))
					filter.AddEntity(e);
			}
			return filter;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		bool Registry<Type, offset>::HasEntity(const type& e) const
		{
			return m_entityPool.HasEntity(e);
		}
	}
}
