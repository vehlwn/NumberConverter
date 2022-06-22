#ifndef NUMBERCONVERTER_H
#define NUMBERCONVERTER_H 1


#include <algorithm>
#include <atomic>
#include <cctype>
#include <sstream>
#include <string>
#include <memory>
#include <exception>
#include <cstddef>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/algorithm/string.hpp>

namespace nsNumberConverter
{

class NumberConverter
{
public:
	std::string operator() (const std::string &inputNumber = "0"
			, const std::size_t base1 = 10
			, const std::size_t base2 = 10
			, const std::size_t digitsAfterPoint = 0);

	std::size_t minBase() const;
	std::size_t maxBase() const;

private:
	using Integer_t = boost::multiprecision::cpp_int;
	using Rational_t = boost::multiprecision::cpp_rational;

	static const std::vector<char>& DIGITS();
	static Rational_t fract(Rational_t num);
	static Integer_t floor(Rational_t num);
	std::pair<std::string, std::string> parseIntFractPart(std::string s) const;

	std::string toStringBase2(Integer_t num) const;
	Integer_t toInteger(const std::string &inputNumber) const;
	std::string fractionPartToString(Rational_t num) const;

	static const char m_decimalSeparator;

	std::size_t m_base1, m_base2, m_digitsAfterPoint;

};

class ParserException : std::exception
{
public:
	ParserException(const std::string &msg, int pos);
	const char * what() const noexcept override;
	int pos() const;

private:
	std::string m_msg;
	int m_pos;
};

} // namespace nsNumberConverter

#endif // NUMBERCONVERTER_H
