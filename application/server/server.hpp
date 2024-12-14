#pragma once
#ifndef SERVER_SERVER_HPP_04112024
#define SERVER_SERVER_HPP_04112024

#include <backend/backend.hpp>
#include <opengl/camera/camera.hpp>
#include <eventid/eventid.hpp>
#include <clock/clock.hpp>

#include <zmq.hpp>

class Server {
public:
	Server();
	Server(const Server& copy) = delete;
	Server operator=(const Server& copy) = delete;
	~Server();

	void CreateServer(const std::string port);
	void Run(const std::string port);
	void Work(const std::string port);

private:
	void FillReplyEvent(zmq::message_t& reply_event, const std::string& id); 
	void ProcessRequest(zmq::message_t& request);
	void UpdateEventsQueue();
	void ConfirmEvent(const std::string& id, const std::string& event_id);
	void FillReplyMap(zmq::message_t& reply_map);
	void FillReplyMapBasis(zmq::message_t& reply_map_basis);

	std::unique_ptr<Backend> backend_;

	zmq::context_t ctx_;
	zmq::socket_t server_;
	std::map<std::string, Camera> id_to_cam;
	std::map<std::string, std::queue<EventId>> id_to_queue;
	std::map<std::string, time_point> id_to_time;

	std::mutex id_to_queue_mtx;
};

#endif