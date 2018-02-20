#pragma once

#include <vector>

#define NOMINMAX 
#include <Windows.h>

class Task {
public:
	virtual void run() = 0;
};

class Thread {
public:
	Thread(int processor = -1);
	static void commit_task(Task* task);
	static void wait_for_tasks();

private:
	static DWORD WINAPI main(PVOID pv_param);

private:
	HANDLE thread;
	HANDLE task_completed_event;

	static Task* pending_task;
	static HANDLE task_ready_event;
	static HANDLE task_commited_event;
	static std::vector<HANDLE> task_completed_events;
};
