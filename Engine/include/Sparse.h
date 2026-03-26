#pragma once

#include <vector>
#include <ranges>
#include <algorithm>

namespace KGR
{
	namespace ECS
	{
		template<typename Type, size_t allocatorSize = 1> requires (std::is_arithmetic_v<Type>)
			struct Sparse_Set
		{
			using type = Type;
			using signed_type = std::make_signed_t<Type>;
			using unsigned_type = std::make_unsigned_t<Type>;
			static constexpr signed_type invalidValue = static_cast<signed_type>(-1);

			Sparse_Set() = default;
			bool Has(const type& e) const;
			void Add(const type& e);
			void Remove(const type& e);

			const std::vector<type>& GetEntities() const;
			unsigned_type Size() const;
			type AvailableSize() const;
			signed_type GetIndex(const type& e) const;

		private:
			void Resize(const type& index);
			void Reserve();
			std::vector<type> m_dense;
			std::vector<signed_type> m_sparse;
		};


		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			bool Sparse_Set<Type, allocatorSize>::Has(const type& e) const
		{
			return m_sparse.size() > e && m_sparse[e] != invalidValue;
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			void Sparse_Set<Type, allocatorSize>::Add(const type& e)
		{
			if (m_sparse.size() <= e)
				Resize(e);
			m_sparse[e] = static_cast<signed_type>(m_dense.size());

			if (m_dense.capacity() <= m_dense.size())
				Reserve();

			m_dense.push_back(e);
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			void Sparse_Set<Type, allocatorSize>::Remove(const type& e)
		{

			signed_type index = m_sparse[e];
			type lastEntity = std::move(m_dense.back());

			m_dense[index] = lastEntity;
			m_dense.pop_back();

			m_sparse[lastEntity] = index;
			m_sparse[e] = invalidValue;
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			const std::vector<typename Sparse_Set<Type, allocatorSize>::type>& Sparse_Set<Type, allocatorSize>::GetEntities() const
		{
			return m_dense;
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			typename Sparse_Set<Type, allocatorSize>::unsigned_type Sparse_Set<Type, allocatorSize>::Size() const
		{
			return m_dense.size();
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			typename Sparse_Set<Type, allocatorSize>::signed_type Sparse_Set<Type, allocatorSize>::GetIndex(const type& e) const
		{
			return m_sparse[e];
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			typename Sparse_Set<Type, allocatorSize>::type Sparse_Set<Type, allocatorSize>::AvailableSize() const
		{
			return m_sparse.size();
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			void Sparse_Set<Type, allocatorSize>::Resize(const type& index)
		{
			size_t lastSize = m_sparse.size();
			m_sparse.resize(lastSize + index + allocatorSize);
			std::ranges::fill(m_sparse | std::views::drop(lastSize) | std::views::take(index + allocatorSize), invalidValue);
		}

		template <typename Type, size_t allocatorSize> requires (std::is_arithmetic_v<Type>)
			void Sparse_Set<Type, allocatorSize>::Reserve()
		{
			m_dense.reserve(std::min(m_dense.capacity() * allocatorSize, m_dense.max_size()));
		}
	}
}