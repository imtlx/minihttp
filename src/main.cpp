#include <iostream>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>

volatile std::sig_atomic_t running = 1;
void handle_sigint(int) {
	running = 0;
}

int main() {
	struct sigaction sa{};
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, nullptr) == -1) {
		std::cerr << "sigaction: " << strerror(errno) << '\n';
		return 1;
	}

	int server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == -1) {
		std::cerr << "socket: " << strerror(errno) << std::endl;
		return 1;
	}

	int opt = 1;
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "setsockopt: " << strerror(errno) << std::endl;
		close(server);
		return 1;
	}

	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server, (struct sockaddr*)&address, sizeof(address)) == -1) {
		std::cerr << "bind: " << strerror(errno) << std::endl;
		close(server);
		return 1;
	}

	if (listen(server, 10) == -1) {
		std::cerr << "listen: " << strerror(errno) << std::endl;
		close(server);
		return 1;
	}

	std::cout << "Listening on port 8080..." << std::endl;

	while (running) {
		int client = accept(server, nullptr, nullptr);
		if (client == -1) {
			if (errno == EINTR) {
				if (!running) break;
				continue;
			}
			std::cerr << "accept: " << strerror(errno) << std::endl;
			continue;
		}

		
		time_t tt = time(nullptr);
		tm* ti = localtime(&tt);
		std::stringstream date_stream;
		date_stream << std::put_time(ti, "%Y-%m-%d %H:%M:%S");
		
		std::string response_body =
			"<html><head>"
			"<title>HTTPServer</title>"
			"<style>h1{color:red}</style>"
			"</head><body>"
			"<h1>Hello world</h1>"
			"<p>Lorem ipsum dolor sit amet!</p>"
			"<p>Today is: " + date_stream.str() + "</p>"
			"</body></html>";

		std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: "+ std::to_string(response_body.size()) +"\r\n"
			"Connection: close\r\n"
			"\r\n" + response_body;

		size_t total_sent = 0;
		while (total_sent < response.size()) {
			ssize_t sent = send(client, response.data()+total_sent, response.size()-total_sent, 0);
			if (sent == -1) {
				if (errno == EINTR) continue;
				std::cerr << "send: " << strerror(errno) << std::endl;
				break;
			}
			if (sent == 0) break;
			total_sent += static_cast<size_t>(sent);
		}
		close(client);
	}

	std::cout << std::endl << "\nShutting down server..." << std::endl;

	close(server);
	std::cout << "Server stopped." << std::endl;
	return 0;
}
