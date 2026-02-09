#pragma once
#include <type_traits>
#include <atomic>
#include "Hasher.h"
#include <string>

template<typename T>
struct TypeName;

#define REGISTER_RTTI(T)                   \
template<>                                 \
struct TypeName<T>                         \
{                                          \
	static constexpr std::string Name()    \
	{                                      \
		return #T;                         \
	}                                      \
};

template<typename T>
concept RTTI_Type = requires
{
	{TypeName<T>::Name()};
};

namespace KGR
{


	namespace RTTI
	{

		template<typename Type>
		class Instance;

		struct Counter
		{
			template<typename Type>
			static size_t GetTypeId()
			{
				static size_t id = m_id++;
				return id;
			}
			template<typename Type>
			static size_t GetTypeId(const Type& value)
			{
				return  Counter::GetTypeId<Type>();
			}
			template<typename Type, typename typeTest >
			static bool IsType(const typeTest& value)
			{
				return Counter::GetTypeId(value) == Counter::GetTypeId<Type>();
			}
		private:
			static std::atomic_size_t m_id;
		};
		struct Hasher
		{
			template<RTTI_Type Type>
			static std::uint64_t GetTypeId()
			{
				static std::string name = TypeName<Type>::Name();
				static std::uint64_t id = Hash::FNV1aHash(name.c_str(), name.size());
				return id;
			}
			template<typename Type>
			static std::uint64_t GetTypeId(const Type&)
			{
				return  Hasher::GetTypeId<Type>();
			}
			template<typename Type, typename typeTest >
			static bool IsType(const typeTest& value)
			{
				return Hasher::GetTypeId(value) == Hasher::GetTypeId<Type>();
			}
		};

		template<typename Type>
		struct Entity
		{
			friend Type;
			size_t GetInstanceId() const
			{
				return m_instanceId;
			}
		private:
			Entity() : m_instanceId(RTTI::Instance<Type>::GetId())
			{

			}
			size_t m_instanceId;
		};


		template<typename Type>
		class Instance
		{
		public:
			friend Entity<Type>;
			static size_t InstanceCount()
			{
				return m_id;
			}
		private:
			static size_t GetId()
			{
				return m_id++;
			}
			static std::atomic_size_t m_id;
		};
		static std::atomic_size_t m_id;
	}

	inline std::atomic_size_t RTTI::Counter::m_id = 0;
	template<typename Type>
	inline std::atomic_size_t RTTI::Instance<Type>::m_id = 0;

}