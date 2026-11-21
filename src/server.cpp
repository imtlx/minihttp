#include "server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <system_error>
#include <cstddef>
#include <algorithm>
#include <string>

namespace {

Socket createListeningSocket(std::uint16_t port) {
	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		throw std::system_error(
			errno,
			std::generic_category(),
			"socket"
		);
	}

	Socket sock(fd);

	constexpr int opt = 1;
	if (::setsockopt(
		sock.get(),
		SOL_SOCKET,
		SO_REUSEADDR,
		&opt,
		sizeof(opt)
	) == -1) {
		throw std::system_error(
			errno,
			std::generic_category(),
			"setsockopt"
		);
	}

	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind(
		sock.get(),
		reinterpret_cast<sockaddr*>(&address),
		sizeof(address)
	) == -1) {
		throw std::system_error(
			errno,
			std::generic_category(),
			"bind"
		);
	}

	
	if (::listen(sock.get(), 10) == -1) {
		throw std::system_error(
			errno,
			std::generic_category(),
			"listen"
		);
	}

	return sock;
}

} // namespace

HttpServer::HttpServer(std::uint16_t port) :
	serverSocket_(createListeningSocket(port)) {}

void HttpServer::handleClient(Socket client) {
	std::string body = "<h1>Hello from MiniHTTP!</h1>";

	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\n" + body;

	constexpr size_t MAX_HEADER_SIZE = 16*1024;
	constexpr size_t BUFFER_SIZE = 4096;

	std::string request;
	request.reserve(BUFFER_SIZE);

	char buffer[BUFFER_SIZE];
	bool headersComplete = false;

	while (!headersComplete) {
		if (request.size() >= MAX_HEADER_SIZE) {
			std::cerr << "Request headers too large\n";
			return;
		}

		size_t remaining = MAX_HEADER_SIZE - request.size();
		size_t bytesToRead = std::min(sizeof(buffer), remaining);

		ssize_t received = ::recv(
			client.get(),
			buffer,
			bytesToRead,
			0
		);

		if (received == -1) {
			if (errno == EINTR) continue;
			std::cerr << "recv: " << strerror(errno) << std::endl;
			return;
		}
		if (received == 0) {
			std::cerr << "Incomplete HTTP request: client disconnected\n";
			return;
		}

		request.append(buffer, static_cast<size_t>(received));
		if (request.find("\r\n\r\n") != std::string::npos) {
			headersComplete = true;
		}
	}

	std::cout.write(request.data(), request.size());
	std::cout << '\n';

	size_t totalSent = 0;
	while (totalSent < response.size()) {
		ssize_t sent = send(
			client.get(),
			response.data()+totalSent,
			response.size()-totalSent,
			MSG_NOSIGNAL
		);
		if (sent == -1) {
			if (errno == EINTR) continue;
			std::cerr << "send: " << strerror(errno) << std::endl;
			break;
		}

		if (sent == 0) break;
		totalSent += static_cast<size_t>(sent);
	}
}

void HttpServer::run(volatile std::sig_atomic_t& running) {
	while (running) {
		int clientFd = ::accept(serverSocket_.get(), nullptr, nullptr);
		if (clientFd == -1) {
			if (errno == EINTR || errno == ECONNABORTED) {
				if (!running) break;
				continue;
			}
			std::cerr << "accept: " << std::strerror(errno) << '\n';
			continue;
		}

		Socket client(clientFd);

		handleClient(std::move(client));
	}
}

