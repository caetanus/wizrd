/*
 * Copyright (c) 2016 - Wizrd Team
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <initializer_list>
#include <map>
#include <iomanip>
#include <sstream>
#include <cstdio>
#include <boost/utility/string_ref.hpp>
#include "exceptions.h"


#ifndef EOF
#define EOF (~0)
#endif

namespace Wizrd {

typedef std::vector<std::vector<std::string>> params;
typedef std::map<std::string, std::string> paramsMap;

static const std::string empty = std::string();

class URLEncodeError: BaseException {
    using BaseException::BaseException;
};

class URL {
public:

    static std::string encode(const params data) throw(URLEncodeError);
    static std::string encode(paramsMap data);
    static params decode(boost::string_ref url);
    static std::map<std::string, std::string> decodeMap(boost::string_ref url);
    static std::string quote(boost::string_ref url, boost::string_ref safe = empty);
    static std::string quotePlus(boost::string_ref url, boost::string_ref safe = empty);
    static std::string unquote(boost::string_ref url);
    static std::string unquotePlus(boost::string_ref url);

private:
    URL() = delete;
    static std::string encodePair(boost::string_ref first, boost::string_ref second);
    static std::vector<std::string> decodePair(boost::string_ref url);
    static std::string quote_(boost::string_ref url, boost::string_ref safe, bool plus);
    static std::string unquote_(boost::string_ref url, bool plus);

    static inline bool validQuoteChar(char value) noexcept {
        return  ((value >= 'A' && value <= 'Z') ||
                 (value >= 'a' && value <= 'z') ||
                 (value >= '0' && value <= '9') ||
                 (value == '-') || (value == '_') ||
                 (value == '.') || (value == '/'));
    }
    static inline const std::string quoteChar(char c) noexcept {
        std::ostringstream sret;
            sret << std::hex
                 << std::uppercase
                 << std::setfill('0')
                 << std::setw(2)
                 << static_cast<int>(c);

            return sret.str();
    }
    static inline bool isHexDigit(char c) noexcept {
        char upperC = c & ~0x20;
        return ((upperC >= 'A' && upperC <= 'F') ||
                (c >= '0' && c <= '9'));
    }
    static inline char toHexNibble(char c) noexcept {
        char upperC = c & ~0x20;
        if (upperC >= 'A' && upperC <= 'F')
            return (upperC - 'A') + 0x0A;
        else
            return (c - '0');
    }

};

}
