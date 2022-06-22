#include "NumberConverter.h"

#include "Ut.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <cctype>
#include <functional>
#include <iterator>

namespace {
using Integer_t = boost::multiprecision::cpp_int;
using Rational_t = boost::multiprecision::cpp_rational;

Integer_t floor(const Rational_t& num)
{
    Integer_t num2 =
        boost::multiprecision::numerator(num) / boost::multiprecision::denominator(num);
    if(Rational_t(num2) > num)
        --num2;
    return num2;
}

Rational_t fract(const Rational_t& num)
{
    return num - floor(num);
}

const char DECIMAL_SEPARATOR =
    std::use_facet<std::numpunct<char>>(std::locale{}).decimal_point();

const auto DIGITS = [] {
    std::vector<char> ret;
    for(char c = '0'; c <= '9'; c++)
        ret.push_back(c);
    for(char c = 'a'; c <= 'z'; c++)
        ret.push_back(c);
    return ret;
}();

std::string_view getValidDigits(const std::size_t base)
{
    return {DIGITS.data(), base};
}

struct IntFractParts
{
    std::string_view intPart, fractPart;
};
IntFractParts parseIntFractPart(const std::string_view s, const std::size_t base1)
{
    std::size_t intPartBegin = 0, intPartLen = 0, fractPartBegin = 0, fractPartLen = 0;
    const auto  matches = [base1](const char c) {
        const auto jt = std::find(DIGITS.begin(), DIGITS.end(), c);
        const auto d = static_cast<std::size_t>(std::distance(DIGITS.begin(), jt));
        return jt != DIGITS.end() && d < base1;
    };
    enum State
    {
        EXPECT_SPACE_OR_DOT_OR_INT_PART_BEGIN,
        EXPECT_INT_PART_MIDDLE_OR_DOT,
        EXPECT_FRACT_PART_BEGIN,
        EXPECT_FRACT_PART_MIDDLE
    } state = EXPECT_SPACE_OR_DOT_OR_INT_PART_BEGIN;
    std::size_t i = 0;
    for(; i != s.size(); i++)
    {
        const auto c = static_cast<char>(std::tolower(s[i]));
        switch(state)
        {
        case EXPECT_SPACE_OR_DOT_OR_INT_PART_BEGIN:
        {
            if(std::isspace(c))
                // Not cnahge.
                ;
            else if(matches(c))
            {
                intPartBegin = i;
                intPartLen = 1;
                state = EXPECT_INT_PART_MIDDLE_OR_DOT;
            }
            else if(DECIMAL_SEPARATOR == c)
            {
                intPartBegin = i;
                intPartLen = 0;
                state = EXPECT_FRACT_PART_BEGIN;
            }
            else
                throw nsNumberConverter::ParserException{
                    ut::toString("Invalid character in integer part: '", c,
                        "'. Expecting space or decimal separator '", DECIMAL_SEPARATOR,
                        "' or one of valid digits: '", getValidDigits(base1), "'."),
                    i};
            break;
        }
        case EXPECT_INT_PART_MIDDLE_OR_DOT:
        {
            if(matches(c))
            {
                ++intPartLen;
                // Not cnahge.
            }
            else if(DECIMAL_SEPARATOR == c)
                state = EXPECT_FRACT_PART_BEGIN;
            else
                throw nsNumberConverter::ParserException{
                    ut::toString("Invalid character in integer part: '", c,
                        "'. Expecting decimal separator '", DECIMAL_SEPARATOR,
                        "' or one of valid digits: '", getValidDigits(base1), "'."),
                    i};
            break;
        }
        case EXPECT_FRACT_PART_BEGIN:
        {
            if(matches(c))
            {
                fractPartBegin = i;
                fractPartLen = 1;
                state = EXPECT_FRACT_PART_MIDDLE;
            }
            else
                throw nsNumberConverter::ParserException{
                    ut::toString("Invalid character in fractional part: '", c,
                        "'. Expecting one of valid digits: '", getValidDigits(base1),
                        "'."),
                    i};
            break;
        }
        case EXPECT_FRACT_PART_MIDDLE:
        {
            if(matches(c))
            {
                ++fractPartLen;
                // Not cnahge.
            }
            else
                throw nsNumberConverter::ParserException{
                    ut::toString("Invalid character in fractional part: '", c,
                        "\'. Expecting one of valid digits: '", getValidDigits(base1),
                        "'."),
                    i};
            break;
        }
        }
    }
    IntFractParts ret;
    ret.intPart = s.substr(intPartBegin, intPartLen);
    ret.fractPart = s.substr(fractPartBegin, fractPartLen);
    return ret;
}

std::string toStringBase2(Integer_t num, const std::size_t base2)
{
    std::string ret;
    if(!num)
        return "0";
    while(num)
    {
        Integer_t q, r;
        boost::multiprecision::divide_qr(num, Integer_t{base2}, q, r);
        const auto i = static_cast<std::size_t>(r);
        ret += DIGITS[i];
        num.swap(q);
    }
    std::reverse(ret.begin(), ret.end());
    return ret;
}

Integer_t toIntegerBase1(const std::string_view inputNumber, const std::size_t base1)
{
    Integer_t ret = 0;
    for(const char c : inputNumber)
    {
        const auto it = std::find(DIGITS.begin(), DIGITS.end(), c);
        const auto d = std::distance(DIGITS.begin(), it);
        ret = ret * base1 + d;
    }
    return ret;
}

std::string fractionPartToString(
    Rational_t num, const std::size_t base2, const std::size_t digitsAfterPoint)
{
    std::string res;
    for(std::size_t i = 0; i < digitsAfterPoint; i++)
    {
        num = fract(num) * base2;
        if(!num)
            break;
        const auto j = static_cast<std::size_t>(floor(num));
        res += DIGITS[j];
    }
    return res;
}

} // namespace

