#pragma once

#include <server/server.hpp>
#include <client/client.hpp>
#include <zmq.hpp>

#define Debug(x) std::cout << #x << " = " << x << std::endl;

class Application
{
public:
	Application();
	~Application();

	void work();
private:
	void CreateServer(const std::string port);
	void ReadCommands(zmq::context_t& context);
	std::shared_ptr<Server> server;
	std::shared_ptr<Client> client;
};