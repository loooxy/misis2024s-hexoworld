#pragma once
#include <render/render.hpp>
#include <zmq.hpp>

#include <condition_variable>
#include <mutex>

class Frontend {
public:
	Frontend();
	~Frontend();
	void work();

	void GetEventToRequest(std::string& ev);
	void GetCommandToRequest(std::string& com);
	void ProcessEvent(std::string& ev);

	void ProcessMap(std::string& map);
	void ProcessMapBasis(std::string& map_basis);
	void ProcessCameras(std::string& cameras);

	void ManageSignals(zmq::context_t& context);
	void InitRender();
private:
	void regular_event_update_river();
	void HandleEvents();

	std::unique_ptr<Render> render_ = nullptr;
	events_queue<Event> events;
	atomic_bool application_is_alive = true;
	atomic_bool is_running = false;
};