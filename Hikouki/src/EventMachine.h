#pragma once

#include <thread>
#include <chrono>
#include "rxcpp/rx.hpp"


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
	State getState() {
		return state;
	}
	rxcpp::observable<State> get_obsseval()
	{
		return subject.get_observable();
	}
};