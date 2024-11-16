#pragma once
#include <workwithmap/workwithmap.hpp>
#include <events_queue/events_queue.hpp>
#include <events/events.hpp>
#include <data_pool/data_pool.hpp>

class Backend {
public:
  Backend();
  ~Backend() = default;
  void work();

  void ProcessData(std::string& ev);
  void GetDataToReply(std::vector<PrintingPoint>& Vertices, std::vector<uint16_t>& TriList);

private:
  std::shared_ptr<WorkWithMap> wwm;
  void regular_event_update_river();

  events_queue<Event> events;
  bool application_is_alive = true;
  data_pool data;
};