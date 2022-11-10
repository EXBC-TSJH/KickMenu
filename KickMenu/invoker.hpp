#pragma once
#include "common.hpp"
#include "natives.hpp"

namespace big
{
	class native_call_context : public rage::scrNativeCallContext
	{
	public:
		native_call_context();
	private:
		std::uint64_t m_return_stack[10];
		std::uint64_t m_arg_stack[100];
	};

	class native_invoker
	{
	public:
		explicit native_invoker() = default;
		~native_invoker() = default;

		void cache_handlers();

		void begin_call();
		void end_call(rage::scrNativeHash hash);

		template <typename T>
		void push_arg(T &&value)
		{
			m_call_context.push_arg(std::forward<T>(value));
		}

		template <typename T>
		T &get_return_value()
		{
			return *m_call_context.get_return_value<T>();
		}
	private:
		native_call_context m_call_context;
		std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> m_handler_cache;
		bool m_handlers_cached = false;
	};

	inline native_invoker g_native_invoker{};
}


template <typename Ret, typename ...Args>
FORCEINLINE Ret invoke(rage::scrNativeHash hash, Args&& ...args)
{
	using namespace big;

	g_native_invoker.begin_call();
	(g_native_invoker.push_arg(std::forward<Args>(args)), ...);
	g_native_invoker.end_call(hash);

	if constexpr (!std::is_same_v<Ret, void>)
	{
		return g_native_invoker.get_return_value<Ret>();
	}
}