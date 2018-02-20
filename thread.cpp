#include "thread.h"

Task* Thread::pending_task = nullptr;
HANDLE Thread::task_ready_event = NULL;
HANDLE Thread::task_commited_event = NULL;
std::vector<HANDLE> Thread::task_completed_events;

Thread::Thread(int processor) {
	if (!task_ready_event) {
		task_ready_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	if (!task_commited_event) {
		task_commited_event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	thread = ::CreateThread(NULL, 0, &Thread::main, (LPVOID)this, 0, NULL);
	task_completed_event = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	task_completed_events.push_back(task_completed_event);

	if (thread && processor != -1) {
		::SetThreadAffinityMask(thread, 1ull << processor);
	}
}

void Thread::commit_task(Task* task) {
	pending_task = task;
	::SetEvent(task_ready_event);
	::WaitForSingleObject(task_commited_event, INFINITE);
}

void Thread::wait_for_tasks() {
	::WaitForMultipleObjects(static_cast<DWORD>(task_completed_events.size()), task_completed_events.data(), TRUE, INFINITE);
}

DWORD Thread::main(PVOID pv_param) {
	Thread* thread = static_cast<Thread*>(pv_param);

	while (true) {
		// get the next task
		::WaitForSingleObject(task_ready_event, INFINITE);
		::ResetEvent(thread->task_completed_event);
		Task* task = pending_task;
		::SetEvent(task_commited_event);

		if (!task)
            break;

        // do the job
		task->run();

		// notify that job is done
		::SetEvent(thread->task_completed_event);
	}
	return 0;
}
