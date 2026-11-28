#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include "http/request.hpp"

#include <string>
#include <string_view>
#include <optional>

namespace http::parser {

std::optional<http::request::RequestLine> parseRequestLine(std::string_view line);

// namespace http::parser

#endif // HTTP_PARSER_HPP
