#pragma once
#include <type_traits>
#include "Sparse.h"
#include "Component.h"
#include <memory>
namespace KGR
{
	namespace ECS
	{
		template<typename Type, size_t offset > requires (std::is_arithmetic_v<Type>)
			struct Registry;

		template<typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
			struct Filter
		{
			friend Registry<Type, offset>;
			using type = Type;
			using storage = Sparse_Storage<Type, offset>;
			using base = Component_Container_Base<Type, offset>;
			template<CompType Component>
			using derived = Component_Container<Component, Type, offset>;

			const std::vector<type> GetEntities() const;

			template<CompType Component>
			Component& GetComponent(const type& e);

			type Size() const;

		private:
			Filter(size_t size);

			template<CompType Component>
			void Add(derived<Component>* ptr);

			void AddEntity(const type& e);

			std::vector<type> m_entities;
			std::vector<base*> m_pools;
			storage m_storage;
		};


		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		const std::vector<typename Filter<Type, offset>::type> Filter<Type, offset>::GetEntities() const
		{
			return m_entities;
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Component>
		Component& Filter<Type, offset>::GetComponent(const type& e)
		{
			if (!m_storage.Has(CompId::GetId<Component>()))
				throw std::out_of_range("entity not store");
			return static_cast<derived<Component>&>(*m_pools[m_storage.GetIndex(CompId::GetId<Component>())]).GetComponent(e);
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		typename Filter<Type, offset>::type Filter<Type, offset>::Size() const
		{
			return m_entities.size();
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		Filter<Type, offset>::Filter(size_t size)
		{
			m_entities.reserve(size);
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		template <CompType Component>
		void Filter<Type, offset>::Add(derived<Component>* ptr)
		{
			m_storage.Add(CompId::GetId<Component>());
			m_pools.push_back(ptr);
		}

		template <typename Type, size_t offset> requires (std::is_arithmetic_v<Type>)
		void Filter<Type, offset>::AddEntity(const type& e)
		{
			m_entities.push_back(e);
		}
	}
}
