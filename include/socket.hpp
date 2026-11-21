#ifndef SOCKET_HPP
#define SOCKET_HPP

class Socket {
private:
	int fd = -1;

public:
	explicit Socket(int fd);
	~Socket();

	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	Socket(Socket&& other) noexcept;
	Socket& operator=(Socket&& other) noexcept;

	int get() const noexcept;
	bool valid() const noexcept;
};

#endif // SOCKET_HPP
