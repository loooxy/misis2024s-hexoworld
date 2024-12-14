#pragma once
#ifndef CLIENT_CLIENT_HPP_04112024
#define CLIENT_CLIENT_HPP_04112024

#include <zmq.hpp>
#include <frontend/frontend.hpp>
#include <string>
#include <atomic>

class Client {
public:
	Client();
	Client(const Client& copy) = delete;
	Client operator=(const Client& copy) = delete;
	~Client();

	void Work(const std::string address);
	void ConnectToServer(const std::string address);
private:
	void FillRequest(zmq::message_t& request);
	void ForwardMapToApp(zmq::message_t& reply_map, zmq::message_t& reply_map_basis);
	void RequestMap();
	int ForwardEventToApp(zmq::message_t& reply_event);

	std::string address_ = "tcp://localhost:5555";

	std::unique_ptr<Frontend> frontend_;
	zmq::context_t ctx_;
	zmq::socket_t client_;

	std::atomic_bool is_map_received = false;
};

#endif