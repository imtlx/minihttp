#include "socket.hpp"

#include <cassert>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

int main() {
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(fd != -1);

	Socket first(fd);
	assert(first.valid());

	Socket second(std::move(first));

	assert(!first.valid());
	assert(second.valid());

	Socket third(-1);
	third = std::move(second);

	assert(!second.valid());
	assert(third.valid());

	return 0;
}
