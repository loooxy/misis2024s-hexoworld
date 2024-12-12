#pragma once
#include <render/render.hpp>

class Frontend {
public:
	explicit Frontend();
	~Frontend() = default;
	void work();

	void GetDataToRequest(std::string& ev);
	void ProcessEvent(std::string& ev);

	void ProcessMap(std::string& map);
	void ProcessMapBasis(std::string& map_basis);
private:
	std::unique_ptr<Render> render_;
	events_queue<Event> events;
};