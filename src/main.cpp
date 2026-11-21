#include "server.hpp"

#include <iostream>
#include <csignal>
#include <cerrno>
#include <cstring>

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

	try {
		HttpServer server(8080);
		std::cout << "Server initialized\n";
		server.run(running);
	}
	catch (const std::system_error& e) {
		std::cerr << e.what() << '\n';
		return 1;
	}
}
