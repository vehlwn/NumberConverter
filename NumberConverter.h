#pragma once

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstddef>
#include <exception>
#include <memory>
#include <sstream>
#include <string>

namespace nsNumberConverter
{
// CLASS NumberConverter
class NumberConverter
{
public:
    std::string operator()(const std::string_view inputNumber = "0",
        const std::size_t base1 = 10, const std::size_t base2 = 10,
        const std::size_t digitsAfterPoint = 0);

    std::size_t minBase() const;
    std::size_t maxBase() const;
    static char decimal_point();

private:
    std::size_t m_base1, m_base2, m_digitsAfterPoint;
};

// CLASS ParserException
class ParserException : std::exception
{
public:
    ParserException(const std::string& msg, const std::size_t pos);
    const char* what() const noexcept override;
    std::size_t pos() const;

private:
    std::string m_msg;
    std::size_t m_pos;
};

} // namespace nsNumberConverter
