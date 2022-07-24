#pragma once

#include <iostream>
#include <mutex>

class Logger
{
public:
	template<typename T, typename... Rest>
	static void Log(const T& arg, const Rest&... rest)
	{
		std::lock_guard<std::recursive_mutex> lock(m_Mutex);

		std::cout << arg << ' ';

		if constexpr (sizeof...(rest) > 0)
			Log(rest...);
		else
			std::cout << std::endl;
	}
private:
	static inline std::recursive_mutex m_Mutex;
};
