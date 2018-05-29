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
		// �C�x���g�n���h������
		handle = CreateEvent(NULL, false, false, NULL);
		if (handle == NULL) {
			throw "CreateEvent �G���[";
		}
		// �X���b�h����(�Q�[�����C��)
		thread = std::thread([&]() { Loop(); });
	}
	void Loop()
	{
		while (1) {
			auto sts = WaitForSingleObject(handle, 1000);	// �C�x���g�t���O���Z�b�g�����̂�҂i1000ms�Ń^�C���A�E�g�j
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