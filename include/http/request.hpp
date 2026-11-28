#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <string_view>
#include <unordered_map>

namespace http::request {

enum class RequestMethod {
	UNKNOWN = 0,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH
};

struct RequestLine {
	RequestMethod method;
	std::string target;
	std::string version;
};

struct Request {
	RequestLine line;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
};

RequestMethod parseMethod(std::string_view method);
std::string_view toString(RequestMethod method);

// namespace http::request

#endif // HTTP_REQUEST_HPP

