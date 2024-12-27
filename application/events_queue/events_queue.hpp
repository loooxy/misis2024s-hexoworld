#pragma once
#include <queue>
#include <mutex>

template<class Event>
class events_queue {
public:
	void push(const std::shared_ptr<Event>& event) {
		mtx.lock();
		events.push(event);
		mtx.unlock();
	}
	std::shared_ptr<Event> pop() {
		mtx.lock();

		auto ans = events.front();
		events.pop();

		mtx.unlock();

		return ans;
	}
	void clear() {
		std::lock_guard<std::recursive_mutex> locker(mtx);
		std::queue<std::shared_ptr<Event>> empty;
		std::swap(events, empty);
	}
	bool empty() {
		std::lock_guard<std::recursive_mutex> locker(mtx);
		return events.empty();
	}
	void lock() {
		mtx.lock();
	}
	void unlock() {
		mtx.unlock();
	}
private:
	std::recursive_mutex mtx;
	std::queue<std::shared_ptr<Event>> events;
};