namespace nsNumberConverter {
// CLASS NumberConverter
std::string NumberConverter::operator()(const std::string_view inputNumber,
    const std::size_t base1, const std::size_t base2, const std::size_t digitsAfterPoint)
{
    m_base1 = base1;
    m_base2 = base2;
    m_digitsAfterPoint = digitsAfterPoint;

    if(std::min(m_base1, m_base2) < minBase() || std::max(m_base1, m_base2) > maxBase())
    {
        throw std::runtime_error(ut::toString(
            "Base of a number system must be from ", minBase(), " to ", maxBase(), "."));
    }

    const auto parsed = parseIntFractPart(inputNumber, m_base1);

    const Integer_t  intPart = toIntegerBase1(parsed.intPart, m_base1);
    const Rational_t fractPart{toIntegerBase1(parsed.fractPart, m_base1),
        boost::multiprecision::pow(
            Integer_t{m_base1}, static_cast<unsigned>(parsed.fractPart.size()))};

    std::string s = toStringBase2(intPart, m_base2);
    if(fractPart && m_digitsAfterPoint > 0)
    {
        std::string s2 = fractionPartToString(fractPart, m_base2, m_digitsAfterPoint);
        s += ut::toString(DECIMAL_SEPARATOR) + s2;
    }
    return s;
}

std::size_t NumberConverter::minBase() const
{
    return 2;
}

std::size_t NumberConverter::maxBase() const
{
    return DIGITS.size();
}

char NumberConverter::decimal_point()
{
    return DECIMAL_SEPARATOR;
}

// CLASS ParserException
ParserException::ParserException(const std::string& msg, const std::size_t pos)
    : m_msg{msg}
    , m_pos{pos}
{
}

const char* ParserException::what() const noexcept
{
    return m_msg.c_str();
}

std::size_t ParserException::pos() const
{
    return m_pos;
}

} // namespace nsNumberConverter
