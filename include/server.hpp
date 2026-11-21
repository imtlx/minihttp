#ifndef SERVER_HPP
#define SERVER_HPP

#include "socket.hpp"

#include <cstdint>
#include <csignal>

class HttpServer {
private:
	Socket serverSocket_;

	void handleClient(Socket client);

public:
	explicit HttpServer(std::uint16_t port);

	void run(volatile std::sig_atomic_t& running);
};

#endif //SERVER_HPP
