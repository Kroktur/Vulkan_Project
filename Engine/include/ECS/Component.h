#pragma once
#include <atomic>
#include <type_traits>
#include "Sparse.h"


namespace KGR
{
	namespace ECS
	{
		struct CompId
		{
		public:
			template<typename Component>
			static size_t GetId();
		private:
			static std::atomic_size_t m_id;
		};
		inline std::atomic_size_t CompId::m_id = 0;
		template <typename Component>
		size_t CompId::GetId()
		{
			static size_t id = m_id++;
			return id;
		}

		template<typename Type, size_t allocatorSize = 1> requires std::is_arithmetic_v<Type>
		struct Com_Container_Base
		{
			using type = Type;
			using storage = Sparse_Set<Type, allocatorSize>;
			using unsigned_type = typename storage::unsigned_type;
			Com_Container_Base() = default;
			virtual ~Com_Container_Base() = default;

			bool HasComponent(const type& e) const;
			virtual void Remove(const type& e) = 0;
			unsigned_type Size() const;
			const std::vector<type>& GetEntities() const;
		protected:
			storage m_storage;
		};




		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		bool Com_Container_Base<Type, allocatorSize>::HasComponent(const type& e) const
		{
			return m_storage.Has(e);
		}

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		typename Com_Container_Base<Type, allocatorSize>::unsigned_type Com_Container_Base<Type, allocatorSize>::
			Size() const
		{
			return m_storage.Size();
		}

		template <typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		const std::vector<typename Com_Container_Base<Type, allocatorSize>::type>& Com_Container_Base<Type,
			allocatorSize>::GetEntities() const
		{
			return m_storage.GetEntities();
		}

		template<typename  Component, typename Type, size_t allocatorSize = 1> requires std::is_arithmetic_v<Type>
		struct Com_Container_Derived : public Com_Container_Base<Type, allocatorSize>
		{
			using type = Type;
			Com_Container_Derived();
			void Add(const type& e);
			void Add(const type& e, Component&& c);
			void Remove(const type& e) override;
			Component& GetComponent(const type& e);
			const Component& GetComponent(const type& e) const;
			const std::vector<Component>& GetAllComponents()const;
		private:
			void Reserve();
			std::vector<Component> m_components;

		};

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		Com_Container_Derived<Component, Type, allocatorSize>::Com_Container_Derived() = default;

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		void Com_Container_Derived<Component, Type, allocatorSize>::Add(const type& e)
		{
			Add(e, std::move(Component{}));
		}

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		void Com_Container_Derived<Component, Type, allocatorSize>::Add(const type& e, Component&& c)
		{
			if (this->HasComponent(e))
				throw std::out_of_range("component already store");

			this->m_storage.Add(e);
			if (m_components.capacity() < m_components.size())
				Reserve();
			m_components.push_back(std::move(c));
		}

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		void Com_Container_Derived<Component, Type, allocatorSize>::Remove(const type& e)
		{
			if (!this->HasComponent(e))
				throw std::out_of_range("component not store");

			auto index = this->m_storage.GetIndex(e);
			this->m_storage.Remove(e);
			m_components[index] = std::move(m_components.back());
			m_components.pop_back();
		}

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		Component& Com_Container_Derived<Component, Type, allocatorSize>::GetComponent(const type& e)
		{
			return m_components[this->m_storage.GetIndex(e)];
		}

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		const Component& Com_Container_Derived<Component, Type, allocatorSize>::GetComponent(const type& e) const
		{
			return m_components[this->m_storage.GetIndex(e)];
		}

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		const std::vector<Component>& Com_Container_Derived<Component, Type, allocatorSize>::GetAllComponents() const
		{
			return m_components;
		}

		template <typename Component, typename Type, size_t allocatorSize> requires std::is_arithmetic_v<Type>
		void Com_Container_Derived<Component, Type, allocatorSize>::Reserve()
		{
			m_components.reserve(std::min(m_components.capacity() * allocatorSize, m_components.max_size()));
		}
	}
}
