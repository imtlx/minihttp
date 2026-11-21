#include "socket.hpp"

#include <unistd.h>
#include <utility>

Socket::Socket(int fd) : fd(fd) {}

Socket::~Socket() {
	if (valid()) ::close(fd);
}

Socket::Socket(Socket&& other) noexcept
	: fd(std::exchange(other.fd, -1)) {}

Socket& Socket::operator=(Socket&& other) noexcept {
	if (this != &other) {
		if (valid()) ::close(fd);
		fd = std::exchange(other.fd, -1);
	}
	return *this;
}

int Socket::get() const noexcept {
	return fd;
}

bool Socket::valid() const noexcept {
	return fd != -1;
}
