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
  void GetDataToReply(std::string& ev);
  void GetMap(std::string& map);
  void GetMapBasis(std::string& map_basis);

private:
  std::shared_ptr<WorkWithMap> wwm;
  events_queue<Event> events_out;
  events_queue<Event> events_in;
  bool application_is_alive = true;

};