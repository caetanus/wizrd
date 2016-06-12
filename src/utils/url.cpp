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

#include "url.h"
#include <iostream>
#include <sstream>
#include <string>
#include <boost/log/trivial.hpp>

namespace Wizrd {



std::string URL::encode(const Wizrd::params data) throw(URLEncodeError)
{
    std::stringstream output;
    if(!data.size()) {
        return std::string();
    }

    for (auto pair: data) {
        if (pair.size() == 1)
            output << quotePlus(pair[0]) << '&';
        else if (pair.size() == 2) {
            output << encodePair(pair[0], pair[1]) << '&';
        }
        else {
            throw URLEncodeError("Data content item should only have 1 or 2"
                                 "items");
        }
    }
    auto v{std::move(output.str())};
    return std::string(v.begin(), v.end() - 1);
}

std::string URL::encode(paramsMap data)
{
    std::stringstream output;
    if (!data.size()) {
        return std::string();
    }
    for(auto& kv: data) {
        output << encodePair(kv.first, kv.second) << '&';
    }
    auto v{std::move(output.str())};
    return std::string(v.begin(), v.end() - 1);
}

params URL::decode(const std::string& url)
{
    auto current{url.begin()};
    params ret;
    if (url.empty())
        return ret;
    for (auto i{url.begin()}; i != url.end(); i++) {
        if (*i == '&') {
            ret.push_back(std::move(decodePair(std::string(current, i))));
            current = (i + 1);
        }
    }
    ret.push_back(std::move(decodePair(std::string(current, url.end()))));
    return ret;
}

std::map<std::string, std::string> URL::decodeMap(const std::string& url)
{
    params data{std::move(decode(url))};
    std::map<std::string, std::string> output;

    for(auto item: data) {
        if (item.size() == 1) {
            output[item[0]] = "";
        }
        else {
            output[item[0]] = item[1];
        }
    }
    return output;
}


std::string URL::quote(const std::string& url, const std::string& safe)
{
    return std::move(quote_(std::move(url), std::move(safe), false));
}
std::string URL::quotePlus(const std::string& url, const std::string& safe)
{
    return std::move(quote_(std::move(url), std::move(safe), true));
}

std::string URL::unquote(const std::string& url)
{
    return unquote_(url, false);
}

std::string URL::unquotePlus(const std::string& url)
{
    return unquote_(std::move(url), true);
}


// === private ===

std::string URL::encodePair(const std::string& first, const std::string& second)
{
    std::stringstream data;
    data << quotePlus(first) << '=' << quotePlus(second);
    return data.str();
}

std::vector<std::string> URL::decodePair(const std::string& url)
{
    std::vector<std::string> out;
    out.reserve(2);
    size_t i = url.find_first_of('=');
    if (i == EOF)
    {
        out.push_back(unquotePlus(url));
    }
    else {
        auto iterator = url.begin() + i;
        auto first{std::string(url.begin(), iterator)};
        auto second{std::string(iterator + 1, url.end())};
        out.push_back(unquotePlus(first));
        out.push_back(unquotePlus(second));
    }
    return out;
}


std::string URL::quote_(const std::string& url, const std::string& safe, bool plus)
{
    std::vector<char> output;
    output.reserve(url.length() * 2);
    for(auto i: url) {
        if(plus && i == ' ') {
            output.push_back('+');
        }
        else if (validQuoteChar(i) || ((!safe.empty()) && i == safe[0])) {
            output.push_back(i);
        }
        else {
            output.push_back('%');
            auto ret{std::move(quoteChar(i))};
            output.push_back(ret[0]);
            output.push_back(ret[1]);
        }
    }
    return std::string(output.begin(), output.end());
}

std::string URL::unquote_(const std::string& url, bool plus)
{
    std::vector<char> output;
    output.reserve(url.length());

    for (auto i = url.begin(); i != url.end(); i++) {
        if (plus && (*i == '+')) {
            output.push_back(' ');
        }
        else if (*i != '%') {
            output.push_back(*i);
        }
        else {
            char out, digit1, digit2;
            if (++i != url.end()) {
                digit1 = *i;
            }
            else {
                --i;
                output.push_back('%');
                continue;
            }
            if (++i != url.end()) {
                digit2 = *i;
            }
            else {
                --i;
                --i;
                output.push_back('%');
                continue;
            }
            if (isHexDigit(digit1))
            {
                if(isHexDigit(digit2)) {
                    out = (toHexNibble(digit1) << 4) | toHexNibble(digit2);
                    output.push_back(out);
                }
                else {
                    output.push_back('%');
                    output.push_back(digit1);
                    i--;
                    continue;
                }
            }
            else {
                output.push_back('%');
                i -= 2;
            }
        }
    }
    return std::string(output.begin(), output.end());
}

}
