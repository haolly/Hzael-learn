#pragma once
#include <assert.h>

namespace Hazel
{
	class RefCounted
	{
	public:
		virtual ~RefCounted() = default;

		void IncRefCount() const
		{
			m_RefCount++;
		}
		void DecRefCount() const
		{
			assert (m_RefCount > 0);
			m_RefCount--;
		}

		uint32_t GetRefCount() const { return m_RefCount;}
	private:
		//todo, atomic
		mutable uint32_t m_RefCount = 0;
	};

	/**
	 * \brief  管理 RefCounted 类的计数器加减逻辑
	 * \tparam T 
	 */
	template<typename T>
	class Ref
	{
	public:
		Ref() : m_Instance(nullptr)
		{
		}
		
		Ref(std::nullptr_t n) : m_Instance(nullptr)
		{
		}

		Ref(T* instance): m_Instance(instance)
		{
			static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");
			IncRef();
		}

		template<typename T2>
		Ref(Ref<T2>& other)
		{
			m_Instance = (T*)other.m_Instance;
			IncRef();
		}
		
		// Move semantic
		template<typename T2>
		Ref(Ref<T2>&& other)
		{
			m_Instance = (T*)other.m_Instance;
			other.m_Instance = nullptr;
		}

		~Ref()
		{
			DecRef();
		}

		Ref(const Ref<T>& other): m_Instance(other.m_Instance)
		{
			IncRef();
		}

		Ref& operator=(std::nullptr_t)
		{
			DecRef();
			// refCount is stored in m_Instance, so when refCount is greater than 1, there MUAST be some other Ref exist
			m_Instance = nullptr;
			return *this;
		}

		Ref& operator=(const Ref<T>& other)
		{
			other.IncRef();
			//TODO, what if this refcount is zero?
			DecRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(const Ref<T2>& other)
		{
			other.IncRef();
			DecRef();
			
			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		Ref& operator=(Ref<T2>&& other)
		{
			DecRef();

			// T2 is a friend class of this, so we can access T2's private variable
			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr;}
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance;}
		const T* operator->() const { return m_Instance;}

		T& operator*() { return *m_Instance;}
		const T& operator*() const { return *m_Instance;}

		T* Raw() { return m_Instance;}
		const T* Raw() const { return m_Instance; }

		void Reset(T* instance = nullptr)
		{
			DecRef();
			m_Instance = instance;
		}

		template<typename T2>
		Ref<T2> As() const
		{
			return Ref<T>(*this);
		}

		template<typename... Args>
		static Ref<T> Create(Args&& ... args)
		{
			return Ref<T>(new T(std::forward<Args>(args)...) );
		}

		bool operator==(const Ref<T>& other) const
		{
			// Note, pointer equals
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const Ref<T>& other) const
		{
			return !(*this == other);
		}

		bool EqualsObject(const Ref<T>& other)
		{
			if(!m_Instance || !other.m_Instance)
				return false;
			// Note, guid equals, ie, point to the same object
			return *m_Instance == *other.m_Instance;
		}

	private:
		void IncRef() const
		{
			if(m_Instance)
				m_Instance->IncRefCount();
		}

		void DecRef() const
		{
			if(m_Instance)
			{
				m_Instance->DecRefCount();
				if(m_Instance->GetRefCount() == 0)
				{
					delete m_Instance;
				}
			}
		}
		template<typename T2>
		friend class Ref;
		T* m_Instance;
	};
}
