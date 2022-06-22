#pragma once

#include <sstream>
#include <string>

namespace ut
{
template<class... Args>
std::string toString(const Args&... args)
{
	std::ostringstream os;
	(os << ... << args);
	return os.str();
}

} // namespace ut
