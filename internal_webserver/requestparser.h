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
#include <sstream>
#include "request.h"

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <tuple>

namespace Wizrd { namespace Server {

class RequestParser
{
public:
    RequestParser();
    void reset();
    enum ResultType {Ok, Error, Processing};

    // this parser works this way because read some has no guarantee to
    // get all available data on request, so, that way the request is parsed partially
    template <class Iterator>
    std::tuple<Iterator, ResultType> parse(Request &request, Iterator begin,
                                           Iterator end)
    {
        ResultType result;
        while (begin != end) {
            result = consume(request, *begin++);
        }
        if (result == Processing) {
            result = Ok;
            switch (state_) {
            case Data:
                request.data = std::move(currentBuffer_);
                break;
            case NewLine2:
                if (currentBuffer_ != "\r\n\r\n")
                    result = Processing;

                break;
            default:
                result = Processing;
            }
        }
        return std::make_tuple(result, begin);
    }
    void reset(Request &request);

private:
    ResultType consume(Request& request, char chr);
    ResultType consumeHeaders(Request& request, char chr);
    inline bool isLowerAlpha(const char chr) noexcept
    {
        return (chr >= 'a' && chr <= 'z');
    }
    inline bool isUpperAlpha(const char chr) noexcept
    {
        return (chr >= 'A' && chr <= 'Z');
    }
    inline bool isAlpha(const char chr) noexcept
    {
        return (isUpperAlpha(chr) || isLowerAlpha(chr));
    }
    inline bool isDigit(const char chr) noexcept
    {
        return (chr >= '0' && chr <= '9');
    }
    inline bool isFloat(const char chr) noexcept
    {
        return isDigit(chr) || chr == '.';
    }
    inline bool isSlash(const char chr) noexcept
    {
        return chr == '/';
    }
    inline bool isSpace(const char chr) noexcept
    {
        return chr == ' ';
    }
    bool isNewLine(const char chr) noexcept
    {
        return (chr == '\n' || chr == '\r');
    }
    bool isComma(const char chr) noexcept
    {
        return chr == ':';
    }

    enum {
        Start,
        Method,
        Space_1,
        Url,
        Space_2,
        Http,
        Version,
        NewLine,
        Headers,
        NewLine2,
        Data
    } state_;
    enum {
        HeaderStart,
        Key,
        Space,
        Value,
        HeaderNewLine
    } headerState_;
    int consumedContent_;


    std::string currentBuffer_;
};

}}
