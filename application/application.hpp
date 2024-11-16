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
private:
	std::shared_ptr<Server> server;
	std::shared_ptr<Client> client;
};