#pragma once

#include <thread>
#include <chrono>
#include "rxcpp/rx.hpp"

using namespace std::chrono;

class EventMachine
{
public:
	enum class State
	{
		Ready,
		Continue,
		Exit,
	};
protected:
	State state;
	std::thread thread;
	rxcpp::subjects::subject<State> subject;
public:
	EventMachine() : state(State::Ready) {}
	void Start()
	{
		// スレッド生成(ゲームメイン)
		thread = std::thread([&]() { Loop(); });
	}
	void Loop()
	{
		state = State::Continue;
		while (state == State::Continue)
		{
			auto start = high_resolution_clock::now();
			while (duration_cast<milliseconds>(high_resolution_clock::now() - start).count() < 1);
			main();
			subject.get_subscriber().on_next(state);
		}
	}
	virtual void main() = 0;
	void SetEnd()
	{
		state = State::Exit;
	}
	void Stop()
	{
		thread.join();
	}
	rxcpp::observable<State> get_obsseval()
	{
		return subject.get_observable();
	}
};