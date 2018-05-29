#pragma once

#include <Windows.h>
#include <thread>

class EventMachine
{
protected:
	HANDLE handle;
	bool end;
	std::thread thread;
public:
	EventMachine() : end(false) {}
	void Start()
	{
		// イベントハンドル生成
		handle = CreateEvent(NULL, false, false, NULL);
		if (handle == NULL) {
			throw "CreateEvent エラー";
		}
		// スレッド生成(ゲームメイン)
		thread = std::thread([&]() { Loop(); });
	}
	void Loop()
	{
		while (1) {
			auto sts = WaitForSingleObject(handle, 1000);	// イベントフラグがセットされるのを待つ（1000msでタイムアウト）
			if (sts == WAIT_FAILED) {
				break;
			}
			else if (sts == WAIT_TIMEOUT) {
				if (end) {
					break;
				}
				continue;
			}
			main();
		}
	}
	virtual void main() = 0;
	void Set()
	{
		if (!end) SetEvent(handle);
	}
	void SetEnd()
	{
		end = true;
	}
	void Stop()
	{
		thread.join();
		CloseHandle(handle);
	}
};