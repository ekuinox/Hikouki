#pragma once

#include <chrono>
#include <functional>

namespace trau
{
	using namespace std::chrono;
	using clock = high_resolution_clock;
	class Timer
	{
	private:
		clock::time_point started;
		nanoseconds diff;
	public:
		Timer()
		{
			started = clock::now();
			diff = nanoseconds(0);
		}
		auto start() -> void
		{
			started = clock::now();
		}
		auto end() -> void
		{
			diff = clock::now() - started;
		}
		auto getSeconds() -> float
		{
			return duration_cast<nanoseconds>(diff).count() / 1000000000.0f;
		}
		template <class _T>
		auto getDiff() -> _T
		{
			return duration_cast<_T>(diff);
		}
		auto getCurrentMs() -> float
		{
			return duration_cast<nanoseconds>(clock::now() - started).count() / 1000000000.0f;
		}
		auto run(std::function<void(void)> func) -> void
		{
			start();
			func();
			end();
		}
	};
}
