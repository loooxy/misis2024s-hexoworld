#pragma once
#include <render/render.hpp>
#include <zmq.hpp>

class Frontend {
public:
	Frontend();
	~Frontend();
	void work(zmq::context_t& context);

	void GetEventToRequest(std::string& ev);
	void GetCommandToRequest(std::string& com);
	void ProcessEvent(std::string& ev);

	void ProcessMap(std::string& map);
	void ProcessMapBasis(std::string& map_basis);
	void ProcessCameras(std::string& cameras);
private:
	void regular_event_update_river();
	void HandleEvents();
	void ManageSignals(zmq::context_t& context);

	std::unique_ptr<Render> render_;
	events_queue<Event> events;
	atomic_bool application_is_alive = true;
};