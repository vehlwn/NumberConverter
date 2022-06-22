#include "NumberConverter.h"

#include "Ut.h"

#include <iterator>

namespace nsNumberConverter
{

ParserException::ParserException(const std::string &msg, int pos)
	: m_msg(msg), m_pos(pos)
{}

const char * ParserException::what() const noexcept
{
	return m_msg.c_str();
}

int ParserException::pos() const
{
	return m_pos;
}

const char NumberConverter::m_decimalSeparator = '.';

std::string NumberConverter::operator() (const std::string &inputNumber
		, const std::size_t base1
		, const std::size_t base2
		, const std::size_t digitsAfterPoint)
{
	m_base1 = base1;
	m_base2 = base2;
	m_digitsAfterPoint = digitsAfterPoint;
	
	if(std::min(m_base1, m_base2) < minBase() || std::max(m_base1, m_base2) > maxBase())
	{
		throw std::runtime_error(Ut::toString("Base of a numeral system must be from ", minBase(), " to ", maxBase(), "."));
	}

	const auto pa = parseIntFractPart(inputNumber);
	const auto &[intPartStr, fractPartStr] = pa;

	const Integer_t intPart = toInteger(intPartStr);
	const Rational_t fractPart(toInteger(fractPartStr)
			, boost::multiprecision::pow(Integer_t(m_base1)
				, static_cast<unsigned>(fractPartStr.size()))
			);
	
	std::string s = toStringBase2(intPart);
	if(!!fractPart && m_digitsAfterPoint > 0)
	{
		std::string s2 = fractionPartToString(fractPart);
		s += Ut::toString(m_decimalSeparator) + s2;
	}
	
	return s;
}

std::string NumberConverter::toStringBase2(Integer_t num) const
{
	std::string res;
	if(!num) return "0";
	while(!!num)
	{
		Integer_t q, r;
		boost::multiprecision::divide_qr(num, Integer_t(m_base2), q, r);
		res += DIGITS()[static_cast<std::size_t>(r)];
		num.swap(q);
	}
	std::reverse(res.begin(), res.end());
	return res;
}

NumberConverter::Integer_t NumberConverter::toInteger(const std::string &inputNumber) const
{
	//std::string temp1, temp2;

	Integer_t num = 0, num2 = 1;
	for(auto it = inputNumber.rbegin(); it != inputNumber.rend(); ++it)
	{
		const char c = *it;

		auto it2 = std::find(DIGITS().begin(), DIGITS().end(), c);
		const auto num3 = std::distance(DIGITS().begin(), it2);

		num += num2 * num3;
		num2 *= m_base1;

		//temp1 = to_string(num);
		//temp2 = to_string(num2);
	}
	
	return num;
}


NumberConverter::Rational_t NumberConverter::fract(Rational_t num)
{
	return num - floor(num);
}

NumberConverter::Integer_t NumberConverter::floor(Rational_t num)
{
	using namespace boost::multiprecision;
	Integer_t num2 = numerator(num) / denominator(num);
	if(Rational_t(num2) > num) --num2;
	return num2;
}

std::string NumberConverter::fractionPartToString(Rational_t num) const
{
	std::string res;

	//std::string temp1, temp2;
	for(std::size_t i = 0; i < m_digitsAfterPoint; i++)
	{
		//temp1 = to_string(num);

		num = fract(num) * m_base2;
		//temp2 = to_string(num);

		if(!num) break;
		const auto num3 = static_cast<std::size_t>(floor(num));
		res += DIGITS()[num3];
	}
	return res;
}

const std::vector<char>& NumberConverter::DIGITS()
{
	using Value_t = std::vector<char>;
	static std::unique_ptr<Value_t> vec;
	if(!vec)
	{
		vec = std::make_unique<Value_t>();
		for(int c = '0'; c <= '9'; c++) vec->push_back(c);
		for(int c = 'a'; c <= 'z'; c++) vec->push_back(c);
	}
	return *vec;
}

std::size_t NumberConverter::minBase() const
{
	return 2;
}

std::size_t NumberConverter::maxBase() const
{
	return DIGITS().size();
}

std::pair<std::string, std::string> NumberConverter::parseIntFractPart(std::string s) const
{
	std::string intPartStr, fractPartStr;
	s = boost::algorithm::to_lower_copy(s);
	auto it = s.begin();
	while(it != s.end() && *it != m_decimalSeparator)
	{
		auto it2 = std::find(DIGITS().begin(), DIGITS().end(), *it);
		const std::size_t num = std::distance(DIGITS().begin(), it2);
		if(it2 == DIGITS().end() || num >= m_base1) break;
		intPartStr += *it;
		++it;
	}
	if(it == s.end())
	{
		return {intPartStr, ""};
	}
	if(*it != m_decimalSeparator)
	{
		throw ParserException(
					Ut::toString("Invalid character \'", *it, "\'.")
					, std::distance(s.begin(), it));
	}
	++it;

	while(it != s.end())
	{
		auto it2 = std::find(DIGITS().begin(), DIGITS().end(), *it);
		const std::size_t num = std::distance(DIGITS().begin(), it2);
		if(it2 == DIGITS().end() || num >= m_base1) break;
		fractPartStr += *it;
		++it;
	}
	if(it == s.end())
	{
		return {intPartStr, fractPartStr};
	}
	else
	{
		throw ParserException(
					Ut::toString("Invalid character \'", *it, "\'.")
					, std::distance(s.begin(), it));
	}
}

} // namespace nsNumberConverter
