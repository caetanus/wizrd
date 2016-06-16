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
#include "exceptions.h"
#include <string>
#include <vector>

namespace Wizrd {

#define BASE64_BREAK_LINE 76
class Base64DecodeException: BaseException { using BaseException::BaseException;};

class Base64 {
public:
    static std::string encode(const std::string& data, bool breakLine = false);
    static std::string encode(const char* data, int size, bool breakLine = false);
    static std::string decode(std::string data) throw(Base64DecodeException);
    static std::string decode(const char* const data, int size) throw(Base64DecodeException);

private:
    static const std::vector<char> encodeTable_;
    static const std::vector<char> decodeTable_;
    Base64() = delete;
};


}
