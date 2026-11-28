#include "http/request.hpp"

#include <string>
#include <string_view>

namespace http::request {

RequestMethod parseMethod(std::string_view method) {
	if (method == "GET")	return RequestMethod::GET;
	if (method == "HEAD")	return RequestMethod::HEAD;
	if (method == "POST")	return RequestMethod::POST;
	if (method == "PUT")	return RequestMethod::PUT;
	if (method == "DELETE")	return RequestMethod::DELETE;
	if (method == "CONNECT")return RequestMethod::CONNECT;
	if (method == "OPTIONS")return RequestMethod::OPTIONS;
	if (method == "TRACE")	return RequestMethod::TRACE;
	if (method == "PATCH")	return RequestMethod::PATCH;

	return RequestMethod::UNKNOWN;
}

std::string_view toString(RequestMethod method) {
	switch (method) {
		case RequestMethod::GET:	return "GET";
		case RequestMethod::HEAD:	return "HEAD";
		case RequestMethod::POST:	return "POST";
		case RequestMethod::PUT:	return "PUT";
		case RequestMethod::DELETE:	return "DELETE";
		case RequestMethod::CONNECT:	return "CONNECT";
		case RequestMethod::OPTIONS:	return "OPTIONS";
		case RequestMethod::TRACE:	return "TRACE";
		case RequestMethod::PATCH:	return "PATCH";

		default: return "UNKNOWN";
	}
}

} // namespace http::request

