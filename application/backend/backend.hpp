#pragma once
#include <workwithmap/workwithmap.hpp>
#include <events_queue/events_queue.hpp>
#include <events/events.hpp>
#include <data_pool/data_pool.hpp>
#include <command/command.hpp>

class Backend {
public:
  Backend();
  ~Backend() = default;
  void work();

  void ProcessEvent(std::string& ev);
  void ProcessCommand(std::string& com, const std::string& id);
  void GetDataToReply(std::string& ev);
  void GetMap(std::string& map);
  void GetMapBasis(std::string& map_basis);
  void GetCameras(std::string& cameras);
  void DeleteCamera(std::string id);

private:
  std::shared_ptr<WorkWithMap> wwm;
  events_queue<Event> events_out;
  events_queue<Event> events_in;
  std::map<std::string, Camera> id_to_cam;
  std::queue<std::pair<std::string, std::shared_ptr<Command>>> commands;
  std::mutex commands_mtx;
  bool application_is_alive = true;

};