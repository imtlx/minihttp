#include "http/parser.hpp"

#include <string>
#include <string_view>
#include <optional>

namespace http::parser {

std::optional<http::request::RequestLine>
parseRequestLine(std::string_view line) {
	if (!line.empty() && line.back() == '\n')
		line.remove_suffix(1);
	if (!line.empty() && line.back() == '\r')
		line.remove_suffix(1);

	// METHOD SP REQUEST-TARGET SP HTTP-VERSION

	const auto firstSpace = line.find(' ');
	if (firstSpace == std::string_view::npos || firstSpace == 0)
		return std::nullopt;

	const auto secondSpace = line.find(' ', firstSpace + 1);
	if (secondSpace == std::string_view::npos ||
	    secondSpace == firstSpace + 1 ||
	    secondSpace + 1 >= line.size())
		return std::nullopt;

	if (line.find(' ', secondSpace + 1) != std::string_view::npos)
		return std::nullopt;

	const auto method = line.substr(0, firstSpace);
	const auto target = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	const auto version = line.substr(secondSpace + 1);

	constexpr std::string_view separators = "()<>@,;:\\\"/[]?={}";
	for (unsigned char c : method) {
		if (c <= 32 || c >= 127 ||
		    separators.find(static_cast<char>(c)) != std::string_view::npos)
			return std::nullopt;
	}

	for (unsigned char c : target) {
		if (c <= 32 || c >= 127)
			return std::nullopt;
	}

	const auto parsedVersion = parseVersion(version);
	if (!parsedVersion)
		return std::nullopt;

	return http::request::RequestLine {
		http::request::parseMethod(method),
		std::string(target),
		*parsedVersion
	};
}

std::optional<http::request::RequestVersion>
parseVersion(std::string_view version) {
	if (version.size() != 8 ||
	    !version.starts_with("HTTP/") ||
	    version[6] != '.' ||
	    version[5] < '0' || version[5] > '9' ||
	    version[7] < '0' || version[7] > '9')
		return std::nullopt;

	return http::request::RequestVersion {
		static_cast<unsigned short>(version[5] - '0'),
		static_cast<unsigned short>(version[7] - '0')
	}
}

} // namespace http::parser
