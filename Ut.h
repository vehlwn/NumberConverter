#ifndef UT_H
#define UT_H

#include <string>
#include <sstream>

namespace ut
{
template<class ...Args>
std::string toString(const Args&... args)
{
	std::ostringstream os;
	(os << ... << args);
	return os.str();
}

}

#endif // UT_H
