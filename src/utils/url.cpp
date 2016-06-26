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
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>

namespace Wizrd {



std::string URL::encode(const Wizrd::params data) throw(URLEncodeError)
{
    std::string output;
    int size = 0;

    if(!data.size()) {
        return output;
    }

    for (auto param: data) {
        for(auto i = param.begin(); i < param.end(); i++) {
            size += i->length();
        }
    }
    output.reserve(size * 2);

    bool first = true;

    for (auto pair: data) {
        if(!first)
            output += '&';
        else
            first = false;
        if (pair.size() == 1)
            output += quotePlus(pair[0]);
        else if (pair.size() == 2) {
            output += encodePair(pair[0], pair[1]);
        }
        else {
            throw URLEncodeError("Data content item should only have 1 or 2"
                                 "items");
        }
    }
    output.shrink_to_fit();
    return output;
}

std::string URL::encode(paramsMap data)
{
    int size = 0;
    std::string output;
    if (!data.size()) {
        return std::string();
    }

    for (auto& kv: data) {
        size += kv.first.length() + kv.second.length();
    }
    output.reserve(size * 2);

    bool first = true;

    for(auto& kv: data) {
        if (!first)
            output += '&';
        else
            first = false;

        output += encodePair(kv.first, kv.second);
    }
    output.shrink_to_fit();
    return output;
}

params URL::decode(boost::string_ref url)
{
    using namespace boost;
    auto current{url.begin()};
    int current_n = 0;
    params ret;

    if (url.empty())
        return ret;
    size_t n = 0;
    boost::string_ref url_fragment;
    for (auto i: url) {
        if (i == '&') {
            url_fragment = url.substr(current_n, n);
            ret.push_back(std::move(decodePair(url_fragment)));
            current_n = (n + 1);
        }
        n++;
    }
    url_fragment = url.substr(current_n, n);
    ret.push_back(std::move(decodePair(url_fragment)));
    return ret;
}

std::map<std::string, std::string> URL::decodeMap(boost::string_ref url)
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


std::string URL::quote(boost::string_ref url, boost::string_ref safe)
{
    return std::move(quote_(url, safe, false));
}
std::string URL::quotePlus(boost::string_ref url, boost::string_ref safe)
{
    return std::move(quote_(url, safe, true));
}

std::string URL::unquote(boost::string_ref url)
{
    return unquote_(url, false);
}

std::string URL::unquotePlus(boost::string_ref url)
{
    return unquote_(url, true);
}


// === private ===

std::string URL::encodePair(boost::string_ref first, boost::string_ref second)
{
    std::string out{std::move(quotePlus(first))};
    out += '=';
    out += quotePlus(second);
    return out;
}

std::vector<std::string> URL::decodePair(boost::string_ref url)
{
    std::vector<std::string> out;
    out.reserve(2);
    size_t i = url.find_first_of('=');
    if (i == EOF)
    {
        out.push_back(std::move(unquotePlus(url)));
    }
    else {
        auto first{url.substr(0, i)};
        auto second{url.substr(i + 1, url.size())};
        out.push_back(unquotePlus(first));
        out.push_back(unquotePlus(second));
    }
    return out;
}


std::string URL::quote_(boost::string_ref url, boost::string_ref safe, bool plus)
{
    std::string output;
    output.reserve(url.length() * 2);
    for(auto i: url) {
        if(plus && i == ' ') {
            output += '+';
        }
        else if (validQuoteChar(i) || ((!safe.empty()) && i == safe[0])) {
            output += i;
        }
        else {
            output += '%';
            auto ret{std::move(quoteChar(i))};
            output += ret[0];
            output += ret[1];
        }
    }
    output.shrink_to_fit();
    return output;
}

std::string URL::unquote_(boost::string_ref url, bool plus)
{
    std::string output;
    output.reserve(url.length());

    for (auto i = url.begin(); i != url.end(); i++) {
        if (plus && (*i == '+')) {
            output += ' ';
        }
        else if (*i != '%') {
            output += *i;
        }
        else {
            char out, digit1, digit2;
            if (++i != url.end()) {
                digit1 = *i;
            }
            else {
                --i;
                output += '%';
                continue;
            }
            if (++i != url.end()) {
                digit2 = *i;
            }
            else {
                --i;
                --i;
                output += '%';
                continue;
            }
            if (isHexDigit(digit1))
            {
                if(isHexDigit(digit2)) {
                    out = (toHexNibble(digit1) << 4) | toHexNibble(digit2);
                    output += out;
                }
                else {
                    output += '%';
                    output += digit1;
                    i--;
                    continue;
                }
            }
            else {
                output += '%';
                i -= 2;
            }
        }
    }
    return output;
}

}
