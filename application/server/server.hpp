#pragma once
#ifndef SERVER_SERVER_HPP_04112024
#define SERVER_SERVER_HPP_04112024

#include <backend/backend.hpp>
#include <opengl/camera/camera.hpp>

#include <zmq.hpp>

class Server {
public:
	Server();
	Server(const Server& copy) = delete;
	Server operator=(const Server& copy) = delete;
	~Server();

	void CreateServer(int port);
	void Run();

private:
	void FillReply(zmq::message_t& reply_vertices, zmq::message_t& reply_trilist); // zmq::message_t& reply_cameras
	void ProcessRequest(zmq::message_t& request);

	std::unique_ptr<Backend> backend_;

	zmq::context_t ctx_;
	zmq::socket_t server_;
	std::map<std::string, Camera> id_to_cam;
};

#endif