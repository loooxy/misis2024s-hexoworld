#pragma once
#include <render/render.hpp>

class Frontend {
public:
	Frontend();
	~Frontend();
	void work();

	void GetDataToRequest(std::string& ev);
	void ProcessEvent(std::string& ev);

	void ProcessMap(std::string& map);
	void ProcessMapBasis(std::string& map_basis);
private:
	void regular_event_update_river();

	std::unique_ptr<Render> render_;
	events_queue<Event> events;
	bool application_is_alive = true;
};