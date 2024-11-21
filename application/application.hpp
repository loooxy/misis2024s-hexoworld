#pragma once

#include <server/server.hpp>
#include <client/client.hpp>

#define Debug(x) std::cout << #x << " = " << x << std::endl;

class Application
{
public:
	Application();
	~Application();

	void work();
	void CreateServer(const std::string& port);
	void Connect(const std::string& address);
private:
	std::shared_ptr<Server> server;
	std::shared_ptr<Client> client;
};