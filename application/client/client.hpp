#pragma once
#ifndef CLIENT_CLIENT_HPP_04112024
#define CLIENT_CLIENT_HPP_04112024

#include <zmq.hpp>
#include <frontend/frontend.hpp>

class Client {
public:
	Client();
	Client(const Client& copy) = delete;
	Client operator=(const Client& copy) = delete;
	~Client();

	void Work();
	void ConnectToServer(const std::string& address);
private:
	void FillRequest(zmq::message_t& request);
	void ForwardDataToApp(zmq::message_t& reply_vertices, zmq::message_t& reply_trilist);

	std::unique_ptr<Frontend> frontend_;
	zmq::context_t ctx_;
	zmq::socket_t client_;
};

#endif