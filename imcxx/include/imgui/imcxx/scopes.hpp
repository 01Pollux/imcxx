#pragma once

#include <type_traits>
#include <string>
#include "imgui/imgui.h"

namespace imcxx
{
	enum class scope_traits : char
	{
		no_dtor,
		has_dtor,
		always_dtor
	};

	template<typename _ParentTy, scope_traits _Traits = scope_traits::has_dtor, bool _HasReturn = true>
	class scope_wrap
	{
	public:
		using parent_type = _ParentTy;
		using this_type = scope_wrap;

		static constexpr scope_traits traits_value = _Traits;
		static constexpr bool has_dtor = traits_value != scope_traits::no_dtor;
		static constexpr bool has_return = _HasReturn;

		explicit scope_wrap(bool res = true) noexcept :
			m_Result(res)
		{}

		~scope_wrap()
		{
			try_destruct();
		}

		scope_wrap& operator=(const scope_wrap&)	= delete;
		scope_wrap& operator=(scope_wrap&&)			= delete;
		scope_wrap(const scope_wrap&)				= delete;
		scope_wrap(scope_wrap&& o) noexcept : 
			m_Result(o.m_Result)
		{
			if (!o.m_Result._HasMoved)
				o.m_Result._HasMoved = true;
		}

	public:
		template<typename ..._Args>
		static parent_type call(_Args&&... args)
		{
			return parent_type{ std::forward<_Args>(args)... };
		}

		[[nodiscard]] operator bool() const noexcept
		{
			if constexpr (has_return)
				return m_Result._Value;
			else return true;
		}

		template<typename _InvokeTy, typename... _Args>
		const parent_type& active_invoke(_InvokeTy&& fn, _Args&&... args) const noexcept(std::is_nothrow_invocable_v<_InvokeTy, _Args...>)
		{
			if (*this)
			{
				std::invoke(std::forward<_InvokeTy>(fn), std::forward<_Args>(args)...);
			}
			return *static_cast<const parent_type*>(this);
		}

		template<typename _InvokeTy, typename... _Args>
		const parent_type& inactive_invoke(_InvokeTy&& fn, _Args&&... args) const  noexcept(std::is_nothrow_invocable_v<_InvokeTy, _Args...>)
		{
			if (*this)
			{
				std::invoke(std::forward<_InvokeTy>(fn), std::forward<_Args>(args)...);
			}
			return *static_cast<const parent_type*>(this);
		}

		template<typename _InvokeTy, typename... _Args>
		const parent_type& passive_invoke(_InvokeTy&& fn, _Args&&... args) const  noexcept(std::is_nothrow_invocable_v<_InvokeTy, _Args...>)
		{
			std::invoke(std::forward<_InvokeTy>(fn), std::forward<_Args>(args)...);
			return *static_cast<const parent_type*>(this);
		}

		template<typename _InvokeTy>
		const parent_type& operator<<(_InvokeTy&& fn) const noexcept(std::is_nothrow_invocable_v<_InvokeTy>)
		{
			return active_invoke(std::forward<_InvokeTy>(fn));
		}



		template<typename _InvokeTy, typename... _Args>
		parent_type& active_invoke(_InvokeTy&& fn, _Args&&... args)
		{
			if (*this)
			{
				std::invoke(std::forward<_InvokeTy>(fn), std::forward<_Args>(args)...);
			}
			return *static_cast<parent_type*>(this);
		}

		template<typename _InvokeTy, typename... _Args>
		parent_type& inactive_invoke(_InvokeTy&& fn, _Args&&... args) noexcept(std::is_nothrow_invocable_v<_InvokeTy, _Args...>)
		{
			if (!*this)
			{
				std::invoke(std::forward<_InvokeTy>(fn), std::forward<_Args>(args)...);
			}
			return *static_cast<parent_type*>(this);
		}

		template<typename _InvokeTy, typename... _Args>
		parent_type& passive_invoke(_InvokeTy&& fn, _Args&&... args) noexcept(std::is_nothrow_invocable_v<_InvokeTy, _Args...>)
		{
			std::invoke(std::forward<_InvokeTy>(fn), std::forward<_Args>(args)...);
			return *static_cast<parent_type*>(this);
		}


		template<typename _InvokeTy>
		parent_type& operator<<(_InvokeTy&& fn) noexcept(std::is_nothrow_invocable_v<_InvokeTy>)
		{
			return active_invoke(std::forward<_InvokeTy>(fn));
		}

	protected:
		template<bool _HasBool = true>
		struct _compressed_type_t
		{
			bool _HasMoved : 1;
			bool _Value : 1;
			constexpr _compressed_type_t(bool v) noexcept :_Value(v), _HasMoved(false) {}
		};

		template<>
		struct _compressed_type_t<false>
		{
			bool _HasMoved : 1;
			constexpr _compressed_type_t(bool) noexcept : _HasMoved(false) {}
		};

		_compressed_type_t<has_return> m_Result;

	private:
		void try_destruct()
		{
			if (m_Result._HasMoved)
				return;

			if constexpr (has_dtor)
			{
				if constexpr (traits_value != scope_traits::always_dtor)
				{
					if constexpr (has_return)
					{
						if (!m_Result._Value)
							return;
					}
				}
				static_cast<parent_type*>(this)->destruct();
			}
		}
	};
}
