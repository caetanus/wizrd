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

#include "base64.h"
#include <vector>
#include <boost/config.hpp>
#include <algorithm>
#include <memory>
#include <sstream>
#include <streambuf>
#include <iostream>
#include <type_traits>

namespace Wizrd {

enum {
    Whitespace = 64,
    Equals,
    Invalid
};


const std::vector<char> Base64::encodeTable_{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                             'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                             'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                             'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                             'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                             'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                             'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                             '4', '5', '6', '7', '8', '9', '+', '/'};

const std::vector<char> Base64::decodeTable_{66,66,66,66,66,66,66,66,66,64,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
                                             66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
                                             54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                             10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
                                             29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
                                             66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
                                             66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
                                             66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
                                             66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
                                             66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
                                             66,66,66,66,66,66};

std::string Base64::encode(const std::string& data, bool breakLine)
{
    std::vector<char> output;
    if (data.length() == 0)
        return "";
    size_t outputSize = (((data.length() / 3) + (data.length() % 3 > 0)) * 4L);

    if (BOOST_UNLIKELY(breakLine))
        outputSize += outputSize % BASE64_BREAK_LINE + 1;

    output.reserve(outputSize);
    std::stringbuf pdata{data};
    size_t current = 0;
    size_t group24;
    auto append = [&output, &breakLine, &current](char v) {
        output.push_back(v);
        if (BOOST_UNLIKELY(breakLine) && !(++current % BASE64_BREAK_LINE))
            output.push_back('\n');
    };

    while (pdata.in_avail() > 2) {

        auto d1 = pdata.sbumpc(),
             d2 = pdata.sbumpc(),
             d3 = pdata.sbumpc();

        group24 = (d1 << 16) | (d2 << 8) | d3;
        const int foo = group24 & 0x00FC0000;
        append(encodeTable_[group24 >> 18 & 0x3f]);
        append(encodeTable_[group24 >> 12 & 0x3f]);
        append(encodeTable_[group24 >> 6 & 0x3f]);
        append(encodeTable_[group24 & 0x3f]);
    }
    switch (pdata.in_avail() % 3) {
    case 1:
        group24 = (pdata.sbumpc() << 16);
        append(encodeTable_[(group24 >> 18) & 0x3f]);
        append(encodeTable_[(group24 >> 12) & 0x3f]);
        append('=');
        append('=');
        break;
    case 2:
        group24 = pdata.sbumpc() << 16 |
                  pdata.sbumpc() << 8;
        append(encodeTable_[(group24 & 0x00FC0000) >> 18]);
        append(encodeTable_[(group24 & 0x0003F000) >> 12]);
        append(encodeTable_[(group24 & 0x00000FC0) >> 6]);
        append('=');
        break;
    }
    if (BOOST_UNLIKELY(breakLine && output[output.size() - 1] != '\n') )
        output.push_back('\n');

    return std::string(output.begin(), output.end());
}

std::string Base64::encode(const char* data, int size, bool breakLine)
{
    return encode(std::string(data, size), breakLine);
}

std::string Base64::decode(std::string data) throw(Base64DecodeException)
{

    std::vector<char> output;
    output.reserve(((data.length() / 4) * 3));
    size_t group24 = 0;
    size_t count = 0;
    size_t padding = 0;
    auto i = data.begin();
    while (i < data.end()) {
        // for speed using a pre computed decode table
        for (int qpos = 0; qpos < 4; qpos++) {
            char c;
            if (i >= data.end())
                break;
            c = decodeTable_[*i++];
            if (output.size() > 2048)
            {
                char bar = ' ';
            }

            switch (c) {

            case Whitespace:
                qpos--;
                if (i >= data.end())
                    goto out;
                continue;
            case Invalid:
                throw Base64DecodeException("Non Valid character in Base64");
                break;
            case Equals:
                padding++;
                count++;
                if (i >= data.end())
                    goto out;
                break;

            default:
                if(padding)
                    throw Base64DecodeException("Invalid Padding in Base64");

                count++;
                group24 = (group24 << 6) | c;

            }
        }
        output.push_back((group24 >> 16) & 0xFF);
        output.push_back((group24 >> 8) & 0xFF);
        output.push_back((group24) & 0xFF);

    }
    out:
    if (count % 4)
        throw Base64DecodeException("Invalid Base64 length");
    switch (padding) {
    case 0:
        break;
    case 1:
        output.push_back((group24 >> 10) & 0xFF);
        output.push_back((group24 >> 2) & 0xFF);
        break;
    case 2:
        output.push_back((group24 >> 4) & 0xff);
        break;
    default:
        throw Base64DecodeException("Invalid Padding in Base64");
    }

    ret:
    return std::string(output.begin(), output.end());
}

std::string Base64::decode(const char* const data, int size) throw(Base64DecodeException)
{

}

}
