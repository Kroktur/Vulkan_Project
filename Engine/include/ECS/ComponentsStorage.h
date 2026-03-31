#pragma once

#pragma once
#include <type_traits>
#include <memory>
#include <stdexcept>
#include "Sparse.h"
#include "Component.h"


namespace KGR
{
	namespace ECS
	{

		template<typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			struct Components_Container_Storage
		{
			using type = Type;
			using storage = Sparse_Set<Type, allocatorSize>;
			using base = Com_Container_Base<Type, allocatorSize>;
			template<typename  Component>
			using derived = Com_Container_Derived<Component, Type, allocatorSize>;

			template<typename Component>
			bool Has() const;

			template<typename Component>
			void Add();

			template<typename Component>
			void Remove();

			template<typename Component>
			derived<Component>& Get() const;

			template<typename Component>
			derived<Component>& GetAndAdd();

			const std::vector<std::unique_ptr<base>>& GetPools() const;
		private:
			std::vector<std::unique_ptr<base>> m_pools;
			storage m_storage;
		};

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			template <typename  Component>
		bool Components_Container_Storage<Type, allocatorSize>::Has() const
		{
			return m_storage.Has(CompId::GetId<Component>());
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		void Components_Container_Storage<Type, allocatorSize>::Add()
		{
			if (Has<Component>())
				throw std::out_of_range("component already store");

			m_storage.Add(CompId::GetId<Component>());
			m_pools.push_back(std::make_unique<derived<Component>>());
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		void Components_Container_Storage<Type, allocatorSize>::Remove()
		{
			if (!Has<Component>())
				throw std::out_of_range("component not store");

			type index = m_storage.GetIndex(CompId::GetId<Component>());
			m_storage.Remove(CompId::GetId<Component>());
			m_pools[index] = std::move(m_pools.back());
			m_pools.pop_back();
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		typename Components_Container_Storage<Type, allocatorSize>::template derived<Component>& Components_Container_Storage<Type,
			allocatorSize>::Get() const
		{
			return static_cast<derived<Component>&>(*m_pools[m_storage.GetIndex(CompId::GetId<Component>())].get());
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			template <typename Component>
		typename Components_Container_Storage<Type, allocatorSize>::template derived<Component>& Components_Container_Storage<Type,
			allocatorSize>::GetAndAdd()
		{
			if (!Has<Component>())
				Add<Component>();
			return Get<Component>();
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			const std::vector<std::unique_ptr<typename Components_Container_Storage<Type, allocatorSize>::base>>&
			Components_Container_Storage<Type, allocatorSize>::GetPools() const
		{
			return m_pools;
		}
	}
}