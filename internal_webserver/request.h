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
#include <sstream>
#include <vector>
#include <ostream>

namespace Wizrd {
namespace Server {

using Header = std::vector<std::string>;


using Headers = std::vector<Header>;

enum class Method {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    OPTIONS,
    CONNECT,
    PATCH,
    CUSTOM
};

struct Request {
    std::string url;
    std::string host;
    std::string methodString;
    std::string versionString;
    bool keepAlive;
    int connectionTimeout;
    int versionMajor;
    int versionMinor;
    Method method;
    std::string contentType;
    int contentLenght;
    Headers headers;
    std::string data;
    inline std::string toString()
    {
        auto headerString = [](const Header& header) -> std::string {
            std::stringstream os;
            os << "{" << header[0];
            if (header.size() > 1)
            {
                os << ": " << header[1];
            }
            os << "}";
            return os.str();

        };
        auto headersString = [&]() -> std::string {
            std::stringstream os;
            os << "{";
            bool first = true;
            for(const Wizrd::Server::Header& header: headers) {
                if(first) {
                    os << headerString(header);
                    first = false;
                }
                else {
                    os << ", " << headerString(header);
                }
            }
            os << "}";
            return os.str();
        };

        std::stringstream os;
        os << "<Server::Request " << methodString << " " << url << ", "
           << "version: " << versionMajor << "." << versionMinor << ", "
           << "keepAlive: " << keepAlive << ", headers: " << headersString();
        return os.str();

    }
};

} // Server namespace
} // Wizrd namespace

template< typename CharT, typename TraitsT >
std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& os, Wizrd::Server::Header const& header)
{
    os << "{" << header[0];
    if (header.size() > 1)
    {
        os << ": " << header[1];
    }
    os << "}";
    return os;
}

template< typename CharT, typename TraitsT >
std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& os, Wizrd::Server::Headers const& headers)
{
    os << "{";
    bool first = true;
    for(const Wizrd::Server::Header& header: headers) {
        if(first) {
            os << header;
            first = false;
        }
        else {
            os << ", " << header;
        }
    }
    return os;
}
template< typename CharT, typename TraitsT >
std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& os, Wizrd::Server::Request const& req)
{
    os << "<Server::Request " << req.methodString << " " << req.url << ", "
       << "version: " << req.versionMajor << "." << req.versionMinor << ", "
       << "keepAlive: " << req.keepAlive << "headers: " << req.headers;
    return os;
}
