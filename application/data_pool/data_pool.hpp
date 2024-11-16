#pragma once
#include <hexoworld/includes.hpp>
#include <application/server/server.hpp>

class data_pool {
public:
	bool check() {
		std::lock_guard<std::mutex> locker(mtx);
		return need_update_buffers;
	}
	void get(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList)
	{
		std::lock_guard<std::mutex> locker(mtx);
		need_update_buffers = false;
		std::swap(Vertices, Vertices_);
		std::swap(TriList, TriList_);
	}
	void set(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList) {
		std::lock_guard<std::mutex> locker(mtx);
		need_update_buffers = true;
		std::swap(Vertices, Vertices_);
		std::swap(TriList, TriList_);
	}
private:
	std::vector<PrintingPoint> Vertices_;
	std::vector<uint16_t> TriList_;
	bool need_update_buffers = false;
	std::mutex mtx;
};